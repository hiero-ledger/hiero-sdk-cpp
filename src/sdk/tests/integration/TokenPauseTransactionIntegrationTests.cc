// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenPauseTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenPauseTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenPauseTransactionIntegrationTests, ExecuteTokenPauseTransaction)
{
  // Given
  const int64_t amount = 10LL;

  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> accountKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setInitialBalance(Hbar(1LL))
                                .setKey(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000ULL)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setPauseKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TokenAssociateTransaction()
                                                         .setAccountId(accountId)
                                                         .setTokenIds({ tokenId })
                                                         .freezeWith(&getTestClient())
                                                         .sign(accountKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addTokenTransfer(tokenId, accountId, amount)
                                                         .addTokenTransfer(tokenId, AccountId(2ULL), -amount)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // When
  EXPECT_NO_THROW(const TransactionReceipt txReceipt =
                    TokenPauseTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));

  // Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                      .addTokenTransfer(tokenId, accountId, -amount)
                                                      .addTokenTransfer(tokenId, AccountId(2ULL), amount)
                                                      .freezeWith(&getTestClient())
                                                      .sign(accountKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // TOKEN_IS_PAUSED
}

//-----
TEST_F(TokenPauseTransactionIntegrationTests, CannotPauseWithNoTokenId)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TokenPauseTransaction().execute(getTestClient()).getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_TOKEN_ID
}
