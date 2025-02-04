// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "CustomFixedFee.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenMintTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenInfoQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenInfoQueryIntegrationTests, ExecuteTokenInfoQuery)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> adminKey;
  std::shared_ptr<PrivateKey> freezeKey;
  std::shared_ptr<PrivateKey> wipeKey;
  std::shared_ptr<PrivateKey> kycKey;
  std::shared_ptr<PrivateKey> supplyKey;
  std::shared_ptr<PrivateKey> feeScheduleKey;
  std::shared_ptr<PrivateKey> pauseKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(adminKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(freezeKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(wipeKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(kycKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(supplyKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(feeScheduleKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(pauseKey = ED25519PrivateKey::generatePrivateKey());

  const std::string tokenName = "ffff";
  const std::string tokenSymbol = "F";
  const uint32_t decimals = 3U;
  const uint64_t supply = 100000ULL;
  const std::string memo = "test memo";
  const std::vector<std::shared_ptr<CustomFee>> customFees = { std::make_shared<CustomFixedFee>(
    CustomFixedFee().setAmount(10LL).setFeeCollectorAccountId(AccountId(2ULL))) };

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName(tokenName)
                              .setTokenSymbol(tokenSymbol)
                              .setDecimals(decimals)
                              .setInitialSupply(supply)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(adminKey)
                              .setFreezeKey(freezeKey)
                              .setWipeKey(wipeKey)
                              .setKycKey(kycKey)
                              .setSupplyKey(supplyKey)
                              .setTokenMemo(memo)
                              .setFeeScheduleKey(feeScheduleKey)
                              .setCustomFees(customFees)
                              .setPauseKey(pauseKey)
                              .freezeWith(&getTestClient())
                              .sign(adminKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When
  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  // Then
  EXPECT_EQ(tokenInfo.mTokenId, tokenId);
  EXPECT_EQ(tokenInfo.mTokenName, tokenName);
  EXPECT_EQ(tokenInfo.mTokenSymbol, tokenSymbol);
  EXPECT_EQ(tokenInfo.mDecimals, decimals);
  EXPECT_EQ(tokenInfo.mTotalSupply, supply);
  EXPECT_EQ(tokenInfo.mTreasuryAccountId, AccountId(2ULL));
  ASSERT_NE(tokenInfo.mAdminKey, nullptr);
  EXPECT_EQ(tokenInfo.mAdminKey->toBytes(), adminKey->getPublicKey()->toBytes());
  ASSERT_NE(tokenInfo.mKycKey, nullptr);
  EXPECT_EQ(tokenInfo.mKycKey->toBytes(), kycKey->getPublicKey()->toBytes());
  ASSERT_NE(tokenInfo.mFreezeKey, nullptr);
  EXPECT_EQ(tokenInfo.mFreezeKey->toBytes(), freezeKey->getPublicKey()->toBytes());
  ASSERT_NE(tokenInfo.mWipeKey, nullptr);
  EXPECT_EQ(tokenInfo.mWipeKey->toBytes(), wipeKey->getPublicKey()->toBytes());
  ASSERT_NE(tokenInfo.mSupplyKey, nullptr);
  EXPECT_EQ(tokenInfo.mSupplyKey->toBytes(), supplyKey->getPublicKey()->toBytes());
  EXPECT_FALSE(tokenInfo.mDefaultFreezeStatus.value());
  EXPECT_FALSE(tokenInfo.mDefaultKycStatus.value());
  EXPECT_FALSE(tokenInfo.mIsDeleted);
  EXPECT_EQ(tokenInfo.mTokenMemo, memo);
  EXPECT_EQ(tokenInfo.mTokenType, TokenType::FUNGIBLE_COMMON);
  EXPECT_EQ(tokenInfo.mSupplyType, TokenSupplyType::INFINITE);
  ASSERT_NE(tokenInfo.mFeeScheduleKey, nullptr);
  EXPECT_EQ(tokenInfo.mFeeScheduleKey->toBytes(), feeScheduleKey->getPublicKey()->toBytes());
  EXPECT_EQ(tokenInfo.mCustomFees.size(), customFees.size());
  ASSERT_NE(tokenInfo.mPauseKey, nullptr);
  EXPECT_EQ(tokenInfo.mPauseKey->toBytes(), pauseKey->getPublicKey()->toBytes());
  EXPECT_FALSE(tokenInfo.mPauseStatus.value());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TokenDeleteTransaction()
                                                         .setTokenId(tokenId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(adminKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
}

//-----
TEST_F(TokenInfoQueryIntegrationTests, CanQueryTokenWithMinimalProperties)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  const std::string tokenName = "ffff";
  const std::string tokenSymbol = "F";

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName(tokenName)
                              .setTokenSymbol(tokenSymbol)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When
  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  // Then
  EXPECT_EQ(tokenInfo.mTokenId, tokenId);
  EXPECT_EQ(tokenInfo.mTokenName, tokenName);
  EXPECT_EQ(tokenInfo.mTokenSymbol, tokenSymbol);
  EXPECT_EQ(tokenInfo.mDecimals, 0U);
  EXPECT_EQ(tokenInfo.mTotalSupply, 0U);
  EXPECT_EQ(tokenInfo.mTreasuryAccountId, AccountId(2ULL));
  EXPECT_EQ(tokenInfo.mAdminKey, nullptr);
  EXPECT_EQ(tokenInfo.mKycKey, nullptr);
  EXPECT_EQ(tokenInfo.mFreezeKey, nullptr);
  EXPECT_EQ(tokenInfo.mWipeKey, nullptr);
  EXPECT_EQ(tokenInfo.mSupplyKey, nullptr);
  EXPECT_FALSE(tokenInfo.mDefaultFreezeStatus.has_value());
  EXPECT_FALSE(tokenInfo.mDefaultKycStatus.has_value());
  EXPECT_FALSE(tokenInfo.mIsDeleted);
  EXPECT_EQ(tokenInfo.mTokenType, TokenType::FUNGIBLE_COMMON);
  EXPECT_EQ(tokenInfo.mSupplyType, TokenSupplyType::INFINITE);
  EXPECT_EQ(tokenInfo.mFeeScheduleKey, nullptr);
  EXPECT_TRUE(tokenInfo.mCustomFees.empty());
  EXPECT_EQ(tokenInfo.mPauseKey, nullptr);
  EXPECT_FALSE(tokenInfo.mPauseStatus.has_value());
}

//-----
TEST_F(TokenInfoQueryIntegrationTests, CanQueryNft)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  const std::string tokenName = "ffff";
  const std::string tokenSymbol = "F";
  const uint64_t maxSupply = 5000ULL;
  const uint64_t supply = 10ULL;

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName(tokenName)
                              .setTokenSymbol(tokenSymbol)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setSupplyType(TokenSupplyType::FINITE)
                              .setMaxSupply(maxSupply)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId)
                                .setMetadata(std::vector<std::vector<std::byte>>(supply, { std::byte(0x01) }))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When
  TokenInfo tokenInfo;
  EXPECT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  // Then
  EXPECT_EQ(tokenInfo.mTokenId, tokenId);
  EXPECT_EQ(tokenInfo.mTokenName, tokenName);
  EXPECT_EQ(tokenInfo.mTokenSymbol, tokenSymbol);
  EXPECT_EQ(tokenInfo.mDecimals, 0U);
  EXPECT_EQ(tokenInfo.mTotalSupply, supply);
  EXPECT_EQ(tokenInfo.mTreasuryAccountId, AccountId(2ULL));
  EXPECT_NE(tokenInfo.mAdminKey, nullptr);
  EXPECT_EQ(tokenInfo.mKycKey, nullptr);
  EXPECT_EQ(tokenInfo.mFreezeKey, nullptr);
  EXPECT_EQ(tokenInfo.mWipeKey, nullptr);
  EXPECT_NE(tokenInfo.mSupplyKey, nullptr);
  EXPECT_FALSE(tokenInfo.mDefaultFreezeStatus.has_value());
  EXPECT_FALSE(tokenInfo.mDefaultKycStatus.has_value());
  EXPECT_FALSE(tokenInfo.mIsDeleted);
  EXPECT_EQ(tokenInfo.mTokenType, TokenType::NON_FUNGIBLE_UNIQUE);
  EXPECT_EQ(tokenInfo.mSupplyType, TokenSupplyType::FINITE);
  EXPECT_EQ(tokenInfo.mMaxSupply, maxSupply);
  EXPECT_EQ(tokenInfo.mFeeScheduleKey, nullptr);
  EXPECT_TRUE(tokenInfo.mCustomFees.empty());
  EXPECT_EQ(tokenInfo.mPauseKey, nullptr);
  EXPECT_FALSE(tokenInfo.mPauseStatus.has_value());
}
