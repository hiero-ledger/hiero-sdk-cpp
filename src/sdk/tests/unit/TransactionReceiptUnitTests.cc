// SPDX-License-Identifier: Apache-2.0
#include "Status.h"
#include "TransactionReceipt.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_receipt.pb.h>

using namespace Hiero;

class TransactionReceiptUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const TopicId& getTestTopicId() const { return mTestTopicId; }
  [[nodiscard]] inline uint64_t getTestTopicSequenceNumber() const { return mTestTopicSequenceNumber; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestTopicRunningHash() const { return mTestTopicRunningHash; }
  [[nodiscard]] inline uint64_t getTestTopicRunningHashVersion() const { return mTestTopicRunningHashVersion; }
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }
  [[nodiscard]] inline const uint64_t& getTestNewTotalSupply() const { return mTestNewTotalSupply; }
  [[nodiscard]] inline const ScheduleId& getTestScheduleId() const { return mTestScheduleId; }
  [[nodiscard]] inline const TransactionId& getTestScheduledTransactionId() const
  {
    return mTestScheduledTransactionId;
  }
  [[nodiscard]] inline const std::vector<uint64_t>& getTestSerialNumbers() const { return mTestSerialNumbers; }

private:
  const AccountId mTestAccountId = AccountId(1ULL);
  const FileId mTestFileId = FileId(2ULL);
  const ContractId mTestContractId = ContractId(3ULL);
  const TopicId mTestTopicId = TopicId(4ULL);
  const uint64_t mTestTopicSequenceNumber = 5ULL;
  const std::vector<std::byte> mTestTopicRunningHash = { std::byte(0x06), std::byte(0x07), std::byte(0x08) };
  const uint64_t mTestTopicRunningHashVersion = 9ULL;
  const TokenId mTestTokenId = TokenId(10ULL);
  const uint64_t mTestNewTotalSupply = 11ULL;
  const ScheduleId mTestScheduleId = ScheduleId(12ULL);
  const TransactionId mTestScheduledTransactionId = TransactionId::generate(AccountId(13ULL));
  const std::vector<uint64_t> mTestSerialNumbers = { 14ULL, 15ULL, 16ULL };
};

//-----
TEST_F(TransactionReceiptUnitTests, ProtobufTransactionReceipt)
{
  // Given
  proto::TransactionReceipt protoTxReceipt;
  protoTxReceipt.set_status(proto::ResponseCodeEnum::SUCCESS);
  protoTxReceipt.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  protoTxReceipt.set_allocated_fileid(getTestFileId().toProtobuf().release());
  protoTxReceipt.set_allocated_contractid(getTestContractId().toProtobuf().release());

  const int32_t value = 6;
  const int32_t secs = 100;

  protoTxReceipt.mutable_exchangerate()->mutable_currentrate()->set_hbarequiv(value);
  protoTxReceipt.mutable_exchangerate()->mutable_currentrate()->set_centequiv(value);
  protoTxReceipt.mutable_exchangerate()->mutable_currentrate()->mutable_expirationtime()->set_seconds(secs);
  protoTxReceipt.mutable_exchangerate()->mutable_nextrate()->set_hbarequiv(value);
  protoTxReceipt.mutable_exchangerate()->mutable_nextrate()->set_centequiv(value);
  protoTxReceipt.mutable_exchangerate()->mutable_nextrate()->mutable_expirationtime()->set_seconds(secs);

  protoTxReceipt.set_allocated_topicid(getTestTopicId().toProtobuf().release());
  protoTxReceipt.set_topicsequencenumber(getTestTopicSequenceNumber());
  protoTxReceipt.set_topicrunninghash(internal::Utilities::byteVectorToString(getTestTopicRunningHash()));
  protoTxReceipt.set_topicrunninghashversion(getTestTopicRunningHashVersion());
  protoTxReceipt.set_allocated_tokenid(getTestTokenId().toProtobuf().release());
  protoTxReceipt.set_newtotalsupply(getTestNewTotalSupply());
  protoTxReceipt.set_allocated_scheduleid(getTestScheduleId().toProtobuf().release());
  protoTxReceipt.set_allocated_scheduledtransactionid(getTestScheduledTransactionId().toProtobuf().release());

  for (const auto& serialNum : getTestSerialNumbers())
  {
    protoTxReceipt.add_serialnumbers(static_cast<int64_t>(serialNum));
  }

  // When
  const TransactionReceipt txRx = TransactionReceipt::fromProtobuf(protoTxReceipt);

  // Then
  EXPECT_EQ(txRx.mStatus, Status::SUCCESS);
  ASSERT_TRUE(txRx.mAccountId.has_value());
  EXPECT_EQ(txRx.mAccountId, getTestAccountId());
  ASSERT_TRUE(txRx.mFileId.has_value());
  EXPECT_EQ(txRx.mFileId, getTestFileId());
  ASSERT_TRUE(txRx.mContractId.has_value());
  EXPECT_EQ(txRx.mContractId, getTestContractId());
  EXPECT_EQ(txRx.mExchangeRates.mCurrentRate.mExchangeRateInCents, value / value);
  EXPECT_EQ(txRx.mExchangeRates.mCurrentRate.mExpirationTime,
            std::chrono::system_clock::time_point(std::chrono::seconds(secs)));
  EXPECT_EQ(txRx.mExchangeRates.mNextRate.mExchangeRateInCents, value / value);
  EXPECT_EQ(txRx.mExchangeRates.mNextRate.mExpirationTime,
            std::chrono::system_clock::time_point(std::chrono::seconds(secs)));
  ASSERT_TRUE(txRx.mTopicId.has_value());
  EXPECT_EQ(txRx.mTopicId, getTestTopicId());
  ASSERT_TRUE(txRx.mTopicSequenceNumber.has_value());
  EXPECT_EQ(txRx.mTopicSequenceNumber, getTestTopicSequenceNumber());
  ASSERT_TRUE(txRx.mTopicRunningHash.has_value());
  EXPECT_EQ(txRx.mTopicRunningHash, getTestTopicRunningHash());
  ASSERT_TRUE(txRx.mTopicRunningHashVersion.has_value());
  EXPECT_EQ(txRx.mTopicRunningHashVersion, getTestTopicRunningHashVersion());
  ASSERT_TRUE(txRx.mTokenId.has_value());
  EXPECT_EQ(txRx.mTokenId, getTestTokenId());
  ASSERT_TRUE(txRx.mNewTotalSupply.has_value());
  EXPECT_EQ(txRx.mNewTotalSupply, getTestNewTotalSupply());
  ASSERT_TRUE(txRx.mScheduleId.has_value());
  EXPECT_EQ(txRx.mScheduleId, getTestScheduleId());
  ASSERT_TRUE(txRx.mScheduledTransactionId.has_value());
  EXPECT_EQ(txRx.mScheduledTransactionId, getTestScheduledTransactionId());
  EXPECT_EQ(txRx.mSerialNumbers.size(), getTestSerialNumbers().size());
}
