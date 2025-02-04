// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "SystemDeleteTransaction.h"
#include "SystemUndeleteTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class SystemUndeleteTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(SystemUndeleteTransactionIntegrationTests, UndeleteFile)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey })
                             .setContents(getTestFileContent())
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    SystemDeleteTransaction()
                      .setFileId(fileId)
                      .setExpirationTime(std::chrono::system_clock::now() + std::chrono::seconds(30))
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt =
                    SystemUndeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}
