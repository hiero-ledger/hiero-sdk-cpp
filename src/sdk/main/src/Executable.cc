// SPDX-License-Identifier: Apache-2.0
#include "Executable.h"
#include "AccountAllowanceApproveTransaction.h"
#include "AccountAllowanceDeleteTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "AccountRecords.h"
#include "AccountRecordsQuery.h"
#include "AccountStakersQuery.h"
#include "AccountUpdateTransaction.h"
#include "Client.h"
#include "ContractByteCodeQuery.h"
#include "ContractCallQuery.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractFunctionResult.h"
#include "ContractInfo.h"
#include "ContractInfoQuery.h"
#include "ContractUpdateTransaction.h"
#include "EthereumTransaction.h"
#include "FileAppendTransaction.h"
#include "FileContentsQuery.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "FileUpdateTransaction.h"
#include "FreezeTransaction.h"
#include "NetworkVersionInfo.h"
#include "NetworkVersionInfoQuery.h"
#include "NodeCreateTransaction.h"
#include "NodeDeleteTransaction.h"
#include "NodeUpdateTransaction.h"
#include "PrngTransaction.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleDeleteTransaction.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "ScheduleSignTransaction.h"
#include "SystemDeleteTransaction.h"
#include "SystemUndeleteTransaction.h"
#include "TokenAirdropTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenBurnTransaction.h"
#include "TokenCancelAirdropTransaction.h"
#include "TokenClaimAirdropTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenDissociateTransaction.h"
#include "TokenFeeScheduleUpdateTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TokenGrantKycTransaction.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenMintTransaction.h"
#include "TokenNftInfo.h"
#include "TokenNftInfoQuery.h"
#include "TokenPauseTransaction.h"
#include "TokenRejectTransaction.h"
#include "TokenRevokeKycTransaction.h"
#include "TokenUnfreezeTransaction.h"
#include "TokenUnpauseTransaction.h"
#include "TokenUpdateNftsTransaction.h"
#include "TokenUpdateTransaction.h"
#include "TokenWipeTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TopicMessageSubmitTransaction.h"
#include "TopicUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionRecordQuery.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "exceptions/MaxAttemptsExceededException.h"
#include "exceptions/PrecheckStatusException.h"
#include "impl/Network.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <algorithm>
#include <grpcpp/impl/codegen/status.h>
#include <limits>
#include <query.pb.h>
#include <response.pb.h>
#include <stdexcept>
#include <thread>
#include <transaction.pb.h>
#include <transaction_response.pb.h>

namespace Hiero
{
//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkResponseType Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::execute(
  const Client& client)
{
  return execute(client, client.getRequestTimeout());
}
//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkResponseType Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::execute(
  const Client& client,
  const std::chrono::system_clock::duration& timeout)
{
  if (mLogger.getLogger()->getName() == DEFAULT_LOGGER_NAME)
  {
    mLogger = client.getLogger();
  }

  setExecutionParameters(client);
  onExecute(client);

  // Get the nodes associated with this Executable's node account IDs.
  const std::vector<std::shared_ptr<internal::Node>> nodes = getNodesFromNodeAccountIds(client);

  // The time to timeout.
  const std::chrono::system_clock::time_point timeoutTime = std::chrono::system_clock::now() + timeout;

  // Keep track of the responses from each node.
  std::unordered_map<std::shared_ptr<internal::Node>, Status> nodeResponses;

  for (unsigned int attempt = 0U;; ++attempt)
  {
    // Get the timeout for the current attempt.
    std::chrono::system_clock::time_point attemptTimeout = std::chrono::system_clock::now() + mCurrentGrpcDeadline;
    if (attemptTimeout > timeoutTime)
    {
      attemptTimeout = timeoutTime;
    }

    if (attempt >= mCurrentMaxAttempts)
    {
      throw MaxAttemptsExceededException(
        "Max number of attempts made (max attempts allowed: " + std::to_string(mCurrentMaxAttempts) + ')');
    }

    const unsigned int nodeIndex = getNodeIndexForExecute(nodes, attempt);
    const std::shared_ptr<internal::Node>& node = nodes.at(nodeIndex);

    // If the returned node is not healthy, then no nodes are healthy and the returned node has the shortest remaining
    // delay. Sleep for the delay period.
    if (!node->isHealthy())
    {
      std::this_thread::sleep_for(node->getRemainingTimeForBackoff());
    }

    // Make sure the Node is connected. If it can't connect, mark this Node as unhealthy and try another Node.
    if (node->channelFailedToConnect())
    {
      mLogger.trace("Failed to connect to node " + node->getAccountId().toString() + " at address " +
                    node->getAddress().toString() + " on attempt " + std::to_string(attempt));
      mLogger.warn(
        "Retrying in " +
        std::to_string(
          std::chrono::duration_cast<std::chrono::milliseconds>(node->getRemainingTimeForBackoff()).count()) +
        " ms after channel connection failure with node " + node->getAccountId().toString() + " during attempt #" +
        std::to_string(attempt));
      node->increaseBackoff();
      continue;
    }

    // Create the request based on the index of the node being used.
    ProtoRequestType request = makeRequest(nodeIndex);
    if (mRequestListener)
    {
      request = mRequestListener(request);
    }

    // Submit the request and get the response.
    ProtoResponseType response;
    const grpc::Status status = submitRequest(request, node, attemptTimeout, &response);

    mLogger.trace("Execute request submitted to node " + node->getAccountId().toString() +
                  " attempt: " + std::to_string(attempt));

    // Increase backoff for this node but try submitting again for UNAVAILABLE, RESOURCE_EXHAUSTED, and INTERNAL
    // responses.
    if (const grpc::StatusCode errorCode = status.error_code(); errorCode == grpc::StatusCode::UNAVAILABLE ||
                                                                errorCode == grpc::StatusCode::RESOURCE_EXHAUSTED ||
                                                                errorCode == grpc::StatusCode::INTERNAL)
    {
      node->increaseBackoff();
      continue;
    }

    // Successful submission, so decrease backoff for this node.
    node->decreaseBackoff();

    // Call the response callback if one exists.
    if (mResponseListener)
    {
      response = mResponseListener(response);
    }

    // Grab and save the response status, and determine what to do next.
    const Status responseStatus = mapResponseStatus(response);
    nodeResponses[node] = responseStatus;

    mLogger.trace(std::string("Received ") + gStatusToString.at(responseStatus) + " response from node " +
                  node->getAccountId().toString() + " during attempt #" + std::to_string(attempt));

    switch (determineStatus(responseStatus, client, response))
    {
      case ExecutionStatus::SERVER_ERROR:
      {
        mLogger.warn("Problem submitting request to node " + node->getAccountId().toString() + " for attempt #" +
                     std::to_string(attempt) + ", retry with new node: " + gStatusToString.at(responseStatus));

        // If all nodes have returned a BUSY signal, backoff (just fallthrough to ExecutionStatus::RETRY case).
        // Otherwise, try the next node.
        if (nodeResponses.size() != nodes.size() ||
            !std::all_of(nodeResponses.cbegin(),
                         nodeResponses.cend(),
                         [](const auto& nodeAndStatus) { return nodeAndStatus.second == Status::BUSY; }))
        {
          continue;
        }

        // If all nodes have returned BUSY, clear the responses.
        nodeResponses.clear();
        [[fallthrough]];
      }
      // Response isn't ready yet from the network
      case ExecutionStatus::RETRY:
      {
        mLogger.warn("Retrying in " +
                     std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(mCurrentBackoff).count()) +
                     " ms after failure with node " + node->getAccountId().toString() + " during attempt #" +
                     std::to_string(attempt));

        std::this_thread::sleep_for(mCurrentBackoff);
        mCurrentBackoff *= 2.0;
        if (mCurrentBackoff > mCurrentMaxBackoff)
        {
          mCurrentBackoff = mCurrentMaxBackoff;
        }

        break;
      }
      case ExecutionStatus::REQUEST_ERROR:
      {
        throw PrecheckStatusException(responseStatus, getTransactionIdInternal());
      }
      default:
      {
        return mapResponse(response);
      }
    }
  }
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
std::future<SdkResponseType>
Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::executeAsync(const Client& client)
{
  return executeAsync(client, client.getRequestTimeout());
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
std::future<SdkResponseType>
Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::executeAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout)
{
  return std::async(std::launch::async, [this, &client, &timeout]() { return this->execute(client, timeout); });
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
void Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::executeAsync(
  const Client& client,
  const std::function<void(const SdkResponseType&, const std::exception&)>& callback)
{
  return executeAsync(client, client.getRequestTimeout(), callback);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
void Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::executeAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout,
  const std::function<void(const SdkResponseType&, const std::exception&)>& callback)
{
  std::future<SdkResponseType> future =
    std::async(std::launch::async, [this, &client, &timeout]() { return this->execute(client, timeout); });

  try
  {
    callback(future.get(), std::exception());
  }
  catch (const std::exception& exception)
  {
    callback(SdkResponseType(), exception);
  }
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
void Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::executeAsync(
  const Client& client,
  const std::function<void(const SdkResponseType&)>& responseCallback,
  const std::function<void(const std::exception&)>& exceptionCallback)
{
  return executeAsync(client, client.getRequestTimeout(), responseCallback, exceptionCallback);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
void Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::executeAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout,
  const std::function<void(const SdkResponseType&)>& responseCallback,
  const std::function<void(const std::exception&)>& exceptionCallback)
{
  std::future<SdkResponseType> future =
    std::async(std::launch::async, [this, &client, &timeout]() { return this->execute(client, timeout); });

  try
  {
    responseCallback(future.get());
  }
  catch (const std::exception& exception)
  {
    exceptionCallback(exception);
  }
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setNodeAccountIds(
  std::vector<AccountId> nodeAccountIds)
{
  mNodeAccountIds = std::move(nodeAccountIds);
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setLogger(
  const Logger& logger)
{
  mLogger = logger;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setRequestListener(
  const std::function<ProtoRequestType(ProtoRequestType&)>& listener)
{
  mRequestListener = listener;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setResponseListener(
  const std::function<ProtoResponseType(ProtoResponseType&)>& listener)
{
  mResponseListener = listener;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setMaxAttempts(
  uint32_t attempts)
{
  mMaxAttempts = attempts;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setMinBackoff(
  const std::chrono::system_clock::duration& backoff)
{
  if ((mMaxBackoff.has_value() && backoff > mMaxBackoff.value()) ||
      (!mMaxBackoff.has_value() && backoff > DEFAULT_MAX_BACKOFF))
  {
    throw std::invalid_argument("Minimum backoff would be larger than maximum backoff");
  }

  mMinBackoff = backoff;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setMaxBackoff(
  const std::chrono::system_clock::duration& backoff)
{
  if ((mMinBackoff.has_value() && backoff < mMinBackoff.value()) ||
      (!mMinBackoff.has_value() && backoff < DEFAULT_MIN_BACKOFF))
  {
    throw std::invalid_argument("Maximum backoff would be smaller than minimum backoff");
  }

  mMaxBackoff = backoff;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
SdkRequestType& Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setGrpcDeadline(
  const std::chrono::system_clock::duration& deadline)
{
  mGrpcDeadline = deadline;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
typename Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::ExecutionStatus
Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::determineStatus(
  Status status,
  const Client&,
  const ProtoResponseType&)
{
  switch (status)
  {
    case Status::PLATFORM_TRANSACTION_NOT_CREATED:
    case Status::PLATFORM_NOT_ACTIVE:
    case Status::BUSY:
      return ExecutionStatus::SERVER_ERROR;
    case Status::OK:
      return ExecutionStatus::SUCCESS;
      // Let derived class handle this status, assume request error
    default:
      return ExecutionStatus::REQUEST_ERROR;
  }
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
void Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::setExecutionParameters(
  const Client& client)
{
  mCurrentMaxAttempts = mMaxAttempts.has_value()              ? mMaxAttempts.value()
                        : client.getMaxAttempts().has_value() ? client.getMaxAttempts().value()
                                                              : DEFAULT_MAX_ATTEMPTS;
  mCurrentMinBackoff = mMinBackoff.has_value()              ? mMinBackoff.value()
                       : client.getMinBackoff().has_value() ? client.getMinBackoff().value()
                                                            : DEFAULT_MIN_BACKOFF;
  mCurrentMaxBackoff = mMaxBackoff.has_value()              ? mMaxBackoff.value()
                       : client.getMaxBackoff().has_value() ? client.getMaxBackoff().value()
                                                            : DEFAULT_MAX_BACKOFF;
  mCurrentBackoff = mCurrentMinBackoff;
  mCurrentGrpcDeadline = mGrpcDeadline.has_value()              ? mGrpcDeadline.value()
                         : client.getGrpcDeadline().has_value() ? client.getGrpcDeadline().value()
                                                                : DEFAULT_GRPC_DEADLINE;
  mMirrorNodeIds = client.getMirrorNetwork();
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
std::vector<std::shared_ptr<internal::Node>>
Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::getNodesFromNodeAccountIds(
  const Client& client) const
{
  std::vector<std::shared_ptr<internal::Node>> nodes;

  // If there are multiple nodes, this Executable should simply try a different node.
  for (const AccountId& accountId : mNodeAccountIds)
  {
    const std::vector<std::shared_ptr<internal::Node>> nodeProxies =
      client.getClientNetwork()->getNodeProxies(accountId);

    // Verify the node account ID mapped to a valid Node.
    if (nodeProxies.empty())
    {
      continue;
    }

    // If there is a single node account id add all it's corresponding proxies
    if (mNodeAccountIds.size() == 1)
    {
      nodes.insert(nodes.end(), nodeProxies.begin(), nodeProxies.end());
      break;
    }

    // Pick a random proxy from the proxy list to add to the node list.
    nodes.push_back(
      nodeProxies.at(internal::Utilities::getRandomNumber(0U, static_cast<unsigned int>(nodeProxies.size()) - 1U)));
  }

  if (nodes.empty())
  {
    throw IllegalStateException(
      "Attempting to fetch node proxies for which are not included in the Client's network. Please review your Client "
      "configuration or the set node account id.");
  }

  return nodes;
}

//-----
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
unsigned int Executable<SdkRequestType, ProtoRequestType, ProtoResponseType, SdkResponseType>::getNodeIndexForExecute(
  const std::vector<std::shared_ptr<internal::Node>>& nodes,
  unsigned int attempt) const
{
  // Keep track of the best candidate node and its delay (initialize to make compiler happy, but this should never be
  // returned without being provided an actual legitimate value).
  unsigned int candidateNodeIndex = -1U;
  std::chrono::system_clock::duration candidateDelay(std::numeric_limits<int64_t>::max());

  // Start looking at nodes at the attempt index, but wrap if there's been more attempts than nodes.
  for (unsigned int i = attempt % nodes.size(); i < nodes.size(); ++i)
  {
    const std::shared_ptr<internal::Node>& node = nodes.at(i);
    if (!node->isHealthy())
    {
      // Mark this the candidate node if it has the smaller delay than the previous candidate.
      const std::chrono::system_clock::duration backoffTime = node->getRemainingTimeForBackoff();
      if (backoffTime < candidateDelay)
      {
        candidateNodeIndex = i;
        candidateDelay = backoffTime;
      }
    }

    // If this node is healthy, then its usable.
    else
    {
      mLogger.trace("Using node " + node->getAccountId().toString() + " for request #" + std::to_string(attempt));
      return i;
    }
  }

  // No nodes are healthy, return the index of the one with the smallest delay.
  mLogger.trace("Using node " + nodes.at(candidateNodeIndex)->getAccountId().toString() + " for request #" +
                std::to_string(attempt));
  return candidateNodeIndex;
}

/**
 * Explicit template instantiations.
 */
template class Executable<AccountAllowanceApproveTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<AccountAllowanceDeleteTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<AccountBalanceQuery, proto::Query, proto::Response, AccountBalance>;
template class Executable<AccountCreateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<AccountDeleteTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<AccountInfoQuery, proto::Query, proto::Response, AccountInfo>;
template class Executable<AccountRecordsQuery, proto::Query, proto::Response, AccountRecords>;
template class Executable<AccountStakersQuery, proto::Query, proto::Response, AccountStakers>;
template class Executable<AccountUpdateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<ContractByteCodeQuery, proto::Query, proto::Response, ContractByteCode>;
template class Executable<ContractCallQuery, proto::Query, proto::Response, ContractFunctionResult>;
template class Executable<ContractCreateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<ContractDeleteTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<ContractExecuteTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<ContractInfoQuery, proto::Query, proto::Response, ContractInfo>;
template class Executable<ContractUpdateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<EthereumTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<FileAppendTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<FileContentsQuery, proto::Query, proto::Response, FileContents>;
template class Executable<FileCreateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<FileDeleteTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<FileInfoQuery, proto::Query, proto::Response, FileInfo>;
template class Executable<FileUpdateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<FreezeTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<NetworkVersionInfoQuery, proto::Query, proto::Response, NetworkVersionInfo>;
template class Executable<NodeCreateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<NodeDeleteTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<NodeUpdateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<PrngTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<ScheduleCreateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<ScheduleDeleteTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<ScheduleInfoQuery, proto::Query, proto::Response, ScheduleInfo>;
template class Executable<ScheduleSignTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<SystemDeleteTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<SystemUndeleteTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenAirdropTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenAssociateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenBurnTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenCancelAirdropTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenClaimAirdropTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenCreateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenDeleteTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenDissociateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenFeeScheduleUpdateTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenFreezeTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenGrantKycTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenInfoQuery, proto::Query, proto::Response, TokenInfo>;
template class Executable<TokenMintTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenNftInfoQuery, proto::Query, proto::Response, TokenNftInfo>;
template class Executable<TokenPauseTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenRejectTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenRevokeKycTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenUnfreezeTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenUnpauseTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenUpdateNftsTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TokenUpdateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TokenWipeTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TopicCreateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TopicDeleteTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TopicInfoQuery, proto::Query, proto::Response, TopicInfo>;
template class Executable<TopicMessageSubmitTransaction,
                          proto::Transaction,
                          proto::TransactionResponse,
                          TransactionResponse>;
template class Executable<TopicUpdateTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;
template class Executable<TransactionReceiptQuery, proto::Query, proto::Response, TransactionReceipt>;
template class Executable<TransactionRecordQuery, proto::Query, proto::Response, TransactionRecord>;
template class Executable<TransferTransaction, proto::Transaction, proto::TransactionResponse, TransactionResponse>;

} // namespace Hiero
