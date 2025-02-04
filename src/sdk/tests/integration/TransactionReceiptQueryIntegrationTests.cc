// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TransactionReceiptQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TransactionReceiptQueryIntegrationTests, CanGetTransactionReceipt)
{
  // Given
  const std::shared_ptr<PrivateKey> testPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> testPublicKey = testPrivateKey->getPublicKey();

  TransactionResponse testTxResponse;
  ASSERT_NO_THROW(testTxResponse = AccountCreateTransaction().setKey(testPublicKey).execute(getTestClient()));

  // When / Then
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt =
                    TransactionReceiptQuery().setTransactionId(testTxResponse.mTransactionId).execute(getTestClient()));
  EXPECT_EQ(txReceipt.mTransactionId, testTxResponse.mTransactionId);

  // Clean up
  AccountId accountId;
  ASSERT_NO_THROW(accountId = txReceipt.mAccountId.value());
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
}