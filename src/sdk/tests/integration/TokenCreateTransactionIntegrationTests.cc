// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "CustomFixedFee.h"
#include "CustomFractionalFee.h"
#include "CustomRoyaltyFee.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenCreateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenCreateTransactionIntegrationTests, ExecuteTokenCreateTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
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
                                .setPauseKey(operatorKey)
                                .setMetadataKey(operatorKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateTokenWithMinimalPropertiesSet)
{
  // Given / When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("ffff")
                                .setTokenSymbol("F")
                                .setTreasuryAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithNoName)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenCreateTransaction()
                                                      .setTokenSymbol("F")
                                                      .setTreasuryAccountId(AccountId(2ULL))
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // MISSING_TOKEN_NAME
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithNoSymbol)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenCreateTransaction()
                                                      .setTokenName("ffff")
                                                      .setTreasuryAccountId(AccountId(2ULL))
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // MISSING_TOKEN_SYMBOL
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithNoTreasuryAccount)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenCreateTransaction()
                                                      .setTokenName("ffff")
                                                      .setTokenSymbol("F")
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_TREASURY_ACCOUNT_FOR_TOKEN
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithoutTreasuryAccountSignature)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenCreateTransaction()
                                                      .setTokenName("ffff")
                                                      .setTokenSymbol("F")
                                                      .setTreasuryAccountId(accountId)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE

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
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithoutAdminKeySignature)
{
  // Given
  std::shared_ptr<PrivateKey> adminKey;
  ASSERT_NO_THROW(adminKey = ED25519PrivateKey::generatePrivateKey());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenCreateTransaction()
                                                      .setTokenName("ffff")
                                                      .setTokenSymbol("F")
                                                      .setTreasuryAccountId(AccountId(2ULL))
                                                      .setAdminKey(adminKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateTokenWithCustomFees)
{
  // Given
  const std::vector<std::shared_ptr<CustomFee>> fees = {
    std::make_shared<CustomFixedFee>(CustomFixedFee().setAmount(10ULL).setFeeCollectorAccountId(AccountId(2ULL))),
    std::make_shared<CustomFractionalFee>(CustomFractionalFee()
                                            .setNumerator(1LL)
                                            .setDenominator(20LL)
                                            .setMinimumAmount(1ULL)
                                            .setMaximumAmount(10ULL)
                                            .setFeeCollectorAccountId(AccountId(2ULL)))
  };

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("ffff")
                                .setTokenSymbol("F")
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setCustomFees(fees)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithMoreThanTenCustomFees)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  const std::vector<std::shared_ptr<CustomFee>> fees(
    11, std::make_shared<CustomFixedFee>(CustomFixedFee().setAmount(10LL).setFeeCollectorAccountId(AccountId(2ULL))));

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenCreateTransaction()
                                                      .setTokenName("ffff")
                                                      .setTokenSymbol("F")
                                                      .setTreasuryAccountId(AccountId(2ULL))
                                                      .setAdminKey(operatorKey)
                                                      .setCustomFees(fees)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // CUSTOM_FEES_LIST_TOO_LONG
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateTokenWithListOfTenCustomFixedFees)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  const std::vector<std::shared_ptr<CustomFee>> fees(
    10, std::make_shared<CustomFixedFee>(CustomFixedFee().setAmount(10LL).setFeeCollectorAccountId(AccountId(2ULL))));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("ffff")
                                .setTokenSymbol("F")
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setAdminKey(operatorKey)
                                .setCustomFees(fees)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateTokenWithListOfTenCustomFractionalFees)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  const std::vector<std::shared_ptr<CustomFee>> fees(
    10,
    std::make_shared<CustomFractionalFee>(CustomFractionalFee()
                                            .setNumerator(1LL)
                                            .setDenominator(20LL)
                                            .setMinimumAmount(1ULL)
                                            .setMaximumAmount(10ULL)
                                            .setFeeCollectorAccountId(AccountId(2ULL))));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("ffff")
                                .setTokenSymbol("F")
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setAdminKey(operatorKey)
                                .setCustomFees(fees)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithCustomFractionalFeeWithHigherMaxThanMin)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When / Then
  EXPECT_THROW(
    const TransactionReceipt txReceipt =
      TokenCreateTransaction()
        .setTokenName("ffff")
        .setTokenSymbol("F")
        .setTreasuryAccountId(AccountId(2ULL))
        .setAdminKey(operatorKey)
        .setCustomFees({ std::make_shared<CustomFractionalFee>(CustomFractionalFee()
                                                                 .setNumerator(1LL)
                                                                 .setDenominator(3LL)
                                                                 .setMinimumAmount(3ULL)
                                                                 .setMaximumAmount(2ULL)
                                                                 .setFeeCollectorAccountId(AccountId(2ULL))) })
        .execute(getTestClient())
        .getReceipt(getTestClient()),
    ReceiptStatusException); // FRACTIONAL_FEE_MAX_AMOUNT_LESS_THAN_MIN_AMOUNT
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithInvalidFeeCollectorAccountId)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TokenCreateTransaction()
                   .setTokenName("ffff")
                   .setTokenSymbol("F")
                   .setTreasuryAccountId(AccountId(2ULL))
                   .setAdminKey(operatorKey)
                   .setCustomFees({ std::make_shared<CustomFixedFee>(CustomFixedFee().setAmount(1LL)) })
                   .execute(getTestClient())
                   .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_CUSTOM_FEE_COLLECTOR
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CannotCreateTokenWithNegativeCustomFee)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TokenCreateTransaction()
                   .setTokenName("ffff")
                   .setTokenSymbol("F")
                   .setTreasuryAccountId(AccountId(2ULL))
                   .setAdminKey(operatorKey)
                   .setCustomFees({ std::make_shared<CustomFixedFee>(
                     CustomFixedFee().setAmount(-1LL).setFeeCollectorAccountId(AccountId(2ULL))) })
                   .execute(getTestClient())
                   .getReceipt(getTestClient()),
               ReceiptStatusException); // CUSTOM_FEE_MUST_BE_POSITIVE
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateNft)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("ffff")
                                .setTokenSymbol("F")
                                .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setAdminKey(operatorKey)
                                .setFreezeKey(operatorKey)
                                .setWipeKey(operatorKey)
                                .setKycKey(operatorKey)
                                .setSupplyKey(operatorKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateNftWithRoyaltyFee)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(
    txReceipt =
      TokenCreateTransaction()
        .setTokenName("ffff")
        .setTokenSymbol("F")
        .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
        .setTreasuryAccountId(AccountId(2ULL))
        .setAdminKey(operatorKey)
        .setSupplyKey(operatorKey)
        .setCustomFees({ std::make_shared<CustomRoyaltyFee>(CustomRoyaltyFee()
                                                              .setNumerator(1LL)
                                                              .setDenominator(10LL)
                                                              .setFallbackFee(CustomFixedFee().setHbarAmount(Hbar(1LL)))
                                                              .setFeeCollectorAccountId(AccountId(2ULL))) })
        .execute(getTestClient())
        .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, AutoSetAutoRenewAccount)
{
  // Given / When
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // Then
  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  ASSERT_EQ(tokenInfo.mAutoRenewAccountId, AccountId(2ULL));
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, DoesNotAutoSetAutoRenewAccount)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey)
                                .setInitialBalance(Hbar(5LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setAutoRenewAccountId(accountId)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .freezeWith(&getTestClient())
                              .sign(accountKey)
                              .sign(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // Then
  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  ASSERT_EQ(tokenInfo.mAutoRenewAccountId, accountId);
}

//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateTokenWithDecimalAdjustmentForSupplyValues)
{
  // Given
  int decimals = 3;
  int64_t userInputInitialSupply = 1000;
  int64_t userInputMaxSupply = 10000;
  int64_t expectedInitialSupply = userInputInitialSupply * 1000;
  int64_t expectedMaxSupply = userInputMaxSupply * 1000;

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("DecimalTest")
                                .setTokenSymbol("DT")
                                .setDecimals(decimals)
                                .setInitialSupply(expectedInitialSupply)
                                .setMaxSupply(expectedMaxSupply)
                                .setSupplyType(TokenSupplyType::FINITE)
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setAdminKey(operatorKey)
                                .setSupplyKey(operatorKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  ASSERT_EQ(tokenInfo.mDecimals, decimals);
  ASSERT_EQ(tokenInfo.mTotalSupply, expectedInitialSupply);
  ASSERT_EQ(tokenInfo.mMaxSupply, expectedMaxSupply);

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}
//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateNftWithZeroDecimalsAndZeroInitialSupply)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("NFTTest")
                                .setTokenSymbol("NFT")
                                .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                                .setDecimals(0)
                                .setInitialSupply(0)
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setAdminKey(operatorKey)
                                .setSupplyKey(operatorKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  ASSERT_EQ(tokenInfo.mTokenType, TokenType::NON_FUNGIBLE_UNIQUE);
  ASSERT_EQ(tokenInfo.mDecimals, 0);
  ASSERT_EQ(tokenInfo.mTotalSupply, 0);

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}
//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateTokenWithDifferentDecimalPrecisionValues)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  std::vector<int> decimalValues = {0, 1, 2, 6, 8, 18};
  int64_t userInputSupply = 100;

  for (int decimals : decimalValues)
  {
    int64_t expectedSupply = userInputSupply;
    for (int i = 0; i < decimals; ++i)
    {
      expectedSupply *= 10;
    }

    TransactionReceipt txReceipt;
    EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                  .setTokenName("DecimalTest" + std::to_string(decimals))
                                  .setTokenSymbol("DT" + std::to_string(decimals))
                                  .setDecimals(decimals)
                                  .setInitialSupply(expectedSupply)
                                  .setTreasuryAccountId(AccountId(2ULL))
                                  .setAdminKey(operatorKey)
                                  .execute(getTestClient())
                                  .getReceipt(getTestClient()));

    TokenId tokenId;
    EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

    TokenInfo tokenInfo;
    EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
    ASSERT_EQ(tokenInfo.mDecimals, decimals);
    ASSERT_EQ(tokenInfo.mTotalSupply, expectedSupply);

    // Clean up
    ASSERT_NO_THROW(txReceipt =
                      TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
  }
}
//-----
TEST_F(TokenCreateTransactionIntegrationTests, CanCreateTokenWhenAutoRenewPeriodIsNull)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  // Calculate expiration time 90 days from now
  using namespace std::chrono;
  auto expirationTime = system_clock::now() + hours(24 * 90);

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("TEST")
                                .setTokenSymbol("TEST")
                                .setTokenType(TokenType::FUNGIBLE_COMMON)
                                .setSupplyType(TokenSupplyType::INFINITE)
                                .setAutoRenewAccountId(AccountId(2ULL))
                                .setInitialSupply(1)
                                .setMaxTransactionFee(Hbar(100LL))
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setExpirationTime(expirationTime)
                                .setDecimals(0)
                                .setAdminKey(operatorKey)
                                // autoRenewPeriod is intentionally NOT set
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  ASSERT_EQ(txReceipt.mStatus, Status::SUCCESS);

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  ASSERT_EQ(tokenInfo.mTokenName, "TEST");
  ASSERT_EQ(tokenInfo.mTokenSymbol, "TEST");
  ASSERT_EQ(tokenInfo.mTokenType, TokenType::FUNGIBLE_COMMON);
  ASSERT_EQ(tokenInfo.mSupplyType, TokenSupplyType::INFINITE);
  ASSERT_EQ(tokenInfo.mAutoRenewAccountId, AccountId(2ULL));

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}
