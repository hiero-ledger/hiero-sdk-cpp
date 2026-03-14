// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FeeEstimateMode.h"
#include "FeeEstimateQuery.h"
#include "FeeEstimateResponse.h"
#include "Hbar.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FeeEstimateQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
// Note: This test is disabled because the fee estimate REST API may not be available on all networks
TEST_F(FeeEstimateQueryIntegrationTests, DISABLED_EstimateTransferTransactionFee)
{
  // Given
  const std::shared_ptr<PrivateKey> newAccountKey = ED25519PrivateKey::generatePrivateKey();

  // Create a new account to transfer to
  TransactionReceipt createReceipt;
  ASSERT_NO_THROW(createReceipt = AccountCreateTransaction()
                                    .setKeyWithoutAlias(newAccountKey->getPublicKey())
                                    .setInitialBalance(Hbar(1LL))
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient()));
  ASSERT_TRUE(createReceipt.mAccountId.has_value());
  const AccountId newAccountId = createReceipt.mAccountId.value();

  // Create a transfer transaction to estimate
  TransferTransaction transferTx = TransferTransaction()
                                     .addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
                                     .addHbarTransfer(newAccountId, Hbar(1LL));

  // Freeze the transaction
  transferTx.freezeWith(&const_cast<Client&>(getTestClient()));

  // Wrap the transaction
  WrappedTransaction wrappedTx(transferTx);

  // When
  FeeEstimateQuery query;
  query.setTransaction(wrappedTx);
  query.setMode(FeeEstimateMode::STATE);

  FeeEstimateResponse response;
  EXPECT_NO_THROW(response = query.execute(getTestClient()));

  // Then
  EXPECT_GT(response.mTotal, 0ULL);

  // Clean up - delete the created account
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(newAccountId)
                    .setTransferAccountId(getTestClient().getOperatorAccountId().value())
                    .freezeWith(&const_cast<Client&>(getTestClient()))
                    .sign(newAccountKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
}

//-----
// Note: This test is disabled because the fee estimate REST API may not be available on all networks
TEST_F(FeeEstimateQueryIntegrationTests, DISABLED_EstimateAccountCreateTransactionFee)
{
  // Given
  const std::shared_ptr<PrivateKey> newAccountKey = ED25519PrivateKey::generatePrivateKey();

  // Create an account create transaction to estimate
  AccountCreateTransaction createTx =
    AccountCreateTransaction().setKeyWithoutAlias(newAccountKey->getPublicKey()).setInitialBalance(Hbar(1LL));

  // Freeze the transaction
  createTx.freezeWith(&const_cast<Client&>(getTestClient()));

  // Wrap the transaction
  WrappedTransaction wrappedTx(createTx);

  // When
  FeeEstimateQuery query;
  query.setTransaction(wrappedTx);
  query.setMode(FeeEstimateMode::STATE);

  FeeEstimateResponse response;
  EXPECT_NO_THROW(response = query.execute(getTestClient()));

  // Then
  EXPECT_GT(response.mTotal, 0ULL);
}

//-----
TEST_F(FeeEstimateQueryIntegrationTests, GettersAndSetters)
{
  // Given
  const std::shared_ptr<PrivateKey> newAccountKey = ED25519PrivateKey::generatePrivateKey();
  AccountCreateTransaction createTx =
    AccountCreateTransaction().setKeyWithoutAlias(newAccountKey->getPublicKey()).setInitialBalance(Hbar(1LL));
  createTx.freezeWith(&const_cast<Client&>(getTestClient()));
  WrappedTransaction wrappedTx(createTx);

  // When
  FeeEstimateQuery query;
  query.setMode(FeeEstimateMode::TRANSIENT);
  query.setMaxAttempts(5);
  query.setTransaction(wrappedTx);

  // Then
  EXPECT_EQ(query.getMode(), FeeEstimateMode::TRANSIENT);
  EXPECT_EQ(query.getMaxAttempts(), 5ULL);
}

//-----
TEST_F(FeeEstimateQueryIntegrationTests, DefaultMode)
{
  // Given / When
  FeeEstimateQuery query;

  // Then
  EXPECT_EQ(query.getMode(), FeeEstimateMode::STATE);
}
