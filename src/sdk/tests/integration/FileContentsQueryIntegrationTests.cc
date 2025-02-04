// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileContentsQuery.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FileContentsQueryIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::vector<std::byte>& getTestFileContents() const { return mTestFileContents; }

private:
  const std::vector<std::byte> mTestFileContents = { std::byte(0x01), std::byte(0x02), std::byte(0x03) };
};

//-----
TEST_F(FileContentsQueryIntegrationTests, ExecuteFileContentsQuery)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(getTestFileContents())
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  FileContents fileContents;
  EXPECT_NO_THROW(fileContents = FileContentsQuery().setFileId(fileId).execute(getTestClient()));

  // Then
  EXPECT_EQ(fileContents, getTestFileContents());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(FileContentsQueryIntegrationTests, CanQueryEmptyFileContents)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  FileContents fileContents;
  EXPECT_NO_THROW(fileContents = FileContentsQuery().setFileId(fileId).execute(getTestClient()));

  // Then
  EXPECT_TRUE(fileContents.empty());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}
