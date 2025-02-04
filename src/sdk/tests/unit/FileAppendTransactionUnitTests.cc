// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileAppendTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <chrono>
#include <cstddef>
#include <file_append.pb.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class FileAppendTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestContents() const { return mTestContents; }

private:
  const FileId mTestFileId = FileId(1ULL);
  const std::vector<std::byte> mTestContents = { std::byte(0x02), std::byte(0x03), std::byte(0x04) };
};

//-----
TEST_F(FileAppendTransactionUnitTests, ConstructFileAppendTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::FileAppendTransactionBody>();
  body->set_allocated_fileid(getTestFileId().toProtobuf().release());
  body->set_contents(internal::Utilities::byteVectorToString(getTestContents()));

  proto::TransactionBody txBody;
  txBody.set_allocated_fileappend(body.release());

  // When
  const FileAppendTransaction fileAppendTransaction(txBody);

  // Then
  EXPECT_EQ(fileAppendTransaction.getFileId(), getTestFileId());
  EXPECT_EQ(fileAppendTransaction.getContents(), getTestContents());
}

//-----
TEST_F(FileAppendTransactionUnitTests, GetSetFileId)
{
  // Given
  FileAppendTransaction transaction;

  // When
  transaction.setFileId(getTestFileId());

  // Then
  EXPECT_EQ(transaction.getFileId(), getTestFileId());
}

//-----
TEST_F(FileAppendTransactionUnitTests, GetSetFileIdFrozen)
{
  // Given
  FileAppendTransaction transaction = FileAppendTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileId(getTestFileId()), IllegalStateException);
}

//-----
TEST_F(FileAppendTransactionUnitTests, GetSetContents)
{
  // Given
  FileAppendTransaction transactionBytes;
  FileAppendTransaction transactionStr;

  // When
  transactionBytes.setContents(getTestContents());
  transactionStr.setContents(internal::Utilities::byteVectorToString(getTestContents()));

  // Then
  EXPECT_EQ(transactionBytes.getContents(), getTestContents());
  EXPECT_EQ(transactionStr.getContents(), getTestContents());
}

//-----
TEST_F(FileAppendTransactionUnitTests, GetSetContentsFrozen)
{
  // Given
  FileAppendTransaction transaction = FileAppendTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContents(getTestContents()), IllegalStateException);
  EXPECT_THROW(transaction.setContents(internal::Utilities::byteVectorToString(getTestContents())),
               IllegalStateException);
}
