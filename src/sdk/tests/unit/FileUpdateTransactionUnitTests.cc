// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileUpdateTransaction.h"
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

class FileUpdateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::vector<std::shared_ptr<Key>>& getTestKeyVector() const { return mTestKeyVector; }
  [[nodiscard]] inline const KeyList& getTestKeyList() const { return mTestKeyList; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestContents() const { return mTestContents; }
  [[nodiscard]] inline const std::string& getTestMemo() const { return mTestMemo; }

private:
  const FileId mTestFileId = FileId(1ULL);
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
TEST_F(FileUpdateTransactionUnitTests, ConstructFileUpdateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::FileUpdateTransactionBody>();
  body->set_allocated_fileid(getTestFileId().toProtobuf().release());
  body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->set_allocated_keys(getTestKeyList().toProtobuf().release());
  body->set_contents(internal::Utilities::byteVectorToString(getTestContents()));

  auto strValue = std::make_unique<google::protobuf::StringValue>();
  strValue->set_value(getTestMemo());
  body->set_allocated_memo(strValue.release());

  proto::TransactionBody txBody;
  txBody.set_allocated_fileupdate(body.release());

  // When
  const FileUpdateTransaction fileUpdateTransaction(txBody);

  // Then
  EXPECT_EQ(fileUpdateTransaction.getFileId(), getTestFileId());
  ASSERT_TRUE(fileUpdateTransaction.getExpirationTime().has_value());
  EXPECT_EQ(fileUpdateTransaction.getExpirationTime(), getTestExpirationTime());
  ASSERT_TRUE(fileUpdateTransaction.getKeys().has_value());
  EXPECT_EQ(fileUpdateTransaction.getKeys()->toBytes(), getTestKeyList().toBytes());
  ASSERT_TRUE(fileUpdateTransaction.getContents().has_value());
  EXPECT_EQ(fileUpdateTransaction.getContents(), getTestContents());
  ASSERT_TRUE(fileUpdateTransaction.getFileMemo().has_value());
  EXPECT_EQ(fileUpdateTransaction.getFileMemo(), getTestMemo());
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetFileId)
{
  // Given
  FileUpdateTransaction transaction;

  // When
  transaction.setFileId(getTestFileId());

  // Then
  EXPECT_EQ(transaction.getFileId(), getTestFileId());
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetFileIdFrozen)
{
  // Given
  FileUpdateTransaction transaction = FileUpdateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileId(getTestFileId()), IllegalStateException);
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetExpirationTime)
{
  // Given
  FileUpdateTransaction transaction;

  // When
  transaction.setExpirationTime(getTestExpirationTime());

  // Then
  ASSERT_TRUE(transaction.getExpirationTime().has_value());
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetExpirationTimeFrozen)
{
  // Given
  FileUpdateTransaction transaction = FileUpdateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetKeys)
{
  // Given
  FileUpdateTransaction transactionWithKeyVector;
  FileUpdateTransaction transactionWithKeyList;

  // When
  transactionWithKeyVector.setKeys(getTestKeyVector());
  transactionWithKeyList.setKeys(getTestKeyList());

  // Then
  ASSERT_TRUE(transactionWithKeyVector.getKeys().has_value());
  ASSERT_TRUE(transactionWithKeyList.getKeys().has_value());

  const std::vector<std::byte> testKeyListBytes = getTestKeyList().toBytes();
  EXPECT_EQ(transactionWithKeyVector.getKeys()->toBytes(), testKeyListBytes);
  EXPECT_EQ(transactionWithKeyList.getKeys()->toBytes(), testKeyListBytes);
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetKeysFrozen)
{
  // Given
  FileUpdateTransaction transaction = FileUpdateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setKeys(getTestKeyList()), IllegalStateException);
  EXPECT_THROW(transaction.setKeys(getTestKeyVector()), IllegalStateException);
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetContents)
{
  // Given
  FileUpdateTransaction transactionBytes;
  FileUpdateTransaction transactionStr;

  // When
  transactionBytes.setContents(getTestContents());
  transactionStr.setContents(internal::Utilities::byteVectorToString(getTestContents()));

  // Then
  ASSERT_TRUE(transactionBytes.getContents().has_value());
  ASSERT_TRUE(transactionStr.getContents().has_value());
  EXPECT_EQ(transactionBytes.getContents(), getTestContents());
  EXPECT_EQ(transactionStr.getContents(), getTestContents());
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetContentsFrozen)
{
  // Given
  FileUpdateTransaction transaction = FileUpdateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContents(getTestContents()), IllegalStateException);
  EXPECT_THROW(transaction.setContents(internal::Utilities::byteVectorToString(getTestContents())),
               IllegalStateException);
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetMemo)
{
  // Given
  FileUpdateTransaction transaction;

  // When
  transaction.setFileMemo(getTestMemo());

  // Then
  ASSERT_TRUE(transaction.getFileMemo().has_value());
  EXPECT_EQ(transaction.getFileMemo(), getTestMemo());
}

//-----
TEST_F(FileUpdateTransactionUnitTests, GetSetMemoFrozen)
{
  // Given
  FileUpdateTransaction transaction = FileUpdateTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileMemo(getTestMemo()), IllegalStateException);
}
