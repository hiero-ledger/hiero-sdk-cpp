// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_MESSAGE_QUERY_H_
#define HIERO_SDK_CPP_TOPIC_MESSAGE_QUERY_H_

#include <chrono>
#include <functional>
#include <memory>

namespace grpc
{
class Status;
}

namespace Hiero
{
class Client;
class SubscriptionHandle;
class TopicId;
class TopicMessage;
}

namespace Hiero
{
/**
 * Subscribe to a topic ID's messages from a mirror node. You will receive all messages for the specified topic or
 * within the defined start and end time.
 */
class TopicMessageQuery
{
public:
  TopicMessageQuery();
  ~TopicMessageQuery();

  /**
   * Copy constructor and assignment operator only copy fields, they do not copy subscription processing.
   */
  TopicMessageQuery(const TopicMessageQuery&);
  TopicMessageQuery& operator=(const TopicMessageQuery&);
  TopicMessageQuery(TopicMessageQuery&&) noexcept;
  TopicMessageQuery& operator=(TopicMessageQuery&&) noexcept;

  /**
   * Subscribe to messages sent on this topic ID set in this TopicMessageQuery.
   *
   * @param client The Client to use which contains the correct network to subscribe.
   * @param onNext The function to call when a message is received.
   * @return The SubscriptionHandle for this TopicMessageQuery.
   */
  std::shared_ptr<SubscriptionHandle> subscribe(const Client& client,
                                                const std::function<void(const TopicMessage&)>& onNext);

  /**
   * Set the ID of the topic from which to get messages.
   *
   * @param topicId The ID of the topic from which to get messages.
   * @return A reference to this TopicMessageQuery object with the newly-set topic ID.
   */
  TopicMessageQuery& setTopicId(const TopicId& topicId);

  /**
   * Set the start time which defines the earliest point from which messages are received.
   *
   * @param start The time which defines the earliest point from which messages are received.
   * @return A reference to this TopicMessageQuery object with the newly-set start time.
   */
  TopicMessageQuery& setStartTime(const std::chrono::system_clock::time_point& start);

  /**
   * Set the end time which defines the latest point from which messages are received.
   *
   * @param end The time which defines the latest point from which messages are received.
   * @return A reference to this TopicMessageQuery object with the newly-set end time.
   */
  TopicMessageQuery& setEndTime(const std::chrono::system_clock::time_point& end);

  /**
   * Set the maximum number of messages to return.
   *
   * @param limit The maximum number of messages to return.
   * @return A reference to this TopicMessageQuery object with the newly-set message limit.
   */
  TopicMessageQuery& setLimit(uint64_t limit);

  /**
   * Set the maximum amount of attempts to try and retrieve messages.
   *
   * @param attempts The maximum amount of attempts to try and retrieve messages.
   * @return A reference to this TopicMessageQuery object with the newly-set maximum attempt amount.
   */
  TopicMessageQuery& setMaxAttempts(uint32_t attempts);

  /**
   * Set the maximum amount of time to wait between submission attempts.
   *
   * @param backoff The maximum amount of time to wait between submission attempts.
   * @return A reference to this TopicMessageQuery object with the newly-set maximum backoff.
   */
  TopicMessageQuery& setMaxBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Set the function to run if there's an error with gRPC communication.
   *
   * @param func The function to run in case of errors.
   * @return A reference to this TopicMessageQuery object with the newly-set error handler.
   */
  TopicMessageQuery& setErrorHandler(const std::function<void(grpc::Status)>& func);

  /**
   * Set the function to run if after an error, a retry should occur. \c TRUE is used to indicate a retry should occur.
   *
   * @param func The function to run to determine if a retry should occur.
   * @return A reference to this TopicMessageQuery object with the newly-set retry handler.
   */
  TopicMessageQuery& setRetryHandler(const std::function<bool(grpc::Status)>& func);

  /**
   * Set the function to run upon completion of the RPC.
   *
   * @param func The function to run upon completion of the RPC.
   * @return A reference to this TopicMessageQuery object with the newly-set completion handler.
   */
  TopicMessageQuery& setCompletionHandler(const std::function<void(void)>& func);

  /**
   * Get the ID of the topic from which to get messages.
   *
   * @return The ID of the topic from which to get messages.
   */
  [[nodiscard]] TopicId getTopicId() const;

  /**
   * Get the start time which defines the earliest point from which messages are received.
   *
   * @return The start time which defines the earliest point from which messages are received.
   */
  [[nodiscard]] std::chrono::system_clock::time_point getStartTime() const;

  /**
   * Get the end time which defines the latest point from which messages are received.
   *
   * @return The end time which defines the latest point from which messages are received.
   */
  [[nodiscard]] std::chrono::system_clock::time_point getEndTime() const;

  /**
   * Get the maximum number of messages to return.
   *
   * @return The maximum number of messages to return.
   */
  [[nodiscard]] uint64_t getLimit() const;

  /**
   * Get the maximum amount of attempts to try and retrieve messages.
   *
   * @return The maximum amount of attempts to try and retrieve messages.
   */
  [[nodiscard]] uint32_t getMaxAttempts() const;

  /**
   * Get the maximum amount of time to wait between submission attempts.
   *
   * @return The maximum amount of time to wait between submission attempts.
   */
  [[nodiscard]] std::chrono::system_clock::duration getMaxBackoff() const;

private:
  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct TopicMessageQueryImpl;
  std::unique_ptr<TopicMessageQueryImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOPIC_MESSAGE_QUERY_H_
