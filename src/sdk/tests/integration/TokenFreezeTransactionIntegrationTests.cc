// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenFreezeTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenFreezeTransactionIntegrationTests, ExecuteTokenFreezeTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> accountKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(accountKey)
                                .setInitialBalance(Hbar(1LL))
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
                              .setFreezeKey(operatorKey)
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

  // When
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = TokenFreezeTransaction()
                                                         .setAccountId(accountId)
                                                         .setTokenId(tokenId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(accountKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                      .addTokenTransfer(tokenId, AccountId(2ULL), -10LL)
                                                      .addTokenTransfer(tokenId, accountId, 10LL)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // ACCOUNT_FROZEN_FOR_TOKEN

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountDeleteTransaction()
                                                         .setDeleteAccountId(accountId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .freezeWith(&getTestClient())
                                                         .sign(accountKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenFreezeTransactionIntegrationTests, CannotFreezeWithNoTokenId)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> accountKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(accountKey)
                                .setInitialBalance(Hbar(1LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenFreezeTransaction()
                                                      .setAccountId(accountId)
                                                      .freezeWith(&getTestClient())
                                                      .sign(accountKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_TOKEN_ID

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountDeleteTransaction()
                                                         .setDeleteAccountId(accountId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .freezeWith(&getTestClient())
                                                         .sign(accountKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
}

//-----
TEST_F(TokenFreezeTransactionIntegrationTests, CannotFreezeWithNoAccountId)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000ULL)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setFreezeKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TokenFreezeTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_ACCOUNT_ID

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenFreezeTransactionIntegrationTests, CannotFreezeTokenOnAccountWithNoAssociation)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> accountKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(accountKey)
                                .setInitialBalance(Hbar(1LL))
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
                              .setFreezeKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenFreezeTransaction()
                                                      .setAccountId(accountId)
                                                      .setTokenId(tokenId)
                                                      .freezeWith(&getTestClient())
                                                      .sign(accountKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // TOKEN_NOT_ASSOCIATED_TO_ACCOUNT

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountDeleteTransaction()
                                                         .setDeleteAccountId(accountId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .freezeWith(&getTestClient())
                                                         .sign(accountKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}
