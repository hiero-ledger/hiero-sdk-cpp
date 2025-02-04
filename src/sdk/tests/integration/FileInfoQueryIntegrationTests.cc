// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "impl/Utilities.h"

#include <chrono>
#include <gtest/gtest.h>

using namespace Hiero;

class FileInfoQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(FileInfoQueryIntegrationTests, ExecuteFileInfoQuery)
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

  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys(keys)
                             .setContents(contents)
                             .setFileMemo(memo)
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  FileInfo fileInfo;
  EXPECT_NO_THROW(fileInfo = FileInfoQuery().setFileId(fileId).execute(getTestClient()));

  // Then
  EXPECT_EQ(fileInfo.mFileId, fileId);
  EXPECT_EQ(fileInfo.mSize, contents.size());
  EXPECT_GE(fileInfo.mExpirationTime, std::chrono::system_clock::now());
  EXPECT_FALSE(fileInfo.mIsDeleted);
  EXPECT_EQ(fileInfo.mAdminKeys.toBytes(), keys.toBytes());
  EXPECT_EQ(fileInfo.mMemo, memo);

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(FileInfoQueryIntegrationTests, NoFileId)
{
  // Given / When / Then
  EXPECT_THROW(const FileInfo fileInfo = FileInfoQuery().execute(getTestClient()),
               PrecheckStatusException); // INVALID_FILE_ID
}

//-----
TEST_F(FileInfoQueryIntegrationTests, CanQueryFileWithNoAdminKeyOrContents)
{
  // Given
  FileId fileId;
  ASSERT_NO_THROW(fileId =
                    FileCreateTransaction().execute(getTestClient()).getReceipt(getTestClient()).mFileId.value());

  // When
  FileInfo fileInfo;
  EXPECT_NO_THROW(fileInfo = FileInfoQuery().setFileId(fileId).execute(getTestClient()));

  // Then
  EXPECT_EQ(fileInfo.mFileId, fileId);
  EXPECT_EQ(fileInfo.mSize, 0);
  EXPECT_GE(fileInfo.mExpirationTime, std::chrono::system_clock::now());
  EXPECT_FALSE(fileInfo.mIsDeleted);
  EXPECT_TRUE(fileInfo.mAdminKeys.empty());
}
