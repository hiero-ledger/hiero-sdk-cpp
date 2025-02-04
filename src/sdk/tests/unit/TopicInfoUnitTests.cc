// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "TopicInfo.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <consensus_get_topic_info.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class TopicInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TopicId& getTestTopicId() const { return mTestTopicId; }
  [[nodiscard]] inline const std::string& getTestTopicMemo() const { return mTestTopicMemo; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestRunningHash() const { return mTestRunningHash; }
  [[nodiscard]] inline uint64_t getTestSequenceNumber() const { return mTestSequenceNumber; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestSubmitKey() const { return mTestSubmitKey; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mTestAutoRenewPeriod;
  }
  [[nodiscard]] inline const AccountId& getTestAutoRenewAccountId() const { return mTestAutoRenewAccountId; }
  [[nodiscard]] inline const LedgerId& getTestLedgerId() const { return mTestLedgerId; }

private:
  const TopicId mTestTopicId = TopicId(1ULL, 2ULL, 3ULL);
  const std::string mTestTopicMemo = "test memo";
  const std::vector<std::byte> mTestRunningHash = { std::byte(0x04), std::byte(0x05), std::byte(0x06) };
  const uint64_t mTestSequenceNumber = 7ULL;
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const std::shared_ptr<PublicKey> mTestAdminKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestSubmitKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::chrono::system_clock::duration mTestAutoRenewPeriod = std::chrono::hours(8);
  const AccountId mTestAutoRenewAccountId = AccountId(9ULL, 10ULL, 11ULL);
  const LedgerId mTestLedgerId = LedgerId({ std::byte(0x0C), std::byte(0x0D) });
};

//-----
TEST_F(TopicInfoUnitTests, FromProtobuf)
{
  // Given
  proto::ConsensusGetTopicInfoResponse protoTopicInfo;
  protoTopicInfo.set_allocated_topicid(getTestTopicId().toProtobuf().release());
  protoTopicInfo.mutable_topicinfo()->set_memo(getTestTopicMemo());
  protoTopicInfo.mutable_topicinfo()->set_runninghash(internal::Utilities::byteVectorToString(getTestRunningHash()));
  protoTopicInfo.mutable_topicinfo()->set_sequencenumber(getTestSequenceNumber());
  protoTopicInfo.mutable_topicinfo()->set_allocated_expirationtime(
    internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  protoTopicInfo.mutable_topicinfo()->set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  protoTopicInfo.mutable_topicinfo()->set_allocated_submitkey(getTestSubmitKey()->toProtobufKey().release());
  protoTopicInfo.mutable_topicinfo()->set_allocated_autorenewperiod(
    internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  protoTopicInfo.mutable_topicinfo()->set_allocated_autorenewaccount(
    getTestAutoRenewAccountId().toProtobuf().release());
  protoTopicInfo.mutable_topicinfo()->set_ledger_id(
    internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));

  // When
  const TopicInfo topicInfo = TopicInfo::fromProtobuf(protoTopicInfo);

  // Then
  EXPECT_EQ(topicInfo.mTopicId, getTestTopicId());
  EXPECT_EQ(topicInfo.mMemo, getTestTopicMemo());
  EXPECT_EQ(topicInfo.mRunningHash, getTestRunningHash());
  EXPECT_EQ(topicInfo.mSequenceNumber, getTestSequenceNumber());
  EXPECT_EQ(topicInfo.mExpirationTime, getTestExpirationTime());
  EXPECT_EQ(topicInfo.mAdminKey->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(topicInfo.mSubmitKey->toBytes(), getTestSubmitKey()->toBytes());
  EXPECT_EQ(topicInfo.mAutoRenewPeriod, getTestAutoRenewPeriod());
  EXPECT_EQ(topicInfo.mAutoRenewAccountId, getTestAutoRenewAccountId());
  EXPECT_EQ(topicInfo.mLedgerId.toBytes(), getTestLedgerId().toBytes());
}

//-----
TEST_F(TopicInfoUnitTests, FromBytes)
{
  // Given
  proto::ConsensusGetTopicInfoResponse protoTopicInfo;
  protoTopicInfo.set_allocated_topicid(getTestTopicId().toProtobuf().release());
  protoTopicInfo.mutable_topicinfo()->set_memo(getTestTopicMemo());
  protoTopicInfo.mutable_topicinfo()->set_runninghash(internal::Utilities::byteVectorToString(getTestRunningHash()));
  protoTopicInfo.mutable_topicinfo()->set_sequencenumber(getTestSequenceNumber());
  protoTopicInfo.mutable_topicinfo()->set_allocated_expirationtime(
    internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  protoTopicInfo.mutable_topicinfo()->set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  protoTopicInfo.mutable_topicinfo()->set_allocated_submitkey(getTestSubmitKey()->toProtobufKey().release());
  protoTopicInfo.mutable_topicinfo()->set_allocated_autorenewperiod(
    internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  protoTopicInfo.mutable_topicinfo()->set_allocated_autorenewaccount(
    getTestAutoRenewAccountId().toProtobuf().release());
  protoTopicInfo.mutable_topicinfo()->set_ledger_id(
    internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));

  // When
  const TopicInfo topicInfo =
    TopicInfo::fromBytes(internal::Utilities::stringToByteVector(protoTopicInfo.SerializeAsString()));

  // Then
  EXPECT_EQ(topicInfo.mTopicId, getTestTopicId());
  EXPECT_EQ(topicInfo.mMemo, getTestTopicMemo());
  EXPECT_EQ(topicInfo.mRunningHash, getTestRunningHash());
  EXPECT_EQ(topicInfo.mSequenceNumber, getTestSequenceNumber());
  EXPECT_EQ(topicInfo.mExpirationTime, getTestExpirationTime());
  EXPECT_EQ(topicInfo.mAdminKey->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(topicInfo.mSubmitKey->toBytes(), getTestSubmitKey()->toBytes());
  EXPECT_EQ(topicInfo.mAutoRenewPeriod, getTestAutoRenewPeriod());
  EXPECT_EQ(topicInfo.mAutoRenewAccountId, getTestAutoRenewAccountId());
  EXPECT_EQ(topicInfo.mLedgerId.toBytes(), getTestLedgerId().toBytes());
}

//-----
TEST_F(TopicInfoUnitTests, ToProtobuf)
{
  // Given
  TopicInfo topicInfo;
  topicInfo.mTopicId = getTestTopicId();
  topicInfo.mTopicId = getTestTopicId();
  topicInfo.mMemo = getTestTopicMemo();
  topicInfo.mRunningHash = getTestRunningHash();
  topicInfo.mSequenceNumber = getTestSequenceNumber();
  topicInfo.mExpirationTime = getTestExpirationTime();
  topicInfo.mAdminKey = getTestAdminKey();
  topicInfo.mSubmitKey = getTestSubmitKey();
  topicInfo.mAutoRenewPeriod = getTestAutoRenewPeriod();
  topicInfo.mAutoRenewAccountId = getTestAutoRenewAccountId();
  topicInfo.mLedgerId = getTestLedgerId();

  // When
  const std::unique_ptr<proto::ConsensusGetTopicInfoResponse> protoTopicInfo = topicInfo.toProtobuf();

  // Then
  EXPECT_EQ(protoTopicInfo->topicid().shardnum(), getTestTopicId().mShardNum);
  EXPECT_EQ(protoTopicInfo->topicid().realmnum(), getTestTopicId().mRealmNum);
  EXPECT_EQ(protoTopicInfo->topicid().topicnum(), getTestTopicId().mTopicNum);
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->memo(), getTestTopicMemo());
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->runninghash(),
            internal::Utilities::byteVectorToString(getTestRunningHash()));
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->sequencenumber(), getTestSequenceNumber());
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->expirationtime().seconds(),
            internal::TimestampConverter::toProtobuf(getTestExpirationTime())->seconds());
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->adminkey().ecdsa_secp256k1(),
            internal::Utilities::byteVectorToString(getTestAdminKey()->toBytesRaw()));
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->submitkey().ecdsa_secp256k1(),
            internal::Utilities::byteVectorToString(getTestSubmitKey()->toBytesRaw()));
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->autorenewperiod().seconds(),
            internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod())->seconds());
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->autorenewaccount().shardnum(), getTestAutoRenewAccountId().mShardNum);
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->autorenewaccount().realmnum(), getTestAutoRenewAccountId().mRealmNum);
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->autorenewaccount().accountnum(),
            getTestAutoRenewAccountId().mAccountNum);
  EXPECT_EQ(protoTopicInfo->mutable_topicinfo()->ledger_id(),
            internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));
}

//-----
TEST_F(TopicInfoUnitTests, ToBytes)
{
  // Given
  TopicInfo topicInfo;
  topicInfo.mTopicId = getTestTopicId();
  topicInfo.mTopicId = getTestTopicId();
  topicInfo.mMemo = getTestTopicMemo();
  topicInfo.mRunningHash = getTestRunningHash();
  topicInfo.mSequenceNumber = getTestSequenceNumber();
  topicInfo.mExpirationTime = getTestExpirationTime();
  topicInfo.mAdminKey = getTestAdminKey();
  topicInfo.mSubmitKey = getTestSubmitKey();
  topicInfo.mAutoRenewPeriod = getTestAutoRenewPeriod();
  topicInfo.mAutoRenewAccountId = getTestAutoRenewAccountId();
  topicInfo.mLedgerId = getTestLedgerId();

  // When
  const std::vector<std::byte> bytes = topicInfo.toBytes();

  // Then
  EXPECT_EQ(bytes, internal::Utilities::stringToByteVector(topicInfo.toProtobuf()->SerializeAsString()));
}
