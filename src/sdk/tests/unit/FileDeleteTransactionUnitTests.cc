// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileDeleteTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <file_delete.pb.h>
#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class FileDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }

private:
  const FileId mTestFileId = FileId(1ULL);
};

//-----
TEST_F(FileDeleteTransactionUnitTests, ConstructFileDeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::FileDeleteTransactionBody>();
  body->set_allocated_fileid(getTestFileId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_filedelete(body.release());

  // When
  const FileDeleteTransaction fileDeleteTransaction(txBody);

  // Then
  EXPECT_EQ(fileDeleteTransaction.getFileId(), getTestFileId());
}

//-----
TEST_F(FileDeleteTransactionUnitTests, GetSetFileId)
{
  // Given
  FileDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFileId(getTestFileId()));

  // Then
  EXPECT_EQ(transaction.getFileId(), getTestFileId());
}

//-----
TEST_F(FileDeleteTransactionUnitTests, GetSetFileIdFrozen)
{
  // Given
  FileDeleteTransaction transaction = FileDeleteTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileId(getTestFileId()), IllegalStateException);
}
