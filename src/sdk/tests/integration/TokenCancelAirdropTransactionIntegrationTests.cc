// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "PendingAirdropId.h"
#include "TokenAirdropTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenCancelAirdropTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenFreezeTransaction.h"
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

class TokenCancelAirdropTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  TokenId createFt(const std::shared_ptr<PrivateKey>& operatorKey, bool pause = false, int amount = 100)
  {
    return TokenCreateTransaction()
      .setTokenName("ffff")
      .setTokenSymbol("F")
      .setTokenType(TokenType::FUNGIBLE_COMMON)
      .setInitialSupply(amount)
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
};

//-----
TEST_F(TokenCancelAirdropTransactionIntegrationTests, CanCancelTokens)
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

  // Create a receiver account with 0 automatic associations, so that the airdrop is pending
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createAccount(receiverKey, false, 0)); // 0 auto associations

  // When
  // Airdrop FT and NFTs to the receiver (tokens will be pending due to 0 associations)
  TransactionRecord airdropRecord;
  ASSERT_NO_THROW(
    airdropRecord =
      TokenAirdropTransaction()
        .addNftTransfer(NftId(nftId, nftSerials[0]), getTestClient().getOperatorAccountId().value(), receiverAccountId)
        .addNftTransfer(NftId(nftId, nftSerials[1]), getTestClient().getOperatorAccountId().value(), receiverAccountId)
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 FT from operator
        .addTokenTransfer(tokenId, receiverAccountId, 100)                               // Airdrop 100 FT to receiver
        .execute(getTestClient())
        .getRecord(getTestClient()););

  std::vector<PendingAirdropId> toCancel;
  for (const PendingAirdropRecord& pr : airdropRecord.mPendingAirdropRecords)
  {
    toCancel.push_back(pr.mPendingAirdropId);
  }

  // Cancel the pending airdrops (both FT and NFTs)
  TransactionRecord cancelAirdropRecord;
  ASSERT_NO_THROW(cancelAirdropRecord = TokenCancelAirdropTransaction()
                                          .setPendingAirdrops(toCancel)
                                          .execute(getTestClient())
                                          .getRecord(getTestClient()););

  // Ensure there are no pending airdrop records left after cancellation
  ASSERT_EQ(cancelAirdropRecord.mPendingAirdropRecords.size(), 0);

  // Verify that the receiver does not hold the FT or NFTs after cancellation
  AccountBalance receiverBalance = AccountBalanceQuery().setAccountId(receiverAccountId).execute(getTestClient());
  ASSERT_EQ(receiverBalance.mTokens.count(tokenId), 0); // No FT in receiver's account
  ASSERT_EQ(receiverBalance.mTokens.count(nftId), 0);   // No NFTs in receiver's account

  // Verify that the operator (original sender) holds the FT and NFTs again after cancellation
  AccountBalance operatorBalance =
    AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient());
  ASSERT_EQ(operatorBalance.mTokens.at(tokenId), 100); // Operator's FT balance restored
  ASSERT_EQ(operatorBalance.mTokens.at(nftId), 2);     // Operator holds 2 NFTs back
}

//-----
TEST_F(TokenCancelAirdropTransactionIntegrationTests, CanCancelTokensWhenTokenIsFrozen)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create a fungible token
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));

  // Create a receiver account with 0 automatic associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createAccount(receiverKey, false, 0)); // 0 auto associations

  // When
  // Airdrop FT to the receiver (tokens will be pending due to 0 associations)
  TransactionRecord airdropRecord;
  ASSERT_NO_THROW(
    airdropRecord =
      TokenAirdropTransaction()
        .addTokenTransfer(tokenId, receiverAccountId, 100)                               // Airdrop 100 FT
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 FT from operator
        .execute(getTestClient())
        .getRecord(getTestClient()));

  // Associate the token with the receiver account
  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(receiverAccountId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Freeze the token
  ASSERT_NO_THROW(TokenFreezeTransaction()
                    .setAccountId(receiverAccountId)
                    .setTokenId(tokenId)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Cancel the pending airdrop
  TransactionRecord cancelAirdropRecord;
  ASSERT_NO_THROW(cancelAirdropRecord =
                    TokenCancelAirdropTransaction()
                      .setPendingAirdrops({ airdropRecord.mPendingAirdropRecords[0].mPendingAirdropId })
                      .execute(getTestClient())
                      .getRecord(getTestClient()));

  // Ensure there are no pending airdrop records left after cancellation
  ASSERT_EQ(cancelAirdropRecord.mPendingAirdropRecords.size(), 0);

  // Verify that the receiver does not hold the tokens via query
  AccountBalance receiverBalance = AccountBalanceQuery().setAccountId(receiverAccountId).execute(getTestClient());
  ASSERT_EQ(receiverBalance.mTokens[tokenId], 0); // No FT in receiver's account

  // Verify that the operator (original sender) holds the tokens again after cancellation
  AccountBalance operatorBalance =
    AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient());
  ASSERT_EQ(operatorBalance.mTokens.at(tokenId), 100); // Operator's FT balance restored
}

//-----
TEST_F(TokenCancelAirdropTransactionIntegrationTests, CanCancelTokensWhenTokenIsPaused)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create a fungible token
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey, true));

  // Create a receiver account with 0 automatic associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createAccount(receiverKey, false, 0)); // 0 auto associations

  // When
  // Airdrop FT to the receiver (tokens will be pending due to 0 associations)
  TransactionRecord airdropRecord;
  ASSERT_NO_THROW(
    airdropRecord =
      TokenAirdropTransaction()
        .addTokenTransfer(tokenId, receiverAccountId, 100)                               // Airdrop 100 FT
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 FT from operator
        .execute(getTestClient())
        .getRecord(getTestClient()););

  // Pause the token
  ASSERT_NO_THROW(TokenPauseTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));

  // Cancel the pending airdrop
  TransactionRecord cancelAirdropRecord;
  ASSERT_NO_THROW(cancelAirdropRecord =
                    TokenCancelAirdropTransaction()
                      .setPendingAirdrops({ airdropRecord.mPendingAirdropRecords[0].mPendingAirdropId })
                      .execute(getTestClient())
                      .getRecord(getTestClient()););

  // Ensure there are no pending airdrop records left after cancellation
  ASSERT_EQ(cancelAirdropRecord.mPendingAirdropRecords.size(), 0);

  // Verify that the receiver does not hold the tokens via query
  AccountBalance receiverBalance = AccountBalanceQuery().setAccountId(receiverAccountId).execute(getTestClient());
  ASSERT_EQ(receiverBalance.mTokens[tokenId], 0); // No FT in receiver's account

  // Verify that the operator (original sender) holds the tokens again after cancellation
  AccountBalance operatorBalance =
    AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient());
  ASSERT_EQ(operatorBalance.mTokens.at(tokenId), 100); // Operator's FT balance restored
}

//-----
TEST_F(TokenCancelAirdropTransactionIntegrationTests, CanCancelTokensWhenTokenIsDeleted)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create a fungible token
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));

  // Create a receiver account with 0 automatic associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createAccount(receiverKey, false, 0)); // 0 auto associations

  // When
  // Airdrop FT to the receiver (tokens will be pending due to 0 associations)
  TransactionRecord airdropRecord;
  ASSERT_NO_THROW(
    airdropRecord =
      TokenAirdropTransaction()
        .addTokenTransfer(tokenId, receiverAccountId, 100)                               // Airdrop 100 FT
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 FT from operator
        .execute(getTestClient())
        .getRecord(getTestClient()););

  // Delete the token
  ASSERT_NO_THROW(TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));

  // Cancel the pending airdrop
  TransactionRecord cancelAirdropRecord;
  ASSERT_NO_THROW(cancelAirdropRecord =
                    TokenCancelAirdropTransaction()
                      .setPendingAirdrops({ airdropRecord.mPendingAirdropRecords[0].mPendingAirdropId })
                      .execute(getTestClient())
                      .getRecord(getTestClient()););

  // Then
  // Ensure there are no pending airdrop records left after cancellation
  ASSERT_EQ(cancelAirdropRecord.mPendingAirdropRecords.size(), 0);

  // Verify that the receiver does not hold the tokens via query
  AccountBalance receiverBalance = AccountBalanceQuery().setAccountId(receiverAccountId).execute(getTestClient());
  ASSERT_EQ(receiverBalance.mTokens[tokenId], 0); // No FT in receiver's account

  // Verify that the operator’s token balance is restored (though it should ideally be unaffected by deletion)
  AccountBalance operatorBalance =
    AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient());
  ASSERT_EQ(operatorBalance.mTokens[tokenId], 100);
}

//-----
TEST_F(TokenCancelAirdropTransactionIntegrationTests, CanCancelTokensToMultipleReceivers)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create fungible and NFT tokens
  TokenId tokenId;
  TokenId nftId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey, false, 1000));
  ASSERT_NO_THROW(nftId = createNft(operatorKey));

  // Mint some NFTs
  std::vector<std::vector<std::byte>> metadata = {
    { std::byte(10) }, { std::byte(11) }, { std::byte(12) }, { std::byte(13) }
  };

  TransactionReceipt txReceipt;

  ASSERT_NO_THROW(txReceipt = TokenMintTransaction()
                                .setTokenId(nftId)
                                .setMetadata(metadata)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()););

  std::vector<uint64_t> nftSerials = txReceipt.mSerialNumbers;

  // Create receiver1 and receiver2 with 0 auto associations
  std::shared_ptr<PrivateKey> receiver1Key = ED25519PrivateKey::generatePrivateKey();
  std::shared_ptr<PrivateKey> receiver2Key = ED25519PrivateKey::generatePrivateKey();
  AccountId receiver1AccountId = createAccount(receiver1Key, false, 0);
  AccountId receiver2AccountId = createAccount(receiver2Key, false, 0);

  // When
  // Airdrop tokens to both receivers
  TransactionRecord airdropRecord;
  ASSERT_NO_THROW(
    airdropRecord =
      TokenAirdropTransaction()
        .addNftTransfer(NftId(nftId, nftSerials[0]), getTestClient().getOperatorAccountId().value(), receiver1AccountId)
        .addNftTransfer(NftId(nftId, nftSerials[1]), getTestClient().getOperatorAccountId().value(), receiver1AccountId)
        .addNftTransfer(NftId(nftId, nftSerials[2]), getTestClient().getOperatorAccountId().value(), receiver2AccountId)
        .addNftTransfer(NftId(nftId, nftSerials[3]), getTestClient().getOperatorAccountId().value(), receiver2AccountId)
        .addTokenTransfer(tokenId, receiver1AccountId, 100)
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100)
        .addTokenTransfer(tokenId, receiver2AccountId, 100)
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100)
        .execute(getTestClient())
        .getRecord(getTestClient()););

  // Cancel the pending airdrops
  std::vector<PendingAirdropId> pendingAirdropIds;
  for (const PendingAirdropRecord& record : airdropRecord.mPendingAirdropRecords)
  {
    pendingAirdropIds.push_back(record.mPendingAirdropId);
  }

  TransactionRecord cancelAirdropRecord;
  ASSERT_NO_THROW(cancelAirdropRecord = TokenCancelAirdropTransaction()
                                          .setPendingAirdrops(pendingAirdropIds)
                                          .execute(getTestClient())
                                          .getRecord(getTestClient()););

  // Then
  ASSERT_EQ(0, cancelAirdropRecord.mPendingAirdropRecords.size());

  // Verify that the receivers do not hold the tokens
  AccountBalance receiver1Balance = AccountBalanceQuery().setAccountId(receiver1AccountId).execute(getTestClient());
  AccountBalance receiver2Balance = AccountBalanceQuery().setAccountId(receiver2AccountId).execute(getTestClient());

  ASSERT_EQ(receiver1Balance.mTokens.count(tokenId), 0);
  ASSERT_EQ(receiver2Balance.mTokens.count(tokenId), 0);

  // Verify that the operator holds the tokens
  AccountBalance operatorBalance =
    AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient());

  ASSERT_EQ(operatorBalance.mTokens.at(tokenId),
            1000); // Operator should have the full initial supply      // Operator should hold all NFTs again
}

TEST_F(TokenCancelAirdropTransactionIntegrationTests, CancelsTokensFromMultipleAirdropTxns)
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

  // Create a receiver account with 0 automatic associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createAccount(receiverKey, false, 0)); // 0 auto associations

  // When
  // Airdrop some of the tokens to the receiver (NFTs and Fungible Token in separate transactions)
  TransactionRecord record1 =
    TokenAirdropTransaction()
      .addNftTransfer(NftId(nftId, nftSerials[0]), getTestClient().getOperatorAccountId().value(), receiverAccountId)
      .execute(getTestClient())
      .getRecord(getTestClient());

  TransactionRecord record2 =
    TokenAirdropTransaction()
      .addNftTransfer(NftId(nftId, nftSerials[1]), getTestClient().getOperatorAccountId().value(), receiverAccountId)
      .execute(getTestClient())
      .getRecord(getTestClient());

  TransactionRecord record3 =
    TokenAirdropTransaction()
      .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 FT from operator
      .addTokenTransfer(tokenId, receiverAccountId, 100)                               // Airdrop 100 FT to receiver
      .execute(getTestClient())
      .getRecord(getTestClient());

  // Collect the Pending Airdrop IDs from the records
  std::vector<PendingAirdropId> pendingAirdropIds;
  pendingAirdropIds.push_back(record1.mPendingAirdropRecords[0].mPendingAirdropId);
  pendingAirdropIds.push_back(record2.mPendingAirdropRecords[0].mPendingAirdropId);
  pendingAirdropIds.push_back(record3.mPendingAirdropRecords[0].mPendingAirdropId);

  // Cancel all the tokens from the pending airdrop
  TransactionRecord cancelAirdropRecord;
  ASSERT_NO_THROW(cancelAirdropRecord = TokenCancelAirdropTransaction()
                                          .setPendingAirdrops(pendingAirdropIds)
                                          .execute(getTestClient())
                                          .getRecord(getTestClient()););

  // Then
  // Ensure no pending airdrop records left after cancellation
  ASSERT_EQ(cancelAirdropRecord.mPendingAirdropRecords.size(), 0);

  // Verify the receiver does not hold the tokens (Fungible Token or NFTs) after cancellation
  AccountBalance receiverBalance = AccountBalanceQuery().setAccountId(receiverAccountId).execute(getTestClient());
  ASSERT_EQ(receiverBalance.mTokens.count(tokenId), 0); // No FT in receiver's account
  ASSERT_EQ(receiverBalance.mTokens.count(nftId), 0);   // No NFTs in receiver's account

  // Verify the operator holds the tokens again after cancellation
  AccountBalance operatorBalance =
    AccountBalanceQuery().setAccountId(getTestClient().getOperatorAccountId().value()).execute(getTestClient());
  ASSERT_EQ(operatorBalance.mTokens.at(tokenId), 100); // Operator's FT balance restored
  ASSERT_EQ(operatorBalance.mTokens.at(nftId), 2);     // Operator holds 2 NFTs back
}

TEST_F(TokenCancelAirdropTransactionIntegrationTests, CannotCancelTokensForNonExistingAirdrop)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create fungible token
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));

  // Create a receiver account with 0 automatic associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createAccount(receiverKey, false, 0)); // 0 auto associations

  // When
  // Airdrop some fungible tokens to the receiver
  TransactionRecord record;
  ASSERT_NO_THROW(
    record =
      TokenAirdropTransaction()
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 FT from operator
        .addTokenTransfer(tokenId, receiverAccountId, 100)                               // Airdrop 100 FT to receiver
        .execute(getTestClient())
        .getRecord(getTestClient()););

  // Create another random account with 0 auto associations (not involved in any airdrop)
  std::shared_ptr<PrivateKey> randomAccountKey = ED25519PrivateKey::generatePrivateKey();
  AccountId randomAccountId = createAccount(randomAccountKey, false, 0); // 0 auto associations

  // When / Then
  // Attempt to cancel tokens from the random account that has no pending airdrops, expecting INVALID_SIGNATURE
  ASSERT_THROW(TokenCancelAirdropTransaction()
                 .setTransactionId(TransactionId::generate(randomAccountId))
                 .setPendingAirdrops({ record.mPendingAirdropRecords.at(0).mPendingAirdropId })
                 .execute(getTestClient()),
               PrecheckStatusException);
}

//-----
TEST_F(TokenCancelAirdropTransactionIntegrationTests, CannotCancelTokensForAlreadyCanceledAirdrop)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Create fungible token
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = createFt(operatorKey));

  // Create a receiver account with 0 automatic associations
  std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  AccountId receiverAccountId;
  ASSERT_NO_THROW(receiverAccountId = createAccount(receiverKey, false, 0)); // 0 auto associations

  // When
  // Airdrop some fungible tokens to the receiver
  TransactionRecord record;
  ASSERT_NO_THROW(
    record =
      TokenAirdropTransaction()
        .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -100) // Deduct 100 FT from operator
        .addTokenTransfer(tokenId, receiverAccountId, 100)                               // Airdrop 100 FT to receiver
        .execute(getTestClient())
        .getRecord(getTestClient()););

  // Then
  // Cancel the tokens for the receiver (valid cancellation)
  ASSERT_NO_THROW(TokenCancelAirdropTransaction()
                    .setPendingAirdrops({ record.mPendingAirdropRecords.at(0).mPendingAirdropId })
                    .execute(getTestClient()));

  // Attempt to cancel the tokens again, expecting INVALID_PENDING_AIRDROP_ID
  ASSERT_THROW(TokenCancelAirdropTransaction()
                 .setPendingAirdrops({ record.mPendingAirdropRecords.at(0).mPendingAirdropId })
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(TokenCancelAirdropTransactionIntegrationTests, CannotCancelWithEmptyPendingAirdropsList)
{
  // Given / When / Then
  // Attempt to cancel tokens without providing pendingAirdropIds
  ASSERT_THROW(TokenCancelAirdropTransaction().execute(getTestClient()), PrecheckStatusException);
}
