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
#include "TokenDissociateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenDissociateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenDissociateTransactionIntegrationTests, ExecuteTokenDissociateTransaction)
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
                              .setDecimals(3U)
                              .setInitialSupply(100000ULL)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setFreezeKey(operatorKey)
                              .setWipeKey(operatorKey)
                              .setKycKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .setFeeScheduleKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenAssociateTransaction()
                                .setAccountId(accountId)
                                .setTokenIds({ tokenId })
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When / Then
  EXPECT_NO_THROW(txReceipt = TokenDissociateTransaction()
                                .setAccountId(accountId)
                                .setTokenIds({ tokenId })
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenDissociateTransactionIntegrationTests, CanDissociateNoTokens)
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
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenDissociateTransaction()
                                .setAccountId(accountId)
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(TokenDissociateTransactionIntegrationTests, CannotDissociateWithNoAccountIdSet)
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
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = TokenDissociateTransaction()
                             .freezeWith(&getTestClient())
                             .sign(accountKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_ACCOUNT_ID

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(TokenDissociateTransactionIntegrationTests, CannotDissociateIfDissociatingAccountDoesNotSign)
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
                              .setDecimals(3U)
                              .setInitialSupply(100000ULL)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setFreezeKey(operatorKey)
                              .setWipeKey(operatorKey)
                              .setKycKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .setFeeScheduleKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenAssociateTransaction()
                                .setAccountId(accountId)
                                .setTokenIds({ tokenId })
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When / Then
  EXPECT_THROW(txReceipt = TokenDissociateTransaction()
                             .setAccountId(accountId)
                             .setTokenIds({ tokenId })
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenDissociateTransactionIntegrationTests, CannotDissociateIfNotAssociated)
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
                              .setDecimals(3U)
                              .setInitialSupply(100000ULL)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setFreezeKey(operatorKey)
                              .setWipeKey(operatorKey)
                              .setKycKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .setFeeScheduleKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When / Then
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = TokenDissociateTransaction()
                             .setAccountId(accountId)
                             .setTokenIds({ tokenId })
                             .freezeWith(&getTestClient())
                             .sign(accountKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // TOKEN_NOT_ASSOCIATED_TO_ACCOUNT

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}
