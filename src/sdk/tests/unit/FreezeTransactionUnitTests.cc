// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FreezeTransaction.h"
#include "KeyList.h"
#include "exceptions/IllegalStateException.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <chrono>
#include <cstddef>
#include <file_update.pb.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class FreezeTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestFileHash() const { return mTestFileHash; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestStartTime() const { return mTestStartTime; }
  [[nodiscard]] inline const FreezeType& getTestFreezeType() const { return mTestFreezeType; }

private:
  const FileId mTestFileId = FileId(1ULL, 2ULL, 3ULL);
  const std::vector<std::byte> mTestFileHash = { std::byte(0x04), std::byte(0x05), std::byte(0x06) };
  const std::chrono::system_clock::time_point mTestStartTime = std::chrono::system_clock::now();
  const FreezeType mTestFreezeType = FreezeType::TELEMETRY_UPGRADE;
};

//-----
TEST_F(FreezeTransactionUnitTests, ConstructFreezeTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::FreezeTransactionBody>();
  body->set_allocated_update_file(getTestFileId().toProtobuf().release());
  body->set_file_hash(internal::Utilities::byteVectorToString(getTestFileHash()));
  body->set_allocated_start_time(internal::TimestampConverter::toProtobuf(getTestStartTime()));
  body->set_freeze_type(gFreezeTypeToProtobufFreezeType.at(getTestFreezeType()));

  proto::TransactionBody txBody;
  txBody.set_allocated_freeze(body.release());

  // When
  const FreezeTransaction freezeTransaction(txBody);

  // Then
  ASSERT_TRUE(freezeTransaction.getFileId().has_value());
  EXPECT_EQ(freezeTransaction.getFileId().value(), getTestFileId());
  EXPECT_EQ(freezeTransaction.getFileHash(), getTestFileHash());
  ASSERT_TRUE(freezeTransaction.getStartTime().has_value());
  EXPECT_EQ(freezeTransaction.getStartTime().value(), getTestStartTime());
  EXPECT_EQ(freezeTransaction.getFreezeType(), getTestFreezeType());
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetFileId)
{
  // Given
  FreezeTransaction transaction;

  // When
  ASSERT_NO_THROW(transaction.setFileId(getTestFileId()));

  // Then
  ASSERT_TRUE(transaction.getFileId().has_value());
  EXPECT_EQ(transaction.getFileId(), getTestFileId());
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetFileIdFrozen)
{
  // Given
  FreezeTransaction transaction = FreezeTransaction()
                                    .setNodeAccountIds({ AccountId(1ULL) })
                                    .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileId(getTestFileId()), IllegalStateException);
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetFileHash)
{
  // Given
  FreezeTransaction transaction;

  // When
  ASSERT_NO_THROW(transaction.setFileHash(getTestFileHash()));

  // Then
  EXPECT_EQ(transaction.getFileHash(), getTestFileHash());
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetFileHashFrozen)
{
  // Given
  FreezeTransaction transaction = FreezeTransaction()
                                    .setNodeAccountIds({ AccountId(1ULL) })
                                    .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileHash(getTestFileHash()), IllegalStateException);
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetStartTime)
{
  // Given
  FreezeTransaction transaction;

  // When
  ASSERT_NO_THROW(transaction.setStartTime(getTestStartTime()));

  // Then
  ASSERT_TRUE(transaction.getStartTime().has_value());
  EXPECT_EQ(transaction.getStartTime(), getTestStartTime());
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetStartTimeFrozen)
{
  // Given
  FreezeTransaction transaction = FreezeTransaction()
                                    .setNodeAccountIds({ AccountId(1ULL) })
                                    .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setStartTime(getTestStartTime()), IllegalStateException);
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetFreezeType)
{
  // Given
  FreezeTransaction transaction;

  // When
  ASSERT_NO_THROW(transaction.setFreezeType(getTestFreezeType()));

  // Then
  EXPECT_EQ(transaction.getFreezeType(), getTestFreezeType());
}

//-----
TEST_F(FreezeTransactionUnitTests, GetSetFreezeTypeFrozen)
{
  // Given
  FreezeTransaction transaction = FreezeTransaction()
                                    .setNodeAccountIds({ AccountId(1ULL) })
                                    .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFreezeType(getTestFreezeType()), IllegalStateException);
}
