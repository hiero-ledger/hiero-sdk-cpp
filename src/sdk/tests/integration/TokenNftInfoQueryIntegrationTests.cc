// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenNftInfo.h"
#include "TokenNftInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenNftInfoQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenNftInfoQueryIntegrationTests, ExecuteTokenNftInfoQuery)
{
  // Given
  const std::vector<std::byte> metadata = { std::byte(0x01), std::byte(0x02), std::byte(0x03) };

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  std::vector<uint64_t> serialNumbers;
  ASSERT_NO_THROW(serialNumbers = TokenMintTransaction()
                                    .setTokenId(tokenId)
                                    .addMetadata(metadata)
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient())
                                    .mSerialNumbers);
  ASSERT_FALSE(serialNumbers.empty());

  const NftId nftId(tokenId, serialNumbers.at(0));

  // When
  TokenNftInfo tokenNftInfo;
  EXPECT_NO_THROW(tokenNftInfo = TokenNftInfoQuery().setNftId(nftId).execute(getTestClient()));

  // Then
  EXPECT_EQ(tokenNftInfo.mNftId, nftId);
  EXPECT_EQ(tokenNftInfo.mAccountId, AccountId(2ULL));
  EXPECT_EQ(tokenNftInfo.mMetadata, metadata);

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenNftInfoQueryIntegrationTests, CannotQueryWithInvalidNftId)
{
  // Given
  const std::vector<std::byte> metadata = { std::byte(0x01), std::byte(0x02), std::byte(0x03) };

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  std::vector<uint64_t> serialNumbers;
  ASSERT_NO_THROW(serialNumbers = TokenMintTransaction()
                                    .setTokenId(tokenId)
                                    .addMetadata(metadata)
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient())
                                    .mSerialNumbers);
  ASSERT_FALSE(serialNumbers.empty());

  const NftId nftId(tokenId, serialNumbers.at(0) + 1);

  // When / Then
  EXPECT_THROW(const TokenNftInfo tokenNftInfo = TokenNftInfoQuery().setNftId(nftId).execute(getTestClient()),
               PrecheckStatusException); // INVALID_NFT_ID

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}
