// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountDeleteTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(AccountDeleteTransactionIntegrationTests, ExecuteAccountDeleteTransaction)
{
  // Given
  const std::shared_ptr<PrivateKey> key = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(key->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountDeleteTransaction()
                                 .setDeleteAccountId(accountId)
                                 .setTransferAccountId(AccountId(2ULL))
                                 .freezeWith(&getTestClient())
                                 .sign(key)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
}

//-----
TEST_F(AccountDeleteTransactionIntegrationTests, CannotDeleteInvalidAccountId)
{
  // Given / When / Then
  EXPECT_THROW(AccountDeleteTransaction().setTransferAccountId(AccountId(2ULL)).execute(getTestClient()),
               PrecheckStatusException);
}

//-----
TEST_F(AccountDeleteTransactionIntegrationTests, CannotDeleteAccountWithoutSignature)
{
  // Given
  const std::shared_ptr<PrivateKey> key = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(key->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = AccountDeleteTransaction()
                             .setDeleteAccountId(accountId)
                             .setTransferAccountId(AccountId(2ULL))
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(key)
                    .execute(getTestClient()));
}
