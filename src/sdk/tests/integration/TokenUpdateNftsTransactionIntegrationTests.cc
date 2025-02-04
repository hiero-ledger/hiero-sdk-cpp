// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "NftId.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenId.h"
#include "TokenMintTransaction.h"
#include "TokenNftInfo.h"
#include "TokenNftInfoQuery.h"
#include "TokenUpdateNftsTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

/**
 * @notice Integration-HIP-657
 * @url https://hips.hedera.com/hip/hip-657
 */
class TokenUpdateNftsTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::vector<std::byte> getTestMetadata() const { return mMetadata; }
  [[nodiscard]] inline const std::vector<std::vector<std::byte>> generateTestMetadataRecords(
    std::vector<std::byte> metadata,
    int count) const
  {
    return std::vector<std::vector<std::byte>>(count, metadata);
  }

private:
  const std::vector<std::byte> mMetadata = { std::byte(0xAA), std::byte(0xAB), std::byte(0xAC), std::byte(0xAD) };
};

//-----
TEST_F(TokenUpdateNftsTransactionIntegrationTests, UpdateNFTMetadata)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  std::shared_ptr<PrivateKey> metadataKey;
  ASSERT_NO_THROW(metadataKey = ED25519PrivateKey::generatePrivateKey());
  const int nftCount = 4;
  const std::vector<std::vector<std::byte>> initialMetadataRecords =
    generateTestMetadataRecords(getTestMetadata(), nftCount);
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };
  const std::vector<std::vector<std::byte>> updatedMetadataRecords =
    generateTestMetadataRecords(updatedMetadataRecord, nftCount / 2);
  const std::vector<std::vector<std::byte>> notUpdatedMetadataRecords =
    generateTestMetadataRecords(getTestMetadata(), nftCount / 2);

  // create a token with metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .setMetadataKey(metadataKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // mint tokens
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId)
                                .setMetadata(initialMetadataRecords)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // check that metadata was set correctly
  std::vector<uint64_t> serials = txReceipt.mSerialNumbers;

  std::vector<std::vector<std::byte>> metadataRecordsAfterMint;

  for (uint64_t serial : serials)
  {
    NftId nftId(tokenId, serial);
    TokenNftInfo nftInfo;
    ASSERT_NO_THROW(nftInfo = TokenNftInfoQuery().setNftId(nftId).execute(getTestClient()););
    metadataRecordsAfterMint.push_back(nftInfo.mMetadata);
  }

  EXPECT_EQ(initialMetadataRecords, metadataRecordsAfterMint);

  // When

  // update metadata of the first two minted NFTs
  int middleIndex = serials.size() / 2;
  std::vector<uint64_t> serialsToUpdate(serials.begin(), serials.begin() + middleIndex);

  ASSERT_NO_THROW(txReceipt = TokenUpdateNftsTransaction()
                                .setTokenId(tokenId)
                                .setSerials(serialsToUpdate)
                                .setMetadata(updatedMetadataRecord)
                                .freezeWith(&getTestClient())
                                .sign(metadataKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()););

  // Then

  // check updated NFTs' metadata
  std::vector<std::vector<std::byte>> metadataRecordsAfterUpdate;

  for (uint64_t serial : serialsToUpdate)
  {
    NftId nftId(tokenId, serial);
    TokenNftInfo nftInfo;
    ASSERT_NO_THROW(nftInfo = TokenNftInfoQuery().setNftId(nftId).execute(getTestClient()););
    metadataRecordsAfterUpdate.push_back(nftInfo.mMetadata);
  }

  EXPECT_EQ(updatedMetadataRecords, metadataRecordsAfterUpdate);

  // check that remaining NFTs were not updated
  std::vector<uint64_t> notUpdatedSerials(serials.begin() + middleIndex, serials.end());

  metadataRecordsAfterUpdate.clear();
  for (uint64_t serial : notUpdatedSerials)
  {
    NftId nftId(tokenId, serial);
    TokenNftInfo nftInfo;
    ASSERT_NO_THROW(nftInfo = TokenNftInfoQuery().setNftId(nftId).execute(getTestClient()););
    metadataRecordsAfterUpdate.push_back(nftInfo.mMetadata);
  }

  EXPECT_EQ(notUpdatedMetadataRecords, metadataRecordsAfterUpdate);
}

//-----
// Would need to be looked at by Hiero Services teams. It Is disabled as it is no longer failing
TEST_F(TokenUpdateNftsTransactionIntegrationTests, DISABLED_CannotUpdateNFTMetadataWhenNotSignedWithMetadataKey)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  std::shared_ptr<PrivateKey> metadataKey;
  ASSERT_NO_THROW(metadataKey = ED25519PrivateKey::generatePrivateKey());
  const int nftCount = 4;
  const std::vector<std::vector<std::byte>> initialMetadataRecords =
    generateTestMetadataRecords(getTestMetadata(), nftCount);
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };

  // create a token with metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .setMetadataKey(metadataKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // mint tokens
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId)
                                .setMetadata(initialMetadataRecords)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  std::vector<uint64_t> serials = txReceipt.mSerialNumbers;

  // When / Then
  ASSERT_THROW(txReceipt = TokenUpdateNftsTransaction()
                             .setTokenId(tokenId)
                             .setSerials(serials)
                             .setMetadata(updatedMetadataRecord)
                             .freezeWith(&getTestClient())
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE
}
