// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_MESSAGE_H_
#define HIERO_SDK_CPP_TOPIC_MESSAGE_H_

#include "TopicMessageChunk.h"
#include "TransactionId.h"

#include <chrono>
#include <cstddef>
#include <vector>

namespace com::hedera::mirror::api::proto
{
class ConsensusTopicResponse;
}

namespace Hiero
{
/**
 * The message of a topic.
 */
class TopicMessage
{
public:
  TopicMessage() = default;

  /**
   * Construct with the consensus timestamp of the last message received, the topic message, the last running hash, the
   * last sequence number, the chunks that make up the message, and the transaction ID.
   *
   * @param lastConsensusTimestamp The consensus timestamp of the last message received.
   * @param message                The topic message.
   * @param lastRunningHash        The last running hash of the topic.
   * @param lastSequenceNumber     The last sequence number of the topic.
   * @param chunks                 The chunks that make up the message.
   * @param transactionId          The ID of the transaction.
   */
  TopicMessage(const std::chrono::system_clock::time_point& lastConsensusTimestamp,
               const std::vector<std::byte>& message,
               const std::vector<std::byte>& lastRunningHash,
               uint64_t lastSequenceNumber,
               const std::vector<TopicMessageChunk>& chunks,
               TransactionId transactionId);

  /**
   * Create a TopicMessage from a single ConsensusTopicResponse protobuf object.
   *
   * @param proto The ConsensusTopicResponse protobuf object from which to create a TopicMessage.
   * @return The constructed TopicMessage object.
   */
  [[nodiscard]] static TopicMessage ofSingle(const com::hedera::mirror::api::proto::ConsensusTopicResponse& proto);

  /**
   * Create a TopicMessage from numerous ConsensusTopicResponse protobuf objects.
   *
   * @param protos The ConsensusTopicResponse protobuf objects from which to create a TopicMessage.
   * @return The constructed TopicMessage object.
   */
  [[nodiscard]] static TopicMessage ofMany(
    const std::vector<com::hedera::mirror::api::proto::ConsensusTopicResponse>& protos);

  /**
   * The consensus timestamp of the full TopicMessage.
   */
  std::chrono::system_clock::time_point mConsensusTimestamp;

  /**
   * The content of this TopicMessage.
   */
  std::vector<std::byte> mContents;

  /**
   * The running hash of the topic that received the message.
   */
  std::vector<std::byte> mRunningHash;

  /**
   * The sequence number of this TopicMessage.
   */
  uint64_t mSequenceNumber = 0ULL;

  /**
   * This TopicMessage's chunks, if it was divided into chunks.
   */
  std::vector<TopicMessageChunk> mChunks;

  /**
   * The ID of the corresponding transaction.
   */
  TransactionId mTransactionId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOPIC_MESSAGE_H_
