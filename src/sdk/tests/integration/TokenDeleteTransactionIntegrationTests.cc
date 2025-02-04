// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenDeleteTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenDeleteTransactionIntegrationTests, ExecuteTokenDeleteTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setDecimals(3)
                              .setInitialSupply(100000)
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

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()));

  // Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = txResponse.getReceipt(getTestClient()));
}

//-----
TEST_F(TokenDeleteTransactionIntegrationTests, CannotDeleteTokenWithNoAdminKeySignature)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  std::shared_ptr<PrivateKey> adminKey;
  ASSERT_NO_THROW(adminKey = ED25519PrivateKey::generatePrivateKey());

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(adminKey)
                              .freezeWith(&getTestClient())
                              .sign(adminKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TokenDeleteTransaction()
                                                         .setTokenId(tokenId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(adminKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
}

//-----
TEST_F(TokenDeleteTransactionIntegrationTests, CannotDeleteTokenWithNoTokenId)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TokenDeleteTransaction().execute(getTestClient()).getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_TOKEN_ID
}