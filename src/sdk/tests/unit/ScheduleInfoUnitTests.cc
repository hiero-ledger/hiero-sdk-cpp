// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "ScheduleInfo.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <schedule_get_info.pb.h>

using namespace Hiero;

class ScheduleInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ScheduleId& getTestScheduleId() const { return mTestScheduleId; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExecutionTime() const
  {
    return mTestExecutionTime;
  }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const WrappedTransaction& getTestSchedulableTransaction() const
  {
    return mTestSchedulableTransaction;
  }
  [[nodiscard]] inline const std::string& getTestMemo() const { return mTestMemo; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const KeyList& getTestSigners() const { return mTestSigners; }
  [[nodiscard]] inline const AccountId& getTestCreatorAccountId() const { return mTestCreatorAccountId; }
  [[nodiscard]] inline const AccountId& getTestPayerAccountId() const { return mTestPayerAccountId; }
  [[nodiscard]] inline const TransactionId& getTestScheduledTransactionId() const
  {
    return mTestScheduledTransactionId;
  }
  [[nodiscard]] inline const LedgerId& getTestLedgerId() const { return mTestLedgerId; }
  [[nodiscard]] inline bool getTestWaitForExpiry() const { return mTestWaitForExpiry; }

private:
  const ScheduleId mTestScheduleId = ScheduleId(1ULL, 2ULL, 3ULL);
  const std::chrono::system_clock::time_point mTestExecutionTime = std::chrono::system_clock::now();
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const WrappedTransaction mTestSchedulableTransaction = WrappedTransaction(AccountCreateTransaction());
  const std::string mTestMemo = "test memo";
  const std::shared_ptr<PublicKey> mTestAdminKey = ED25519PrivateKey::generatePrivateKey()->getPublicKey();
  const KeyList mTestSigners = KeyList::of(
    { ED25519PrivateKey::fromString(
        "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e10"),
      ED25519PrivateKey::fromString(
        "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e11"),
      ED25519PrivateKey::fromString(
        "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e12") });
  const AccountId mTestCreatorAccountId = AccountId(4ULL, 5ULL, 6ULL);
  const AccountId mTestPayerAccountId = AccountId(7ULL, 8ULL, 9ULL);
  const TransactionId mTestScheduledTransactionId = TransactionId::generate(AccountId(10ULL, 11ULL, 12ULL));
  const LedgerId mTestLedgerId = LedgerId({ std::byte(0x0D), std::byte(0x0E), std::byte(0x0F) });
  const bool mTestWaitForExpiry = true;
};

//-----
TEST_F(ScheduleInfoUnitTests, FromProtobuf)
{
  // Given
  proto::ScheduleInfo protoScheduleInfo;
  protoScheduleInfo.set_allocated_scheduleid(getTestScheduleId().toProtobuf().release());
  protoScheduleInfo.set_allocated_execution_time(internal::TimestampConverter::toProtobuf(getTestExecutionTime()));
  protoScheduleInfo.set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  protoScheduleInfo.set_allocated_scheduledtransactionbody(
    getTestSchedulableTransaction().toSchedulableProtobuf().release());
  protoScheduleInfo.set_memo(getTestMemo());
  protoScheduleInfo.set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  protoScheduleInfo.set_allocated_signers(getTestSigners().toProtobuf().release());
  protoScheduleInfo.set_allocated_creatoraccountid(getTestCreatorAccountId().toProtobuf().release());
  protoScheduleInfo.set_allocated_payeraccountid(getTestPayerAccountId().toProtobuf().release());
  protoScheduleInfo.set_allocated_scheduledtransactionid(getTestScheduledTransactionId().toProtobuf().release());
  protoScheduleInfo.set_ledger_id(internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));
  protoScheduleInfo.set_wait_for_expiry(getTestWaitForExpiry());

  // When
  const ScheduleInfo scheduleInfo = ScheduleInfo::fromProtobuf(protoScheduleInfo);

  // Then
  EXPECT_EQ(scheduleInfo.mScheduleId, getTestScheduleId());
  ASSERT_TRUE(scheduleInfo.mExecutionTime.has_value());
  EXPECT_EQ(scheduleInfo.mExecutionTime.value(), getTestExecutionTime());
  EXPECT_EQ(scheduleInfo.mExpirationTime, getTestExpirationTime());
  EXPECT_NE(scheduleInfo.mScheduledTransaction.getTransaction<AccountCreateTransaction>(), nullptr);
  EXPECT_EQ(scheduleInfo.mMemo, getTestMemo());
  EXPECT_EQ(scheduleInfo.mAdminKey->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(scheduleInfo.mSignatories.toBytes(), getTestSigners().toBytes());
  EXPECT_EQ(scheduleInfo.mCreatorAccountId, getTestCreatorAccountId());
  EXPECT_EQ(scheduleInfo.mPayerAccountId, getTestPayerAccountId());
  EXPECT_EQ(scheduleInfo.mScheduledTransactionId, getTestScheduledTransactionId());
  EXPECT_EQ(scheduleInfo.mLedgerId.toBytes(), getTestLedgerId().toBytes());
  EXPECT_EQ(scheduleInfo.mWaitForExpiry, getTestWaitForExpiry());
}

//-----
TEST_F(ScheduleInfoUnitTests, FromBytes)
{
  // Given
  proto::ScheduleInfo protoScheduleInfo;
  protoScheduleInfo.set_allocated_scheduleid(getTestScheduleId().toProtobuf().release());
  protoScheduleInfo.set_allocated_execution_time(internal::TimestampConverter::toProtobuf(getTestExecutionTime()));
  protoScheduleInfo.set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  protoScheduleInfo.set_allocated_scheduledtransactionbody(
    getTestSchedulableTransaction().toSchedulableProtobuf().release());
  protoScheduleInfo.set_memo(getTestMemo());
  protoScheduleInfo.set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  protoScheduleInfo.set_allocated_signers(getTestSigners().toProtobuf().release());
  protoScheduleInfo.set_allocated_creatoraccountid(getTestCreatorAccountId().toProtobuf().release());
  protoScheduleInfo.set_allocated_payeraccountid(getTestPayerAccountId().toProtobuf().release());
  protoScheduleInfo.set_allocated_scheduledtransactionid(getTestScheduledTransactionId().toProtobuf().release());
  protoScheduleInfo.set_ledger_id(internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));
  protoScheduleInfo.set_wait_for_expiry(getTestWaitForExpiry());

  // When
  const ScheduleInfo scheduleInfo =
    ScheduleInfo::fromBytes(internal::Utilities::stringToByteVector(protoScheduleInfo.SerializeAsString()));

  // Then
  EXPECT_EQ(scheduleInfo.mScheduleId, getTestScheduleId());
  ASSERT_TRUE(scheduleInfo.mExecutionTime.has_value());
  EXPECT_EQ(scheduleInfo.mExecutionTime.value(), getTestExecutionTime());
  EXPECT_EQ(scheduleInfo.mExpirationTime, getTestExpirationTime());
  EXPECT_NE(scheduleInfo.mScheduledTransaction.getTransaction<AccountCreateTransaction>(), nullptr);
  EXPECT_EQ(scheduleInfo.mMemo, getTestMemo());
  EXPECT_EQ(scheduleInfo.mAdminKey->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(scheduleInfo.mSignatories.toBytes(), getTestSigners().toBytes());
  EXPECT_EQ(scheduleInfo.mCreatorAccountId, getTestCreatorAccountId());
  EXPECT_EQ(scheduleInfo.mPayerAccountId, getTestPayerAccountId());
  EXPECT_EQ(scheduleInfo.mScheduledTransactionId, getTestScheduledTransactionId());
  EXPECT_EQ(scheduleInfo.mLedgerId.toBytes(), getTestLedgerId().toBytes());
  EXPECT_EQ(scheduleInfo.mWaitForExpiry, getTestWaitForExpiry());
}

//-----
TEST_F(ScheduleInfoUnitTests, ToProtobuf)
{
  // Given
  ScheduleInfo scheduleInfo;
  scheduleInfo.mScheduleId = getTestScheduleId();
  scheduleInfo.mExecutionTime = getTestExecutionTime();
  scheduleInfo.mExpirationTime = getTestExpirationTime();
  scheduleInfo.mScheduledTransaction = getTestSchedulableTransaction();
  scheduleInfo.mMemo = getTestMemo();
  scheduleInfo.mAdminKey = getTestAdminKey();
  scheduleInfo.mSignatories = getTestSigners();
  scheduleInfo.mCreatorAccountId = getTestCreatorAccountId();
  scheduleInfo.mPayerAccountId = getTestPayerAccountId();
  scheduleInfo.mScheduledTransactionId = getTestScheduledTransactionId();
  scheduleInfo.mLedgerId = getTestLedgerId();
  scheduleInfo.mWaitForExpiry = getTestWaitForExpiry();

  // When
  const std::unique_ptr<proto::ScheduleInfo> protoScheduleInfo = scheduleInfo.toProtobuf();

  // Then
  EXPECT_EQ(protoScheduleInfo->scheduleid().shardnum(), getTestScheduleId().mShardNum);
  EXPECT_EQ(protoScheduleInfo->scheduleid().realmnum(), getTestScheduleId().mRealmNum);
  EXPECT_EQ(protoScheduleInfo->scheduleid().schedulenum(), getTestScheduleId().mScheduleNum);
  EXPECT_EQ(protoScheduleInfo->execution_time().seconds(),
            internal::TimestampConverter::toProtobuf(getTestExecutionTime())->seconds());
  EXPECT_EQ(protoScheduleInfo->expirationtime().seconds(),
            internal::TimestampConverter::toProtobuf(getTestExecutionTime())->seconds());
  EXPECT_TRUE(protoScheduleInfo->scheduledtransactionbody().has_cryptocreateaccount());
  EXPECT_EQ(protoScheduleInfo->memo(), getTestMemo());
  EXPECT_EQ(protoScheduleInfo->adminkey().ed25519(),
            internal::Utilities::byteVectorToString(getTestAdminKey()->toBytesRaw()));
  EXPECT_EQ(protoScheduleInfo->signers().keys_size(), getTestSigners().size());
  EXPECT_EQ(protoScheduleInfo->creatoraccountid().shardnum(), getTestCreatorAccountId().mShardNum);
  EXPECT_EQ(protoScheduleInfo->creatoraccountid().realmnum(), getTestCreatorAccountId().mRealmNum);
  EXPECT_EQ(protoScheduleInfo->creatoraccountid().accountnum(), getTestCreatorAccountId().mAccountNum);
  EXPECT_EQ(protoScheduleInfo->payeraccountid().shardnum(), getTestPayerAccountId().mShardNum);
  EXPECT_EQ(protoScheduleInfo->payeraccountid().realmnum(), getTestPayerAccountId().mRealmNum);
  EXPECT_EQ(protoScheduleInfo->payeraccountid().accountnum(), getTestPayerAccountId().mAccountNum);
  EXPECT_EQ(protoScheduleInfo->scheduledtransactionid().accountid().shardnum(),
            getTestScheduledTransactionId().mAccountId.mShardNum);
  EXPECT_EQ(protoScheduleInfo->scheduledtransactionid().accountid().realmnum(),
            getTestScheduledTransactionId().mAccountId.mRealmNum);
  EXPECT_EQ(protoScheduleInfo->scheduledtransactionid().accountid().accountnum(),
            getTestScheduledTransactionId().mAccountId.mAccountNum);
  EXPECT_EQ(protoScheduleInfo->scheduledtransactionid().transactionvalidstart().seconds(),
            internal::TimestampConverter::toProtobuf(getTestScheduledTransactionId().mValidTransactionTime)->seconds());
  EXPECT_EQ(protoScheduleInfo->ledger_id(), internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));
  EXPECT_EQ(protoScheduleInfo->wait_for_expiry(), getTestWaitForExpiry());
}

//-----
TEST_F(ScheduleInfoUnitTests, ToBytes)
{
  // Given
  ScheduleInfo scheduleInfo;
  scheduleInfo.mScheduleId = getTestScheduleId();
  scheduleInfo.mExecutionTime = getTestExecutionTime();
  scheduleInfo.mExpirationTime = getTestExpirationTime();
  scheduleInfo.mScheduledTransaction = getTestSchedulableTransaction();
  scheduleInfo.mMemo = getTestMemo();
  scheduleInfo.mAdminKey = getTestAdminKey();
  scheduleInfo.mSignatories = getTestSigners();
  scheduleInfo.mCreatorAccountId = getTestCreatorAccountId();
  scheduleInfo.mPayerAccountId = getTestPayerAccountId();
  scheduleInfo.mScheduledTransactionId = getTestScheduledTransactionId();
  scheduleInfo.mLedgerId = getTestLedgerId();
  scheduleInfo.mWaitForExpiry = getTestWaitForExpiry();

  // When
  const std::vector<std::byte> bytes = scheduleInfo.toBytes();

  // Then
  EXPECT_EQ(bytes, internal::Utilities::stringToByteVector(scheduleInfo.toProtobuf()->SerializeAsString()));
}
