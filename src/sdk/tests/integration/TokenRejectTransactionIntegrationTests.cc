// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenPauseTransaction.h"
#include "TokenRejectFlow.h"
#include "TokenRejectTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenRejectTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  TokenId createFt(const std::shared_ptr<PrivateKey>& operatorKey, bool pause = false)
  {
    return TokenCreateTransaction()
      .setTokenName("ffff")
      .setTokenSymbol("F")
      .setTokenType(TokenType::FUNGIBLE_COMMON)
      .setInitialSupply(100000)
      .setTreasuryAccountId(AccountId(2ULL))
      .setAdminKey(operatorKey)
      .setFreezeKey(operatorKey)
      .setWipeKey(operatorKey)
      .setPauseKey(pause ? operatorKey : nullptr)
      .setSupplyKey(operatorKey)
      .setFeeScheduleKey(operatorKey)
      .execute(getTestClient())
      .getReceipt(getTestClient())
      .mTokenId.value();
  }

  TokenId createNft(const std::shared_ptr<PrivateKey>& operatorKey, bool pause = false)
  {
    return TokenCreateTransaction()
      .setTokenName("ffff")
      .setTokenSymbol("F")
      .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
      .setTreasuryAccountId(AccountId(2ULL))
      .setSupplyType(TokenSupplyType::FINITE)
      .setMaxSupply(10)
      .setAdminKey(operatorKey)
      .setFreezeKey(operatorKey)
      .setWipeKey(operatorKey)
      .setPauseKey(pause ? operatorKey : nullptr)
      .setSupplyKey(operatorKey)
      .setFeeScheduleKey(operatorKey)
      .execute(getTestClient())
      .getReceipt(getTestClient())
      .mTokenId.value();
  }

  AccountId createAccount(const std::shared_ptr<PrivateKey>& accountKey, bool treasury = false)
  {
    AccountCreateTransaction tx = AccountCreateTransaction()
                                    .setKeyWithoutAlias(accountKey)
                                    .setInitialBalance(Hbar(5))
                                    .setMaxAutomaticTokenAssociations(1000);

    if (treasury)
    {
      tx.setInitialBalance(Hbar(0))
        .setReceiverSignatureRequired(treasury)
        .freezeWith(&getTestClient())
        .sign(accountKey);
    }

    return tx.execute(getTestClient()).getReceipt(getTestClient()).mAccountId.value();
  }

private:
};

//-----
TEST_F(TokenRejectTransactionIntegrationTests, CanExecuteForFT)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create two fts
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey));
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createFt(operatorKey));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // When
  // Transfer fts to the receiver
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId1, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId1, receiver, 10)
                                .addTokenTransfer(tokenId2, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId2, receiver, 10)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  // Reject the tokens
  EXPECT_NO_THROW(txReceipt = TokenRejectTransaction()
                                .setOwner(receiver)
                                .setFts({ tokenId1, tokenId2 })
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify the balance of the receiver is 0
  AccountBalance balance;
  EXPECT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiver).execute(getTestClient()));

  EXPECT_EQ(0, balance.mTokens[tokenId1]);
  EXPECT_EQ(0, balance.mTokens[tokenId2]);

  // Verify the tokens are transferred back to the treasury
  EXPECT_NO_THROW(
    balance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));

  EXPECT_EQ(100000, balance.mTokens[tokenId1]);
  EXPECT_EQ(100000, balance.mTokens[tokenId2]);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, CanExecuteForNFT)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create two nfts
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createNft(operatorKey));
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createNft(operatorKey));

  // Mint the nfts
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId1)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId2)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // When
  // Transfer nfts to the receiver
  ASSERT_NO_THROW(txReceipt =
                    TransferTransaction()
                      .addNftTransfer(tokenId1.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .addNftTransfer(tokenId2.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Then
  // Reject the nfts
  EXPECT_NO_THROW(txReceipt = TokenRejectTransaction()
                                .setOwner(receiver)
                                .setNfts({ tokenId1.nft(1), tokenId2.nft(1) })
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify the balance of the receiver is 0
  AccountBalance balance;
  EXPECT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiver).execute(getTestClient()));

  EXPECT_EQ(0, balance.mTokens[tokenId1]);
  EXPECT_EQ(0, balance.mTokens[tokenId2]);

  // Verify the tokens are transferred back to the treasury
  EXPECT_NO_THROW(
    balance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));

  EXPECT_EQ(1, balance.mTokens[tokenId1]);
  EXPECT_EQ(1, balance.mTokens[tokenId2]);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, CanExecuteForFTAndNFT)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create two fts
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey));
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createFt(operatorKey));

  // Create two nfts
  TokenId tokenId3;
  ASSERT_NO_THROW(tokenId3 = createNft(operatorKey));
  TokenId tokenId4;
  ASSERT_NO_THROW(tokenId4 = createNft(operatorKey));

  // Mint the nfts
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId3)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId4)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When
  // Transfer fts to the receiver
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId1, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId1, receiver, 10)
                                .addTokenTransfer(tokenId2, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId2, receiver, 10)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Transfer nfts to the receiver
  ASSERT_NO_THROW(txReceipt =
                    TransferTransaction()
                      .addNftTransfer(tokenId3.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .addNftTransfer(tokenId4.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Then
  // Reject the fts and nfts
  EXPECT_NO_THROW(txReceipt = TokenRejectTransaction()
                                .setOwner(receiver)
                                .setNfts({ tokenId3.nft(1), tokenId4.nft(1) })
                                .setFts({ tokenId1, tokenId2 })
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify the ft balance of the receiver is 0
  AccountBalance balance;
  EXPECT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiver).execute(getTestClient()));

  EXPECT_EQ(0, balance.mTokens[tokenId1]);
  EXPECT_EQ(0, balance.mTokens[tokenId2]);

  // Verify the tokens are transferred back to the treasury
  EXPECT_NO_THROW(
    balance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));

  EXPECT_EQ(100000, balance.mTokens[tokenId1]);
  EXPECT_EQ(100000, balance.mTokens[tokenId2]);

  // Verify the ft balance of the receiver is 0
  EXPECT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiver).execute(getTestClient()));

  EXPECT_EQ(0, balance.mTokens[tokenId3]);
  EXPECT_EQ(0, balance.mTokens[tokenId4]);

  // Verify the tokens are transferred back to the treasury
  EXPECT_NO_THROW(
    balance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));

  EXPECT_EQ(1, balance.mTokens[tokenId3]);
  EXPECT_EQ(1, balance.mTokens[tokenId4]);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, ReceiverSigRequired)
{
  // Given
  // Create treasury account with auto associations
  AccountId treasury;
  std::shared_ptr<PrivateKey> treasuryKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(treasury = createAccount(treasuryKey, true));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create ft for
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("Test Fungible Token")
                              .setTokenSymbol("TFT")
                              .setTokenMemo("I was created for integration tests")
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(treasury)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFreezeKey(getTestClient().getOperatorPublicKey())
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .setMetadataKey(getTestClient().getOperatorPublicKey())
                              .freezeWith(&getTestClient())
                              .sign(treasuryKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When
  // Transfer fts to the receiver
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId, treasury, -10)
                                .addTokenTransfer(tokenId, receiver, 10)
                                .freezeWith(&getTestClient())
                                .sign(treasuryKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  // Reject the ft
  EXPECT_NO_THROW(txReceipt = TokenRejectTransaction()
                                .setOwner(receiver)
                                .setFts({ tokenId })
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, TokenFrozen)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create an ft
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey));

  // Create an nft
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createNft(operatorKey));

  // Mint the nfts
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId2)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When
  // Transfer fts to the receiver
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId1, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId1, receiver, 10)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Transfer nfts to the receiver
  ASSERT_NO_THROW(txReceipt =
                    TransferTransaction()
                      .addNftTransfer(tokenId2.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Freeze the ft
  ASSERT_NO_THROW(txReceipt = TokenFreezeTransaction()
                                .setTokenId(tokenId1)
                                .setAccountId(receiver)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Reject the ft
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setFts({ tokenId1 })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);

  // Freeze the nft
  ASSERT_NO_THROW(txReceipt = TokenFreezeTransaction()
                                .setTokenId(tokenId2)
                                .setAccountId(receiver)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  // Reject the nfts
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setNfts({ tokenId2.nft(1) })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, TokenPaused)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create an ft
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey, true));

  // Create an nft
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createNft(operatorKey, true));

  // Mint the nfts
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId2)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When
  // Transfer fts to the receiver
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId1, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId1, receiver, 10)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Transfer nfts to the receiver
  ASSERT_NO_THROW(txReceipt =
                    TransferTransaction()
                      .addNftTransfer(tokenId2.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Pause the ft
  ASSERT_NO_THROW(txReceipt =
                    TokenPauseTransaction().setTokenId(tokenId1).execute(getTestClient()).getReceipt(getTestClient()));

  // Pause the nft
  ASSERT_NO_THROW(txReceipt =
                    TokenPauseTransaction().setTokenId(tokenId2).execute(getTestClient()).getReceipt(getTestClient()));

  // Then
  // Reject the ft
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setFts({ tokenId1 })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);
  // Reject the nft
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setNfts({ tokenId2.nft(1) })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, RemovesAllowance)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create spender account with auto associations
  AccountId spender;
  std::shared_ptr<PrivateKey> spenderKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(spender = createAccount(spenderKey));

  // Create an nft
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createNft(operatorKey, true));

  // Mint the nft
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId)
                                .setMetadata({ { std::byte(0x01) }, { std::byte(0x02) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When
  // Transfer nft to the receiver
  ASSERT_NO_THROW(txReceipt =
                    TransferTransaction()
                      .addNftTransfer(tokenId.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .addNftTransfer(tokenId.nft(2), getTestClient().getOperatorAccountId().value(), receiver)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // approve allowance to the spender
  ASSERT_NO_THROW(txReceipt = AccountAllowanceApproveTransaction()
                                .approveTokenNftAllowance(tokenId.nft(1), receiver, spender)
                                .approveTokenNftAllowance(tokenId.nft(2), receiver, spender)
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify that the spender has allowance
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addApprovedNftTransfer(tokenId.nft(1), receiver, spender)
                                .setTransactionId(TransactionId::generate(spender))
                                .freezeWith(&getTestClient())
                                .sign(spenderKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  // Reject nfts
  EXPECT_NO_THROW(txReceipt = TokenRejectTransaction()
                                .setOwner(receiver)
                                .setNfts({ tokenId.nft(2) })
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify no further allowance
  ASSERT_THROW(txReceipt = TransferTransaction()
                             .addApprovedNftTransfer(tokenId.nft(2), receiver, spender)
                             .setTransactionId(TransactionId::generate(spender))
                             .freezeWith(&getTestClient())
                             .sign(spenderKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, FailsWhenRejectingNFTWithTokenID)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create spender account with auto associations
  AccountId spender;
  std::shared_ptr<PrivateKey> spenderKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(spender = createAccount(spenderKey));

  // Create an nft
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createNft(operatorKey, true));

  // Mint the nft
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId)
                                .setMetadata({ { std::byte(0x01) }, { std::byte(0x02) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  // When
  // Transfer nft to the receiver
  ASSERT_NO_THROW(txReceipt =
                    TransferTransaction()
                      .addNftTransfer(tokenId.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .addNftTransfer(tokenId.nft(2), getTestClient().getOperatorAccountId().value(), receiver)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Then
  // Reject nft from fts collection
  ASSERT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setFts({ tokenId })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, FailsWithTokenReferenceRepeated)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create an ft
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey));

  // Create an nft
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createNft(operatorKey));

  // Mint the nfts
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId2)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  // When
  // Transfer fts to the receiver
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId1, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId1, receiver, 10)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Transfer nfts to the receiver
  ASSERT_NO_THROW(txReceipt =
                    TransferTransaction()
                      .addNftTransfer(tokenId2.nft(1), getTestClient().getOperatorAccountId().value(), receiver)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Then
  // Reject the ft repeated
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setFts({ tokenId1, tokenId1 })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               PrecheckStatusException); // TOKEN_REFERENCE_REPEATED

  // Reject the nft repeated
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setNfts({ tokenId2.nft(1), tokenId2.nft(1) })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               PrecheckStatusException); // TOKEN_REFERENCE_REPEATED
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, FailsWhenOwnerHasNoBalance)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create an ft
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey));

  // Create an nft
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createNft(operatorKey));

  // Mint the nfts
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId2)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  // When
  // Associate the receiver
  EXPECT_NO_THROW(txReceipt = TokenAssociateTransaction()
                                .setAccountId(receiver)
                                .setTokenIds({ tokenId1, tokenId2 })
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  // Reject the ft with invalid balance
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setFts({ tokenId1 })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // INSUFFICIENT_BALANCE

  // Reject the nft with invalid owner
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setNfts({ tokenId2.nft(1) })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_OWNER_ID
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, FailsTreasuryRejects)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create an ft
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey));

  // Create an nft
  TokenId tokenId2;
  ASSERT_NO_THROW(tokenId2 = createNft(operatorKey));

  // Mint the nfts
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(tokenId2)
                                .setMetadata({ { std::byte(0xAB) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // When / Then
  // Reject the ft and nft for treasury
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(AccountId(2ULL))
                             .setFts({ tokenId1 })
                             .setNfts({ tokenId2.nft(1) })
                             .freezeWith(&getTestClient())
                             .sign(operatorKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // ACCOUNT_IS_TREASURY
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, FailsWithInvalidToken)
{
  // Given
  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // When / Then
  // Reject invalid ft or nft
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setFts({ TokenId(2ULL) })
                             .setNfts({ TokenId(2ULL).nft(1) })
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, FailsWhenNoFTsOrNFTsProvided)
{
  // Given
  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // When / Then
  // Reject with empty fts or nfts
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .freezeWith(&getTestClient())
                             .sign(receiverKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               PrecheckStatusException); // EMPTY_TOKEN_REFERENCE_LIST
}

//-----
TEST_F(TokenRejectTransactionIntegrationTests, FailsWithInvalidSignature)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // Create an ft
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));

  // When
  // Transfer fts to the receiver
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId, receiver, 10)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  // Then
  // Reject ft with invalid signature
  ASSERT_THROW(txReceipt = TokenRejectTransaction()
                             .setOwner(receiver)
                             .setFts({ tokenId })
                             .freezeWith(&getTestClient())
                             .sign(ED25519PrivateKey::generatePrivateKey())
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);
}

TEST_F(TokenRejectTransactionIntegrationTests, CanExecuteFlowForFT)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create two fts
  TokenId tokenId1;
  ASSERT_NO_THROW(tokenId1 = createFt(operatorKey));

  // Create receiver account with auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // When
  // manually associate ft
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenAssociateTransaction()
                                .setAccountId(receiver)
                                .setTokenIds({ tokenId1 })
                                .freezeWith(&getTestClient())
                                .sign(receiverKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Transfer fts to the receiver
  ASSERT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId1, getTestClient().getOperatorAccountId().value(), -10)
                                .addTokenTransfer(tokenId1, receiver, 10)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  // Initialize a TokenRejectFlow
  TokenRejectFlow tokenRejectFlow;
  tokenRejectFlow.tokenRejectTransaction.setOwner(receiver).setFts({ tokenId1 });
  tokenRejectFlow.tokenDissociateTransaction.setAccountId(receiver);
  tokenRejectFlow.setReceiverPrivateKey(receiverKey);

  // execute the token reject flow
  EXPECT_NO_THROW(txReceipt = tokenRejectFlow.execute(getTestClient()).getReceipt(getTestClient()););

  // Verify the balance of the receiver is 0
  AccountBalance balance;
  EXPECT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiver).execute(getTestClient()));

  EXPECT_EQ(0, balance.mTokens[tokenId1]);

  // Verify the tokens are transferred back to the treasury
  EXPECT_NO_THROW(
    balance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));

  EXPECT_EQ(100000, balance.mTokens[tokenId1]);
}