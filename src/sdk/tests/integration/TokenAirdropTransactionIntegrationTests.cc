// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "TokenAirdropTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenPauseTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenAirdropTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  TokenId createFt(const std::shared_ptr<PrivateKey>& operatorKey, bool pause = false)
  {
    return TokenCreateTransaction()
      .setTokenName("ffff")
      .setTokenSymbol("F")
      .setTokenType(TokenType::FUNGIBLE_COMMON)
      .setInitialSupply(100)
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

  AccountId createAccount(const std::shared_ptr<PrivateKey>& accountKey, bool treasury = false, int assoc = 1000)
  {
    AccountCreateTransaction tx =
      AccountCreateTransaction().setKey(accountKey).setInitialBalance(Hbar(5)).setMaxAutomaticTokenAssociations(assoc);

    if (treasury)
    {
      tx.setInitialBalance(Hbar(0))
        .setReceiverSignatureRequired(treasury)
        .freezeWith(&getTestClient())
        .sign(accountKey);
    }

    return tx.execute(getTestClient()).getReceipt(getTestClient()).mAccountId.value();
  }

  AccountId createReceiverAccountWithSig(const std::shared_ptr<PrivateKey>& receiverKey)
  {
    return AccountCreateTransaction()
      .setKey(receiverKey)
      .setInitialBalance(Hbar(1))           // Small initial balance
      .setReceiverSignatureRequired(true)   // Require signature for token transfers
      .setMaxAutomaticTokenAssociations(-1) // Unlimited auto-associations
      .freezeWith(&getTestClient())         // Freeze the transaction for signing
      .sign(receiverKey)                    // Sign with the receiver's private key
      .execute(getTestClient())
      .getReceipt(getTestClient())
      .mAccountId.value();
  }

  void approveTokenAllowance(const TokenId& tokenId,
                             const AccountId& sender,
                             const AccountId& spender,
                             int64_t amount,
                             const std::shared_ptr<PrivateKey>& senderKey)
  {
    ASSERT_NO_THROW(
      AccountAllowanceApproveTransaction()
        .approveTokenAllowance(tokenId, sender, spender, amount) // Approve spender to use amount from sender
        .freezeWith(&getTestClient())                            // Freeze the transaction for signing
        .sign(senderKey)                                         // Sign with sender key
        .execute(getTestClient())
        .getReceipt(getTestClient()));
  }

  void transferToken(const TokenId& tokenId, const AccountId& sender, const AccountId& receiver, int64_t amount)
  {
    ASSERT_NO_THROW(TransferTransaction()
                      .addTokenTransfer(tokenId, sender, -amount)  // Deduct from sender
                      .addTokenTransfer(tokenId, receiver, amount) // Add to receiver
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));
  }
};

TEST_F(TokenAirdropTransactionIntegrationTests, CanAirdropAssociatedTokens)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create fungible and NFT tokens
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey, 3));
  TokenId nftId = createNft(operatorKey);
  ASSERT_NO_THROW(nftId = createNft(operatorKey));

  // Mint some NFTs
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(nftId)
                                .setMetadata({ { std::byte(10) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  std::vector<uint64_t> nftSerials = txReceipt.mSerialNumbers;

  // Create receiver account
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey));

  // When
  // Airdrop the tokens
  ASSERT_NO_THROW(
    txReceipt = TokenAirdropTransaction()
                  .addNftTransfer(nftId.nft(nftSerials[0]), getTestClient().getOperatorAccountId().value(), receiver)
                  .addTokenTransfer(tokenId, receiver, 100) // Assuming amount is 100
                  .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100)
                  .execute(getTestClient())
                  .getReceipt(getTestClient()));

  // Then
  // Verify the receiver holds the tokens
  AccountBalance receiverAccountBalance;
  ASSERT_NO_THROW(receiverAccountBalance = AccountBalanceQuery().setAccountId(receiver).execute(getTestClient()));
  ASSERT_EQ(100, receiverAccountBalance.mTokens[tokenId]);
  ASSERT_EQ(1, receiverAccountBalance.mTokens[nftId]);

  // Verify the operator does not hold the tokens
  AccountBalance operatorBalance;
  ASSERT_NO_THROW(
    operatorBalance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));
  ASSERT_EQ(0, operatorBalance.mTokens[tokenId]); // Assuming initial balance was 100
  ASSERT_EQ(0, operatorBalance.mTokens[nftId]);   // Assuming initial count was 2
}

TEST_F(TokenAirdropTransactionIntegrationTests, CanAirdropNonAssociatedTokens)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create FT and NFT tokens
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));
  TokenId nftId;
  ASSERT_NO_THROW(nftId = createNft(operatorKey));

  // Mint some NFTs
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(nftId)
                                .setMetadata({ { std::byte(10) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  std::vector<uint64_t> nftSerials = txReceipt.mSerialNumbers;

  // Create receiver account with 0 auto associations
  AccountId receiver;
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  ASSERT_NO_THROW(receiver = createAccount(receiverKey, 0, 0));

  // When
  // Airdrop the tokens
  TokenAirdropTransaction tx;
  TransactionResponse txResponse;
  ASSERT_NO_THROW(
    txResponse = tx.addNftTransfer(nftId.nft(nftSerials[0]), getTestClient().getOperatorAccountId().value(), receiver)
                   .addTokenTransfer(tokenId, receiver, 100) // Assuming amount is 100
                   .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100)
                   .execute(getTestClient()));
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  // Then
  // Verify the pending airdrops
  ASSERT_NE(txResponse.getRecord(getTestClient()).mPendingAirdropRecords.size(), 0);

  // Verify the receiver does not hold the tokens
  AccountBalance receiverAccountBalance;
  ASSERT_NO_THROW(receiverAccountBalance = AccountBalanceQuery().setAccountId(receiver).execute(getTestClient()));
  ASSERT_EQ(receiverAccountBalance.mTokens.count(tokenId), 0);
  ASSERT_EQ(receiverAccountBalance.mTokens.count(nftId), 0);

  // Verify the operator does hold the tokens
  AccountBalance operatorBalance;
  ASSERT_NO_THROW(
    operatorBalance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));
  ASSERT_EQ(100, operatorBalance.mTokens[tokenId]); // Assuming initial balance was 100
  ASSERT_EQ(1, operatorBalance.mTokens[nftId]);     // Assuming initial count was 1
}

//-----
TEST_F(TokenAirdropTransactionIntegrationTests, CanAirdropToAlias)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create fungible and NFT tokens
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));
  TokenId nftId;
  ASSERT_NO_THROW(nftId = createNft(operatorKey));

  // Mint some NFTs
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(nftId)
                                .setMetadata({ { std::byte(10) }, { std::byte(11) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  std::vector<uint64_t> nftSerials = txReceipt.mSerialNumbers;

  // Create alias account using public key
  std::shared_ptr<PrivateKey> aliasPrivateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId aliasAccountId = aliasPrivateKey->getPublicKey()->toAccountId();

  // When
  // Airdrop the tokens to the alias account
  ASSERT_NO_THROW(
    txReceipt =
      TokenAirdropTransaction()
        .addNftTransfer(nftId.nft(nftSerials[0]), getTestClient().getOperatorAccountId().value(), aliasAccountId)
        .addNftTransfer(nftId.nft(nftSerials[1]), getTestClient().getOperatorAccountId().value(), aliasAccountId)
        .addTokenTransfer(tokenId, aliasAccountId, 100)
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100)
        .execute(getTestClient())
        .getReceipt(getTestClient()));

  // Then
  // Verify the alias (hollow) account now holds the tokens
  AccountBalance aliasAccountBalance;
  ASSERT_NO_THROW(aliasAccountBalance = AccountBalanceQuery().setAccountId(aliasAccountId).execute(getTestClient()));
  ASSERT_EQ(100, aliasAccountBalance.mTokens[tokenId]);
  ASSERT_EQ(2, aliasAccountBalance.mTokens[nftId]);

  // Verify the operator does not hold the tokens
  AccountBalance operatorBalance;
  ASSERT_NO_THROW(
    operatorBalance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));
  ASSERT_EQ(0, operatorBalance.mTokens[tokenId]); // Assuming initial balance was 100
  ASSERT_EQ(0, operatorBalance.mTokens[nftId]);
}

//-----
TEST_F(TokenAirdropTransactionIntegrationTests, CanAirdropTokensWithReceiverSigRequiredFungible)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create fungible token
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));

  // Create receiver account with receiverSig=true and unlimited auto associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createReceiverAccountWithSig(receiverKey));

  // When
  // Airdrop tokens to receiver with receiverSig=true
  ASSERT_NO_THROW(TokenAirdropTransaction()
                    .addTokenTransfer(tokenId, receiverAccountId, 100) // Transfer 100 tokens
                    .addTokenTransfer(
                      tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 tokens from operator
                    .freezeWith(&getTestClient()) // Freeze the transaction to be signed
                    .sign(receiverKey)            // Receiver must sign due to receiverSig=true
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  // Verify the receiver holds the transferred tokens
  AccountBalance receiverBalance;
  ASSERT_NO_THROW(receiverBalance = AccountBalanceQuery().setAccountId(receiverAccountId).execute(getTestClient()));
  ASSERT_EQ(100, receiverBalance.mTokens[tokenId]);

  // Verify the operator's token balance is reduced
  AccountBalance operatorBalance;
  ASSERT_NO_THROW(
    operatorBalance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));
  ASSERT_EQ(0, operatorBalance.mTokens[tokenId]); // Assuming the initial balance was 100
}

//-----
TEST_F(TokenAirdropTransactionIntegrationTests, CanAirdropNftWithReceiverSigRequired)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create NFT token
  TokenId nftId;
  ASSERT_NO_THROW(nftId = createNft(operatorKey));

  // Mint some NFTs
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()

                                .setTokenId(nftId)
                                .setMetadata({ { std::byte(10) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  std::vector<uint64_t> nftSerials = txReceipt.mSerialNumbers;

  // Create receiver account with receiverSig=true and unlimited auto associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createReceiverAccountWithSig(receiverKey));

  // When
  // Airdrop NFTs to receiver with receiverSig=true
  ASSERT_NO_THROW(TokenAirdropTransaction()
                    .addNftTransfer(nftId.nft(nftSerials[0]),
                                    getTestClient().getOperatorAccountId().value(),
                                    receiverAccountId) // Transfer the minted NFT
                    .freezeWith(&getTestClient())      // Freeze the transaction to be signed
                    .sign(receiverKey)                 // Receiver must sign due to receiverSig=true
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  // Verify the receiver holds the transferred NFT
  AccountBalance receiverBalance;
  ASSERT_NO_THROW(receiverBalance = AccountBalanceQuery().setAccountId(receiverAccountId).execute(getTestClient()));
  ASSERT_EQ(1, receiverBalance.mTokens[nftId]); // Ensure the receiver holds the NFT

  // Verify the operator's NFT balance is reduced
  AccountBalance operatorBalance;
  ASSERT_NO_THROW(
    operatorBalance =
      AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient()));
  ASSERT_EQ(0, operatorBalance.mTokens[nftId]); // Assuming the operator started with 1 NFT and transferred it out
}

//-----
TEST_F(TokenAirdropTransactionIntegrationTests, CannotAirdropTokensWithAllowanceAndWithoutBalance)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create a fungible token
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));

  // Create a spender account and sender account
  std::shared_ptr<PrivateKey> spenderKey = ED25519PrivateKey::generatePrivateKey();
  AccountId spenderAccountId;
  ASSERT_NO_THROW(spenderAccountId = createAccount(spenderKey));

  std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  AccountId senderAccountId;
  ASSERT_NO_THROW(senderAccountId = createAccount(senderKey));

  // Transfer some tokens to the sender from the operator
  int64_t amount = 100; // Amount to transfer
  ASSERT_NO_THROW(transferToken(tokenId, getTestClient().getOperatorAccountId().value(), senderAccountId, amount));

  // Approve token allowance from sender to spender
  ASSERT_NO_THROW(approveTokenAllowance(tokenId, senderAccountId, spenderAccountId, amount, senderKey));

  // When / Then
  // Attempt to airdrop tokens using the allowance (fails with NOT_SUPPORTED)
  ASSERT_THROW(
    {
      TokenAirdropTransaction tx;
      tx.addTokenTransfer(tokenId, spenderAccountId, amount)         // Add to spender
        .addApprovedTokenTransfer(tokenId, senderAccountId, -amount) // Deduct from sender via approval
        .freezeWith(&getTestClient())                                // Freeze for signing
        .sign(spenderKey)                                            // Sign with spender key
        .execute(getTestClient())
        .getReceipt(getTestClient());
    },
    PrecheckStatusException);
}

TEST_F(TokenAirdropTransactionIntegrationTests, CannotAirdropNftsWithAllowanceAndWithoutBalance)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create NFT
  TokenId nftId;
  ASSERT_NO_THROW(nftId = createNft(operatorKey));

  // Mint 2 NFTs
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(nftId)
                                .setMetadata({ { std::byte(10) }, { std::byte(11) } })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  std::vector<uint64_t> nftSerials = txReceipt.mSerialNumbers;

  // Create spender account and sender account
  std::shared_ptr<PrivateKey> spenderKey = ED25519PrivateKey::generatePrivateKey();
  AccountId spenderAccountId;
  ASSERT_NO_THROW(spenderAccountId = createAccount(spenderKey));

  std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  AccountId senderAccountId;
  ASSERT_NO_THROW(senderAccountId = createAccount(senderKey));

  // Transfer NFTs to sender directly from the operator
  ASSERT_NO_THROW({
    TransferTransaction()
      .addNftTransfer(NftId(nftId, nftSerials[0]), getTestClient().getOperatorAccountId().value(), senderAccountId)
      .addNftTransfer(NftId(nftId, nftSerials[1]), getTestClient().getOperatorAccountId().value(), senderAccountId)
      .execute(getTestClient())
      .getReceipt(getTestClient());
  });

  // Approve NFT allowance from sender to spender directly
  ASSERT_NO_THROW({
    AccountAllowanceApproveTransaction()
      .approveTokenNftAllowance(NftId(nftId, nftSerials[0]), senderAccountId, spenderAccountId)
      .approveTokenNftAllowance(NftId(nftId, nftSerials[1]), senderAccountId, spenderAccountId)
      .freezeWith(&getTestClient())
      .sign(senderKey)
      .execute(getTestClient())
      .getReceipt(getTestClient());
  });

  // When
  // Attempt to airdrop NFTs using the allowance (fails with NOT_SUPPORTED)
  ASSERT_THROW(
    {
      TokenAirdropTransaction tx;
      tx.addApprovedNftTransfer(NftId(nftId, nftSerials[0]), senderAccountId, spenderAccountId)
        .addApprovedNftTransfer(NftId(nftId, nftSerials[1]), senderAccountId, spenderAccountId)
        .freezeWith(&getTestClient())
        .sign(spenderKey)
        .execute(getTestClient())
        .getReceipt(getTestClient());
    },
    PrecheckStatusException);
}