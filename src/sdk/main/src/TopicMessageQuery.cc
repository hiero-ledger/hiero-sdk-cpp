// SPDX-License-Identifier: Apache-2.0
#include "TopicMessageQuery.h"
#include "Client.h"
#include "Defaults.h"
#include "SubscriptionHandle.h"
#include "TopicId.h"
#include "TopicMessage.h"
#include "impl/MirrorNetwork.h"
#include "impl/MirrorNode.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <chrono>
#include <grpcpp/grpcpp.h>
#include <mirror/consensus_service.grpc.pb.h>
#include <thread>
#include <unordered_map>

namespace Hiero
{
namespace
{
// Enum to track the status of the gRPC call.
enum class CallStatus
{
  STATUS_CREATE = 0,
  STATUS_PROCESSING = 1,
  STATUS_FINISH = 2
};

// Helper function used to get a connected mirror node.
std::shared_ptr<internal::MirrorNode> getConnectedMirrorNode(const std::shared_ptr<internal::MirrorNetwork>& network)
{
  std::shared_ptr<internal::MirrorNode> node = network->getNextMirrorNode();
  while (node->channelFailedToConnect())
  {
    node = network->getNextMirrorNode();
  }

  return node;
}

// Subscriber function called by TopicMessageQuery::subscribe().
void startSubscription(
  const std::shared_ptr<internal::MirrorNetwork>& network,
  std::unique_ptr<grpc::ClientAsyncReader<com::hedera::mirror::api::proto::ConsensusTopicResponse>> reader,
  std::unique_ptr<grpc::ClientContext> context,
  std::unique_ptr<grpc::CompletionQueue> queue,
  std::unique_ptr<CallStatus> callStatus,
  com::hedera::mirror::api::proto::ConsensusTopicQuery query,
  std::function<void(grpc::Status)> errorHandler,
  std::function<bool(grpc::Status)> retryHandler,
  std::function<void(void)> completionHandler,
  std::function<void(const TopicMessage&)> onNext,
  uint32_t maxAttempts,
  std::chrono::system_clock::duration maxBackoff,
  std::shared_ptr<SubscriptionHandle> handle)
{
  // Grab moved objects.
  std::vector<std::unique_ptr<grpc::ClientContext>> contexts;
  std::vector<std::unique_ptr<grpc::CompletionQueue>> queues;
  contexts.push_back(std::move(context));
  queues.push_back(std::move(queue));

  handle->setOnUnsubscribe([&contexts]() { contexts.back()->TryCancel(); });

  // Declare needed variables.
  com::hedera::mirror::api::proto::ConsensusTopicResponse response;
  std::unordered_map<TransactionId, std::vector<com::hedera::mirror::api::proto::ConsensusTopicResponse>>
    pendingMessages;
  std::chrono::system_clock::duration backoff = DEFAULT_MIN_BACKOFF;
  grpc::Status grpcStatus;
  uint64_t attempt = 0ULL;
  bool complete = false;
  bool ok = false;
  void* tag;

  // Loop until the RPC completes.
  while (true)
  {
    // Process based on the completion queue status.
    switch (queues.back()->AsyncNext(
      &tag, &ok, std::chrono::system_clock::now() + std::chrono::duration_cast<std::chrono::milliseconds>(backoff)))
    {
      case grpc::CompletionQueue::TIMEOUT:
      {
        // Backoff if the completion queue timed out.
        backoff = (backoff * 2 > maxBackoff) ? maxBackoff : backoff * 2;
        break;
      }
      case grpc::CompletionQueue::GOT_EVENT:
      {
        // Decrease the backoff time.
        backoff = (backoff / 2 < DEFAULT_MIN_BACKOFF) ? DEFAULT_MIN_BACKOFF : backoff / 2;

        // Process based on the call status.
        switch (static_cast<CallStatus>(*internal::Utilities::toTypePtr<long>(tag)))
        {
          case CallStatus::STATUS_CREATE:
          {
            if (ok)
            {
              // Update the call status to processing.
              *callStatus = CallStatus::STATUS_PROCESSING;
            }

            // Let fall through to process message.
            [[fallthrough]];
          }
          case CallStatus::STATUS_PROCESSING:
          {
            // If the response should be processed, process it.
            if (ok)
            {
              // Read the response.
              reader->Read(&response, callStatus.get());

              // Adjust the query timestamp and limit, in case a retry is triggered.
              if (response.has_consensustimestamp())
              {
                // Add one of the smallest denomination of time this machine can handle.
                query.set_allocated_consensusstarttime(internal::TimestampConverter::toProtobuf(
                  internal::TimestampConverter::fromProtobuf(response.consensustimestamp()) +
                  std::chrono::duration<int, std::ratio<1, std::chrono::system_clock::period::den>>()));
              }

              if (query.limit() > 0ULL)
              {
                query.set_limit(query.limit() - 1ULL);
              }

              // Process the received message.
              if (!response.has_chunkinfo() || response.chunkinfo().total() == 1)
              {
                onNext(TopicMessage::ofSingle(response));
              }
              else
              {
                const TransactionId transactionId =
                  TransactionId::fromProtobuf(response.chunkinfo().initialtransactionid());
                pendingMessages[transactionId].push_back(response);

                if (pendingMessages[transactionId].size() == response.chunkinfo().total())
                {
                  onNext(TopicMessage::ofMany(pendingMessages[transactionId]));
                }
              }
            }

            // If the response shouldn't be processed (due to completion or error), finish the RPC.
            else
            {
              *callStatus = CallStatus::STATUS_FINISH;
              reader->Finish(&grpcStatus, callStatus.get());
            }

            break;
          }
          case CallStatus::STATUS_FINISH:
          {
            if (grpcStatus.ok())
            {
              // RPC completed successfully.
              completionHandler();

              // Shutdown the completion queue.
              queues.back()->Shutdown();

              // Mark the RPC as complete.
              complete = true;
              break;
            }
            else
            {
              // An error occurred. Whether retrying or not, cancel the call and close the queue.
              contexts.back()->TryCancel();
              queues.back()->Shutdown();

              if (attempt >= maxAttempts || !retryHandler(grpcStatus))
              {
                // This RPC call shouldn't be retried, handle the error and mark as complete to exit.
                errorHandler(grpcStatus);
                complete = true;
              }
            }

            break;
          }
          default:
          {
            // Unrecognized call status, do nothing for now (not sure if this is correct).
            break;
          }
        }

        break;
      }
      case grpc::CompletionQueue::SHUTDOWN:
      {
        // Getting here means the RPC is reached completion or encountered an un-retriable error, and the completion
        // queue has been shut down. End the subscription.
        if (complete)
        {
          // Give a second for the queue to finish its processing.
          std::this_thread::sleep_for(std::chrono::seconds(1));
          return;
        }

        // If the completion queue has been shut down and the RPC hasn't completed, that means the RPC needs to be
        // retried. Increase the backoff for the retry.
        backoff = (backoff * 2 > maxBackoff) ? maxBackoff : backoff * 2;
        std::this_thread::sleep_for(backoff);
        ++attempt;

        // Resend the query to a different node with a different completion queue and client context.
        contexts.push_back(std::make_unique<grpc::ClientContext>());
        queues.push_back(std::make_unique<grpc::CompletionQueue>());

        // Reset the call status and send the query.
        *callStatus = CallStatus::STATUS_CREATE;
        reader = getConnectedMirrorNode(network)->getConsensusServiceStub()->AsyncsubscribeTopic(
          contexts.back().get(), query, queues.back().get(), callStatus.get());

        break;
      }
      default:
      {
        // Not sure what to do here, just fail out.
        std::cout << "Unknown gRPC completion queue event, failing.." << std::endl;
        return;
      }
    }
  }
}

} // namespace

// Implementation object for TopicMessageQuery.
struct TopicMessageQuery::TopicMessageQueryImpl
{
  // The message query to be sent.
  com::hedera::mirror::api::proto::ConsensusTopicQuery mQuery;

  // The maximum amount of attempts to try and retrieve messages.
  uint32_t mMaxAttempts = DEFAULT_MAX_ATTEMPTS;

  // The maximum amount of time to wait between submission attempts.
  std::chrono::system_clock::duration mMaxBackoff = DEFAULT_MAX_BACKOFF;

  // The function to run when there's an error.
  std::function<void(grpc::Status)> mErrorHandler = [](const grpc::Status& status)
  { std::cout << "Subscription error: " << status.error_message() << std::endl; };

  // The function to run when a retry is required.
  std::function<bool(grpc::Status)> mRetryHandler = [](const grpc::Status& status)
  {
    return (status.error_code() == grpc::StatusCode::NOT_FOUND) ||
           (status.error_code() == grpc::StatusCode::RESOURCE_EXHAUSTED) ||
           (status.error_code() == grpc::StatusCode::UNAVAILABLE) ||
           (status.error_code() == grpc::StatusCode::INTERNAL);
  };

  // The function to run when streaming is complete.
  std::function<void(void)> mCompletionHandler = []() { std::cout << "RPC subscription complete!" << std::endl; };
};

//-----
TopicMessageQuery::TopicMessageQuery()
  : mImpl(std::make_unique<TopicMessageQueryImpl>())
{
}

//-----
TopicMessageQuery::~TopicMessageQuery() = default;

//-----
TopicMessageQuery::TopicMessageQuery(const TopicMessageQuery& other)
  : TopicMessageQuery()
{
  mImpl->mQuery = other.mImpl->mQuery;
  mImpl->mMaxAttempts = other.mImpl->mMaxAttempts;
  mImpl->mMaxBackoff = other.mImpl->mMaxBackoff;
}

//-----
TopicMessageQuery& TopicMessageQuery::operator=(const TopicMessageQuery& other)
{
  if (this != &other)
  {
    mImpl->mQuery = other.mImpl->mQuery;
    mImpl->mMaxAttempts = other.mImpl->mMaxAttempts;
    mImpl->mMaxBackoff = other.mImpl->mMaxBackoff;
  }

  return *this;
}

//-----
TopicMessageQuery::TopicMessageQuery(TopicMessageQuery&& other) noexcept
  : mImpl(std::move(other.mImpl))
{
  // Leave moved-from TopicMessageQuery in valid state.
  other.mImpl = std::make_unique<TopicMessageQueryImpl>();
}

//-----
TopicMessageQuery& TopicMessageQuery::operator=(TopicMessageQuery&& other) noexcept
{
  if (this != &other)
  {
    mImpl = std::move(other.mImpl);

    // Leave moved-from TopicMessageQuery in valid state.
    other.mImpl = std::make_unique<TopicMessageQueryImpl>();
  }

  return *this;
}

//-----
std::shared_ptr<SubscriptionHandle> TopicMessageQuery::subscribe(const Client& client,
                                                                 const std::function<void(const TopicMessage&)>& onNext)
{
  // Create the subscription handle and assign its unsubscribe function to cancel this TopicMessageQuery's context
  // (which will cancel the gRPC call).
  auto handle = std::make_shared<SubscriptionHandle>();

  // Set up the subscription before passing it to the listening thread.
  auto context = std::make_unique<grpc::ClientContext>();
  auto queue = std::make_unique<grpc::CompletionQueue>();
  auto callStatus = std::make_unique<CallStatus>();

  // Send the query and initiate the subscription.
  std::thread(&startSubscription,
              client.getClientMirrorNetwork(),
              getConnectedMirrorNode(client.getClientMirrorNetwork())
                ->getConsensusServiceStub()
                ->AsyncsubscribeTopic(context.get(), mImpl->mQuery, queue.get(), callStatus.get()),
              std::move(context),
              std::move(queue),
              std::move(callStatus),
              mImpl->mQuery,
              mImpl->mErrorHandler,
              mImpl->mRetryHandler,
              mImpl->mCompletionHandler,
              onNext,
              mImpl->mMaxAttempts,
              mImpl->mMaxBackoff,
              handle)
    .detach();

  return handle;
}

//-----
TopicMessageQuery& TopicMessageQuery::setTopicId(const TopicId& topicId)
{
  mImpl->mQuery.set_allocated_topicid(topicId.toProtobuf().release());
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setStartTime(const std::chrono::system_clock::time_point& start)
{
  mImpl->mQuery.set_allocated_consensusstarttime(internal::TimestampConverter::toProtobuf(start));
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setEndTime(const std::chrono::system_clock::time_point& end)
{
  mImpl->mQuery.set_allocated_consensusendtime(internal::TimestampConverter::toProtobuf(end));
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setLimit(uint64_t limit)
{
  mImpl->mQuery.set_limit(limit);
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setMaxAttempts(uint32_t attempts)
{
  mImpl->mMaxAttempts = attempts;
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setMaxBackoff(const std::chrono::system_clock::duration& backoff)
{
  mImpl->mMaxBackoff = backoff;
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setErrorHandler(const std::function<void(grpc::Status)>& func)
{
  mImpl->mErrorHandler = func;
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setRetryHandler(const std::function<bool(grpc::Status)>& func)
{
  mImpl->mRetryHandler = func;
  return *this;
}

//-----
TopicMessageQuery& TopicMessageQuery::setCompletionHandler(const std::function<void()>& func)
{
  mImpl->mCompletionHandler = func;
  return *this;
}

//-----
TopicId TopicMessageQuery::getTopicId() const
{
  return TopicId::fromProtobuf(mImpl->mQuery.topicid());
}

//-----
std::chrono::system_clock::time_point TopicMessageQuery::getStartTime() const
{
  return internal::TimestampConverter::fromProtobuf(mImpl->mQuery.consensusstarttime());
}

//-----
std::chrono::system_clock::time_point TopicMessageQuery::getEndTime() const
{
  return internal::TimestampConverter::fromProtobuf(mImpl->mQuery.consensusendtime());
}

//-----
uint64_t TopicMessageQuery::getLimit() const
{
  return mImpl->mQuery.limit();
}

//-----
uint32_t TopicMessageQuery::getMaxAttempts() const
{
  return mImpl->mMaxAttempts;
}

//-----
std::chrono::system_clock::duration TopicMessageQuery::getMaxBackoff() const
{
  return mImpl->mMaxBackoff;
}

} // namespace Hiero
