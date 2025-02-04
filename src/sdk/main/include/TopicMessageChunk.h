// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_MESSAGE_CHUNK_H_
#define HIERO_SDK_CPP_TOPIC_MESSAGE_CHUNK_H_

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
 * A chunk of a topic message.
 */
class TopicMessageChunk
{
public:
  TopicMessageChunk() = default;

  /**
   * Construct from a ConsensusTopicResponse protobuf object.
   *
   * @param proto The ConsensusTopicResponse protobuf object.
   */
  explicit TopicMessageChunk(const com::hedera::mirror::api::proto::ConsensusTopicResponse& proto);

  /**
   * The consensus timestamp of this topic message chunk.
   */
  std::chrono::system_clock::time_point mConsensusTimestamp;

  /**
   * The size of this chunk's content.
   */
  uint64_t mContentSize = 0ULL;

  /**
   * The running hash of this chunk.
   */
  std::vector<std::byte> mRunningHash;

  /**
   * The sequence number of this chunk.
   */
  uint64_t mSequenceNumber = 0ULL;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOPIC_MESSAGE_CHUNK_H_
