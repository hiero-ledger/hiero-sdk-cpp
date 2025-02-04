// SPDX-License-Identifier: Apache-2.0
#include "TopicMessage.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <mirror/consensus_service.pb.h>

using namespace Hiero;

class TopicMessageUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestConsensusTimestamp() const
  {
    return mTestConsensusTimestamp;
  }
  [[nodiscard]] inline const std::vector<std::byte>& getTestContents() const { return mTestContents; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestRunningHash() const { return mTestRunningHash; }
  [[nodiscard]] inline uint64_t getTestSequenceNumber() const { return mTestSequenceNumber; }
  [[nodiscard]] inline const TransactionId& getTestTransactionId() const { return mTestTransactionId; }

private:
  const std::chrono::system_clock::time_point mTestConsensusTimestamp = std::chrono::system_clock::now();
  const std::vector<std::byte> mTestContents = { std::byte(0x01), std::byte(0x02), std::byte(0x03) };
  const std::vector<std::byte> mTestRunningHash = { std::byte(0x04), std::byte(0x05), std::byte(0x06) };
  const uint64_t mTestSequenceNumber = 7ULL;
  const TransactionId mTestTransactionId = TransactionId::generate(AccountId(8ULL, 9ULL, 10ULL));
};

//-----
TEST_F(TopicMessageUnitTests, ConstructWithArgs)
{
  // Given
  TopicMessageChunk chunk;
  chunk.mConsensusTimestamp = getTestConsensusTimestamp();
  chunk.mContentSize = 1ULL;
  chunk.mRunningHash = getTestRunningHash();
  chunk.mSequenceNumber = getTestSequenceNumber();

  const std::vector<TopicMessageChunk> chunks = { chunk, chunk, chunk };

  // When
  const TopicMessage topicMessage(getTestConsensusTimestamp(),
                                  getTestContents(),
                                  getTestRunningHash(),
                                  getTestSequenceNumber(),
                                  chunks,
                                  getTestTransactionId());

  // Then
  EXPECT_EQ(topicMessage.mConsensusTimestamp, getTestConsensusTimestamp());
  EXPECT_EQ(topicMessage.mContents, getTestContents());
  EXPECT_EQ(topicMessage.mRunningHash, getTestRunningHash());
  EXPECT_EQ(topicMessage.mSequenceNumber, getTestSequenceNumber());
  EXPECT_EQ(topicMessage.mChunks.size(), chunks.size());
  EXPECT_EQ(topicMessage.mTransactionId, getTestTransactionId());
}

//-----
TEST_F(TopicMessageUnitTests, OfSingle)
{
  // Given
  com::hedera::mirror::api::proto::ConsensusTopicResponse consensusTopicResponse;
  consensusTopicResponse.set_allocated_consensustimestamp(
    internal::TimestampConverter::toProtobuf(getTestConsensusTimestamp()));
  consensusTopicResponse.set_message(internal::Utilities::byteVectorToString(getTestContents()));
  consensusTopicResponse.set_runninghash(internal::Utilities::byteVectorToString(getTestRunningHash()));
  consensusTopicResponse.set_sequencenumber(getTestSequenceNumber());
  consensusTopicResponse.mutable_chunkinfo()->set_allocated_initialtransactionid(
    getTestTransactionId().toProtobuf().release());

  // When
  const TopicMessage topicMessage = TopicMessage::ofSingle(consensusTopicResponse);

  // Then
  EXPECT_EQ(topicMessage.mConsensusTimestamp, getTestConsensusTimestamp());
  EXPECT_EQ(topicMessage.mContents, getTestContents());
  EXPECT_EQ(topicMessage.mRunningHash, getTestRunningHash());
  EXPECT_EQ(topicMessage.mSequenceNumber, getTestSequenceNumber());
  EXPECT_EQ(topicMessage.mChunks.size(), 1);
  EXPECT_EQ(topicMessage.mTransactionId, getTestTransactionId());
}

//-----
TEST_F(TopicMessageUnitTests, OfMany)
{
  // Given
  com::hedera::mirror::api::proto::ConsensusTopicResponse consensusTopicResponse1;
  com::hedera::mirror::api::proto::ConsensusTopicResponse consensusTopicResponse2;

  consensusTopicResponse1.set_allocated_consensustimestamp(
    internal::TimestampConverter::toProtobuf(getTestConsensusTimestamp()));
  consensusTopicResponse1.set_message(internal::Utilities::byteVectorToString(getTestContents()));
  consensusTopicResponse1.set_runninghash(internal::Utilities::byteVectorToString(getTestRunningHash()));
  consensusTopicResponse1.set_sequencenumber(getTestSequenceNumber());
  consensusTopicResponse1.mutable_chunkinfo()->set_allocated_initialtransactionid(
    getTestTransactionId().toProtobuf().release());
  consensusTopicResponse1.mutable_chunkinfo()->set_number(1);
  consensusTopicResponse1.mutable_chunkinfo()->set_total(2);

  consensusTopicResponse2.set_allocated_consensustimestamp(
    internal::TimestampConverter::toProtobuf(getTestConsensusTimestamp() + std::chrono::seconds(1)));
  consensusTopicResponse2.set_message(internal::Utilities::byteVectorToString(getTestContents()));
  consensusTopicResponse2.set_runninghash(internal::Utilities::byteVectorToString(getTestRunningHash()));
  consensusTopicResponse2.set_sequencenumber(getTestSequenceNumber() + 1ULL);
  consensusTopicResponse2.mutable_chunkinfo()->set_number(2);
  consensusTopicResponse2.mutable_chunkinfo()->set_total(2);

  // When
  const TopicMessage topicMessage = TopicMessage::ofMany({ consensusTopicResponse1, consensusTopicResponse2 });

  // Then
  const std::vector<std::byte> totalContents =
    internal::Utilities::concatenateVectors({ getTestContents(), getTestContents() });
  EXPECT_EQ(topicMessage.mConsensusTimestamp, getTestConsensusTimestamp() + std::chrono::seconds(1));
  EXPECT_EQ(topicMessage.mContents, totalContents);
  EXPECT_EQ(topicMessage.mRunningHash, getTestRunningHash());
  EXPECT_EQ(topicMessage.mSequenceNumber, getTestSequenceNumber() + 1ULL);
  EXPECT_EQ(topicMessage.mChunks.size(), 2);
  EXPECT_EQ(topicMessage.mTransactionId, getTestTransactionId());
}
