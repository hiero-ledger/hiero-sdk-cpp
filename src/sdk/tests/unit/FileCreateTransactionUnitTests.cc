// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "KeyList.h"
#include "exceptions/IllegalStateException.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <chrono>
#include <cstddef>
#include <file_create.pb.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class FileCreateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::vector<std::shared_ptr<Key>>& getTestKeyVector() const { return mTestKeyVector; }
  [[nodiscard]] inline const KeyList& getTestKeyList() const { return mTestKeyList; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestContents() const { return mTestContents; }
  [[nodiscard]] inline const std::string& getTestMemo() const { return mTestMemo; }

private:
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const std::vector<std::shared_ptr<Key>> mTestKeyVector = {
    ED25519PrivateKey::fromString(
      "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e10"),
    ED25519PrivateKey::fromString(
      "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e11"),
    ED25519PrivateKey::fromString(
      "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e12")
  };
  const KeyList mTestKeyList = KeyList::of(mTestKeyVector);
  const std::vector<std::byte> mTestContents = { std::byte(0x08), std::byte(0x09), std::byte(0x10) };
  const std::string mTestMemo = "test file memo";
};

//-----
TEST_F(FileCreateTransactionUnitTests, ConstructFileCreateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::FileCreateTransactionBody>();
  body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->set_allocated_keys(getTestKeyList().toProtobuf().release());
  body->set_contents(internal::Utilities::byteVectorToString(getTestContents()));
  body->set_memo(getTestMemo());

  proto::TransactionBody txBody;
  txBody.set_allocated_filecreate(body.release());

  // When
  const FileCreateTransaction fileCreateTransaction(txBody);

  // Then
  EXPECT_EQ(fileCreateTransaction.getExpirationTime(), getTestExpirationTime());
  EXPECT_EQ(fileCreateTransaction.getKeys().toBytes(), getTestKeyList().toBytes());
  EXPECT_EQ(fileCreateTransaction.getContents(), getTestContents());
  EXPECT_EQ(fileCreateTransaction.getFileMemo(), getTestMemo());
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetExpirationTime)
{
  // Given
  FileCreateTransaction transaction;

  // When
  transaction.setExpirationTime(getTestExpirationTime());

  // Then
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetExpirationTimeFrozen)
{
  // Given
  FileCreateTransaction transaction = FileCreateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetKeys)
{
  // Given
  FileCreateTransaction transactionWithKeyVector;
  FileCreateTransaction transactionWithKeyList;

  // When
  transactionWithKeyVector.setKeys(getTestKeyVector());
  transactionWithKeyList.setKeys(getTestKeyList());

  // Then
  const std::vector<std::byte> testKeyListBytes = getTestKeyList().toBytes();
  EXPECT_EQ(transactionWithKeyVector.getKeys().toBytes(), testKeyListBytes);
  EXPECT_EQ(transactionWithKeyList.getKeys().toBytes(), testKeyListBytes);
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetKeysFrozen)
{
  // Given
  FileCreateTransaction transaction = FileCreateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setKeys(getTestKeyList()), IllegalStateException);
  EXPECT_THROW(transaction.setKeys(getTestKeyVector()), IllegalStateException);
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetContents)
{
  // Given
  FileCreateTransaction transaction;

  // When
  transaction.setContents(getTestContents());

  // Then
  EXPECT_EQ(transaction.getContents(), getTestContents());
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetContentsFrozen)
{
  // Given
  FileCreateTransaction transaction = FileCreateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContents(getTestContents()), IllegalStateException);
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetMemo)
{
  // Given
  FileCreateTransaction transaction;

  // When
  transaction.setFileMemo(getTestMemo());

  // Then
  EXPECT_EQ(transaction.getFileMemo(), getTestMemo());
}

//-----
TEST_F(FileCreateTransactionUnitTests, GetSetMemoFrozen)
{
  // Given
  FileCreateTransaction transaction = FileCreateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileMemo(getTestMemo()), IllegalStateException);
}
