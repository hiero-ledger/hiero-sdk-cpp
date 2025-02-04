// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "TransactionId.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <chrono>
#include <gtest/gtest.h>

using namespace Hiero;

class TransactionIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestValidStartTime() const
  {
    return mTestValidStartTime;
  }
  [[nodiscard]] inline bool getTestScheduled() const { return mTestScheduled; }
  [[nodiscard]] inline int getTestNonce() const { return mTestNonce; }

private:
  const AccountId mTestAccountId = AccountId(1ULL);
  const std::chrono::system_clock::time_point mTestValidStartTime = std::chrono::system_clock::now();
  const bool mTestScheduled = true;
  const int mTestNonce = 2;
};

//-----
TEST_F(TransactionIdUnitTests, WithValidStart)
{
  // Given / When
  const TransactionId transactionId = TransactionId::withValidStart(getTestAccountId(), getTestValidStartTime());

  // Then
  EXPECT_EQ(transactionId.mAccountId, getTestAccountId());
  EXPECT_EQ(transactionId.mValidTransactionTime, getTestValidStartTime());
}

//-----
TEST_F(TransactionIdUnitTests, GenerateTransactionId)
{
  // Given
  const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

  // When
  const TransactionId transactionId = TransactionId::generate(getTestAccountId());

  // Then
  EXPECT_EQ(transactionId.mAccountId, getTestAccountId());
  // No real good way to verify here, just checking that the TransactionId validTransactionTime was made after the time
  // taken above.
  EXPECT_GE(transactionId.mValidTransactionTime.time_since_epoch().count(), now.time_since_epoch().count());
}

//-----
TEST_F(TransactionIdUnitTests, FromProtobuf)
{
  // Given
  proto::TransactionID proto;
  proto.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  proto.set_allocated_transactionvalidstart(internal::TimestampConverter::toProtobuf(getTestValidStartTime()));
  proto.set_scheduled(getTestScheduled());
  proto.set_nonce(getTestNonce());

  // When
  const TransactionId transactionId = TransactionId::fromProtobuf(proto);

  // Then
  EXPECT_EQ(transactionId.mAccountId, getTestAccountId());
  EXPECT_EQ(transactionId.mValidTransactionTime, getTestValidStartTime());
  EXPECT_EQ(transactionId.getScheduled(), getTestScheduled());
  EXPECT_EQ(transactionId.getNonce(), getTestNonce());
}

//-----
TEST_F(TransactionIdUnitTests, FromString)
{
  // Given
  const std::chrono::system_clock::duration durationSinceEpoch = getTestValidStartTime().time_since_epoch();
  const std::chrono::seconds secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);
  const std::string validStartTimeStr =
    std::to_string(secondsSinceEpoch.count()) + '.' +
    std::to_string(
      std::chrono::duration_cast<std::chrono::nanoseconds>(durationSinceEpoch - secondsSinceEpoch).count());

  // When
  const TransactionId transactionId = TransactionId::fromString(
    getTestAccountId().toString() + '@' + validStartTimeStr + (getTestScheduled() ? "?scheduled" : "") +
    (getTestNonce() != 0 ? ('/' + std::to_string(getTestNonce())) : ""));

  // Then
  EXPECT_EQ(transactionId.mAccountId, getTestAccountId());
  EXPECT_EQ(transactionId.mValidTransactionTime, getTestValidStartTime());
  EXPECT_EQ(transactionId.getScheduled(), getTestScheduled());
  EXPECT_EQ(transactionId.getNonce(), getTestNonce());
}

//-----
TEST_F(TransactionIdUnitTests, FromBytes)
{
  // Given
  proto::TransactionID proto;
  proto.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  proto.set_allocated_transactionvalidstart(internal::TimestampConverter::toProtobuf(getTestValidStartTime()));
  proto.set_scheduled(getTestScheduled());
  proto.set_nonce(getTestNonce());

  // When
  const TransactionId transactionId =
    TransactionId::fromBytes(internal::Utilities::stringToByteVector(proto.SerializeAsString()));

  // Then
  EXPECT_EQ(transactionId.mAccountId, getTestAccountId());
  EXPECT_EQ(transactionId.mValidTransactionTime, getTestValidStartTime());
  EXPECT_EQ(transactionId.getScheduled(), getTestScheduled());
  EXPECT_EQ(transactionId.getNonce(), getTestNonce());
}

//-----
TEST_F(TransactionIdUnitTests, ToProtobuf)
{
  // Given
  TransactionId transactionId;
  transactionId.mAccountId = getTestAccountId();
  transactionId.mValidTransactionTime = getTestValidStartTime();
  transactionId.setScheduled(getTestScheduled());
  transactionId.setNonce(getTestNonce());

  // When
  const std::unique_ptr<proto::TransactionID> proto = transactionId.toProtobuf();

  // Then
  EXPECT_EQ(AccountId::fromProtobuf(proto->accountid()), getTestAccountId());
  EXPECT_EQ(internal::TimestampConverter::fromProtobuf(proto->transactionvalidstart()), getTestValidStartTime());
  EXPECT_EQ(proto->scheduled(), getTestScheduled());
  EXPECT_EQ(proto->nonce(), getTestNonce());
}

//-----
TEST_F(TransactionIdUnitTests, ToString)
{
  // Given
  const std::chrono::system_clock::duration durationSinceEpoch = getTestValidStartTime().time_since_epoch();
  const std::chrono::seconds secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);
  const std::string validStartTimeStr =
    std::to_string(secondsSinceEpoch.count()) + '.' +
    std::to_string(
      std::chrono::duration_cast<std::chrono::nanoseconds>(durationSinceEpoch - secondsSinceEpoch).count());

  TransactionId transactionId;
  transactionId.mAccountId = getTestAccountId();
  transactionId.mValidTransactionTime = getTestValidStartTime();
  transactionId.setScheduled(getTestScheduled());
  transactionId.setNonce(getTestNonce());

  // When
  const std::string str = transactionId.toString();

  // Then
  EXPECT_EQ(str,
            getTestAccountId().toString() + '@' + validStartTimeStr + (getTestScheduled() ? "?scheduled" : "") +
              (getTestNonce() != 0 ? ('/' + std::to_string(getTestNonce())) : ""));
}

//-----
TEST_F(TransactionIdUnitTests, ToBytes)
{
  // Given
  TransactionId transactionId;
  transactionId.mAccountId = getTestAccountId();
  transactionId.mValidTransactionTime = getTestValidStartTime();
  transactionId.setScheduled(getTestScheduled());
  transactionId.setNonce(getTestNonce());

  // When
  const std::vector<std::byte> bytes = transactionId.toBytes();

  // Then
  EXPECT_EQ(bytes, internal::Utilities::stringToByteVector(transactionId.toProtobuf()->SerializeAsString()));
}

//-----
TEST_F(TransactionIdUnitTests, SetGetScheduled)
{
  // Given
  TransactionId transactionId;

  // When
  transactionId.setScheduled(getTestScheduled());

  // Then
  EXPECT_EQ(transactionId.getScheduled(), getTestScheduled());
}

//-----
TEST_F(TransactionIdUnitTests, SetGetNonce)
{
  // Given
  TransactionId transactionId;

  // When
  transactionId.setNonce(getTestNonce());

  // Then
  EXPECT_EQ(transactionId.getNonce(), getTestNonce());
}
