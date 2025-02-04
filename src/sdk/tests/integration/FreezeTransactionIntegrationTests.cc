// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "FreezeTransaction.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FreezeTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(FreezeTransactionIntegrationTests, DISABLED_ExecuteFreezeTransaction)
{
  // Given
  std::unique_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt =
                    FreezeTransaction()
                      .setFileId(fileId)
                      .setStartTime(std::chrono::system_clock::now() + std::chrono::seconds(30))
                      .setFreezeType(FreezeType::FREEZE_ONLY)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}
