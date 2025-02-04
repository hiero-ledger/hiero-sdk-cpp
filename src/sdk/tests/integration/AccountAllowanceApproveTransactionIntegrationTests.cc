// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountAllowanceApproveTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(AccountAllowanceApproveTransactionIntegrationTests, ExecuteAccountAllowanceApproveTransaction)
{
  // Given
  const Hbar amount(5LL);
  const std::shared_ptr<PrivateKey> allowerKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> alloweeKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  AccountId allowerAccountId;
  AccountId alloweeAccountId;
  ASSERT_NO_THROW(allowerAccountId = AccountCreateTransaction()
                                       .setKey(allowerKey->getPublicKey())
                                       .setInitialBalance(amount)
                                       .execute(getTestClient())
                                       .getReceipt(getTestClient())
                                       .mAccountId.value());
  ASSERT_NO_THROW(alloweeAccountId = AccountCreateTransaction()
                                       .setKey(alloweeKey->getPublicKey())
                                       .setInitialBalance(amount)
                                       .execute(getTestClient())
                                       .getReceipt(getTestClient())
                                       .mAccountId.value());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountAllowanceApproveTransaction()
                                 .approveHbarAllowance(allowerAccountId, alloweeAccountId, amount)
                                 .freezeWith(&getTestClient())
                                 .sign(allowerKey)
                                 .execute(getTestClient()));

  // Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = txResponse.getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(allowerAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(allowerKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(alloweeAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(alloweeKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountAllowanceApproveTransactionIntegrationTests, CannotAllowAllowanceWithoutAllowerSignature)
{
  // Given
  const Hbar amount(5LL);
  const std::shared_ptr<PrivateKey> allowerKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> alloweeKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  AccountId allowerAccountId;
  AccountId alloweeAccountId;
  ASSERT_NO_THROW(allowerAccountId = AccountCreateTransaction()
                                       .setKey(allowerKey->getPublicKey())
                                       .setInitialBalance(amount)
                                       .execute(getTestClient())
                                       .getReceipt(getTestClient())
                                       .mAccountId.value());
  ASSERT_NO_THROW(alloweeAccountId = AccountCreateTransaction()
                                       .setKey(alloweeKey->getPublicKey())
                                       .setInitialBalance(amount)
                                       .execute(getTestClient())
                                       .getReceipt(getTestClient())
                                       .mAccountId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountAllowanceApproveTransaction()
                                                      .approveHbarAllowance(allowerAccountId, alloweeAccountId, amount)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(allowerAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(allowerKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(alloweeAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(alloweeKey)
                    .execute(getTestClient()));
}
