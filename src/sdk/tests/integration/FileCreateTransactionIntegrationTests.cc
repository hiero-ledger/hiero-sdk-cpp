// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FileCreateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(FileCreateTransactionIntegrationTests, ExecuteFileCreateTransaction)
{
  // Given
  std::unique_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::vector<std::byte> contents;
  ASSERT_NO_THROW(contents = internal::Utilities::stringToByteVector("[e2e::FileCreateTransaction]"));

  const std::string memo = "test file memo";
  const KeyList keys = KeyList::of({ operatorKey->getPublicKey() });

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = FileCreateTransaction()
                                .setKeys(keys)
                                .setContents(contents)
                                .setFileMemo(memo)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  FileId fileId;
  ASSERT_NO_THROW(fileId = txReceipt.mFileId.value());

  FileInfo fileInfo;
  ASSERT_NO_THROW(fileInfo = FileInfoQuery().setFileId(fileId).execute(getTestClient()));

  EXPECT_EQ(fileInfo.mSize, contents.size());
  EXPECT_FALSE(fileInfo.mIsDeleted);
  EXPECT_EQ(fileInfo.mAdminKeys.toBytes(), keys.toBytes());
  EXPECT_EQ(fileInfo.mMemo, memo);

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(FileCreateTransactionIntegrationTests, CanCreateFileWithNoContents)
{
  // Given
  std::unique_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  const KeyList keys = KeyList::of({ operatorKey->getPublicKey() });

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt =
                    FileCreateTransaction().setKeys(keys).execute(getTestClient()).getReceipt(getTestClient()));

  // Then
  FileId fileId;
  ASSERT_NO_THROW(fileId = txReceipt.mFileId.value());

  FileInfo fileInfo;
  ASSERT_NO_THROW(fileInfo = FileInfoQuery().setFileId(fileId).execute(getTestClient()));

  EXPECT_EQ(fileInfo.mSize, 0);
  EXPECT_FALSE(fileInfo.mIsDeleted);
  EXPECT_EQ(fileInfo.mAdminKeys.toBytes(), keys.toBytes());

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(FileCreateTransactionIntegrationTests, CanCreateFileWithNoKey)
{
  // Given / When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = FileCreateTransaction().execute(getTestClient()).getReceipt(getTestClient()));

  // Then
  FileId fileId;
  ASSERT_NO_THROW(fileId = txReceipt.mFileId.value());

  FileInfo fileInfo;
  ASSERT_NO_THROW(fileInfo = FileInfoQuery().setFileId(fileId).execute(getTestClient()));

  EXPECT_EQ(fileInfo.mSize, 0);
  EXPECT_FALSE(fileInfo.mIsDeleted);
  EXPECT_TRUE(fileInfo.mAdminKeys.empty());
}
