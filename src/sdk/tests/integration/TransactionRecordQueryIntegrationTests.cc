// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "TransactionId.h"
#include "TransactionRecord.h"
#include "TransactionRecordQuery.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TransactionRecordQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TransactionRecordQueryIntegrationTests, CanGetTransactionRecord)
{
  // Given
  const std::shared_ptr<PrivateKey> testPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> testPublicKey = testPrivateKey->getPublicKey();

  TransactionResponse testTxResponse;
  ASSERT_NO_THROW(testTxResponse = AccountCreateTransaction().setKey(testPublicKey).execute(getTestClient()));

  // When / Then
  TransactionRecord txRecord;
  EXPECT_NO_THROW(txRecord =
                    TransactionRecordQuery().setTransactionId(testTxResponse.mTransactionId).execute(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(txRecord.mReceipt->mAccountId.value())
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
}