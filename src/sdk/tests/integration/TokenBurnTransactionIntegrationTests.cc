// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenAssociateTransaction.h"
#include "TokenBurnTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenMintTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenBurnTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TokenBurnTransactionIntegrationTests, ExecuteTokenBurnTransaction)
{
  // Given
  const uint64_t initialSupply = 100000ULL;
  const uint64_t burnAmount = 10ULL;

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(initialSupply)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenBurnTransaction()
                                .setTokenId(tokenId)
                                .setAmount(burnAmount)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  EXPECT_EQ(txReceipt.mNewTotalSupply, initialSupply - burnAmount);

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenBurnTransactionIntegrationTests, CannotBurnTokensWhenTokenIdIsNotSet)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TokenBurnTransaction().setAmount(10ULL).execute(getTestClient()).getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_TOKEN_ID
}

//-----
TEST_F(TokenBurnTransactionIntegrationTests, CanBurnTokensWhenAmountIsNotSet)
{
  // Given
  const uint64_t initialSupply = 100000ULL;

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(initialSupply)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt =
                    TokenBurnTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));

  // Then
  EXPECT_EQ(txReceipt.mNewTotalSupply, initialSupply);

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenBurnTransactionIntegrationTests, CannotBurnTokensIfSupplyKeyDoesNotSign)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> supplyKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(supplyKey = ED25519PrivateKey::generatePrivateKey());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000ULL)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(supplyKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When / Then
  EXPECT_THROW(
    const TransactionReceipt txReceipt =
      TokenBurnTransaction().setTokenId(tokenId).setAmount(10ULL).execute(getTestClient()).getReceipt(getTestClient()),
    ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenBurnTransactionIntegrationTests, CanBurnNfts)
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
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(
    txReceipt =
      TokenMintTransaction()
        .setTokenId(tokenId)
        .setMetadata(
          { { std::byte(0x01) }, { std::byte(0x02) }, { std::byte(0x03) }, { std::byte(0x04) }, { std::byte(0x05) } })
        .execute(getTestClient())
        .getReceipt(getTestClient()));

  // When
  EXPECT_NO_THROW(txReceipt = TokenBurnTransaction()
                                .setTokenId(tokenId)
                                .setSerialNumbers(txReceipt.mSerialNumbers)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  EXPECT_EQ(txReceipt.mNewTotalSupply, 0);

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenBurnTransactionIntegrationTests, CannotBurnNftIfNftIsNotOwnedByTreasury)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> accountKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

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
                                    .setMetadata({ { std::byte(0x01) } })
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient())
                                    .mSerialNumbers);
  ASSERT_FALSE(serialNumbers.empty());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(accountKey)
                                .setInitialBalance(Hbar(1LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenAssociateTransaction()
                                .setTokenIds({ tokenId })
                                .setAccountId(accountId)
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  const NftId nftId(tokenId, serialNumbers.at(0));
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addNftTransfer(nftId, AccountId(2ULL), accountId)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When / Then
  EXPECT_THROW(txReceipt = TokenBurnTransaction()
                             .setTokenId(tokenId)
                             .setSerialNumbers(serialNumbers)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // TREASURY_MUST_OWN_BURNED_NFT

  // Clean up
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addNftTransfer(nftId, accountId, AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setTransferAccountId(AccountId(2ULL))
                                .setDeleteAccountId(accountId)
                                .freezeWith(&getTestClient())
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}
