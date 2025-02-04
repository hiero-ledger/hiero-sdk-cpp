// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "FileUpdateTransaction.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FileUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(FileUpdateTransactionIntegrationTests, ExecuteFileUpdateTransaction)
{
  // Given
  const std::string memo = "file create memo";
  const std::string newMemo = "update file memo";

  std::unique_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  std::vector<std::byte> contents;
  std::vector<std::byte> newContents;
  FileId fileId;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(
    newKey = ED25519PrivateKey::fromString(
      "302e020100300506032b6570042204209d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60"));
  ASSERT_NO_THROW(contents = internal::Utilities::stringToByteVector("[e2e::FileCreateTransaction]"));
  ASSERT_NO_THROW(newContents = internal::Utilities::stringToByteVector("[e2e::FileUpdateTransaction]"));
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(contents)
                             .setFileMemo(memo)
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = FileUpdateTransaction()
                                .setFileId(fileId)
                                .setKeys({ newKey->getPublicKey() })
                                .setContents(newContents)
                                .setFileMemo(newMemo)
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  FileInfo fileInfo;
  ASSERT_NO_THROW(fileInfo = FileInfoQuery().setFileId(fileId).execute(getTestClient()));

  EXPECT_EQ(fileInfo.mSize, contents.size());
  EXPECT_FALSE(fileInfo.mIsDeleted);
  EXPECT_EQ(fileInfo.mAdminKeys.toBytes(), KeyList::of({ newKey->getPublicKey() }).toBytes());
  EXPECT_EQ(fileInfo.mMemo, newMemo);

  // Clean up
  ASSERT_NO_THROW(txReceipt = FileDeleteTransaction()
                                .setFileId(fileId)
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(FileUpdateTransactionIntegrationTests, CannotUpdateImmutableFile)
{
  // Given
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    newKey = ED25519PrivateKey::fromString(
      "302e020100300506032b6570042204209d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60"));
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setContents(internal::Utilities::stringToByteVector("[e2e::FileCreateTransaction]"))
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = FileUpdateTransaction()
                                                      .setFileId(fileId)
                                                      .setKeys({ newKey->getPublicKey() })
                                                      .setContents("[e2e::FileUpdateTransaction]")
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // UNAUTHORIZED
}

//-----
TEST_F(FileUpdateTransactionIntegrationTests, CannotUpdateWithNoFileId)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = FileUpdateTransaction()
                                                      .setContents("[e2e::FileUpdateTransaction]")
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_FILE_ID
}
