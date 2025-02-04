// SPDX-License-Identifier: Apache-2.0
#include "TopicMessageChunk.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <mirror/consensus_service.pb.h>

using namespace Hiero;

class TopicMessageChunkUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestConsensusTimestamp() const
  {
    return mTestConsensusTimestamp;
  }
  [[nodiscard]] inline const std::vector<std::byte>& getTestContents() const { return mTestContents; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestRunningHash() const { return mTestRunningHash; }
  [[nodiscard]] inline uint64_t getTestSequenceNumber() const { return mTestSequenceNumber; }

private:
  const std::chrono::system_clock::time_point mTestConsensusTimestamp = std::chrono::system_clock::now();
  const std::vector<std::byte> mTestContents = { std::byte(0x01), std::byte(0x02), std::byte(0x03) };
  const std::vector<std::byte> mTestRunningHash = { std::byte(0x04), std::byte(0x05), std::byte(0x06) };
  const uint64_t mTestSequenceNumber = 7ULL;
};

//-----
TEST_F(TopicMessageChunkUnitTests, ConstructWithArgs)
{
  // Given
  com::hedera::mirror::api::proto::ConsensusTopicResponse consensusTopicResponse;
  consensusTopicResponse.set_allocated_consensustimestamp(
    internal::TimestampConverter::toProtobuf(getTestConsensusTimestamp()));
  consensusTopicResponse.set_message(internal::Utilities::byteVectorToString(getTestContents()));
  consensusTopicResponse.set_runninghash(internal::Utilities::byteVectorToString(getTestRunningHash()));
  consensusTopicResponse.set_sequencenumber(getTestSequenceNumber());

  // When
  const TopicMessageChunk topicMessageChunk(consensusTopicResponse);

  // Then
  EXPECT_EQ(topicMessageChunk.mConsensusTimestamp, getTestConsensusTimestamp());
  EXPECT_EQ(topicMessageChunk.mContentSize, getTestContents().size());
  EXPECT_EQ(topicMessageChunk.mRunningHash, getTestRunningHash());
  EXPECT_EQ(topicMessageChunk.mSequenceNumber, getTestSequenceNumber());
}
