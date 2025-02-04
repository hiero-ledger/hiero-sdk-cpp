// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "AccountRecords.h"
#include "AccountRecordsQuery.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "Defaults.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountRecordsQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(AccountRecordsQueryIntegrationTests, ExecuteAccountRecordsQuery)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId operatorAccountId(2ULL);
  const Hbar amount(1LL);
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .setInitialBalance(amount)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addHbarTransfer(operatorAccountId, amount.negated())
                                                         .addHbarTransfer(accountId, amount)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addHbarTransfer(operatorAccountId, amount)
                                                         .addHbarTransfer(accountId, amount.negated())
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // When
  AccountRecords accountRecords;
  EXPECT_NO_THROW(accountRecords = AccountRecordsQuery().setAccountId(accountId).execute(getTestClient()));

  // Then
  EXPECT_TRUE(accountRecords.mRecords.empty());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(operatorAccountId)
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountRecordsQueryIntegrationTests, NoAccountId)
{
  // Given / When / Then
  EXPECT_THROW(AccountRecordsQuery().execute(getTestClient()), PrecheckStatusException); // INVALID_ACCOUNT_ID
}
