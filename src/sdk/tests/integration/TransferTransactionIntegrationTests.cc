// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenMintTransaction.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>
#include <transaction_record.pb.h>

using namespace Hiero;

class TransferTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TransferTransactionIntegrationTests, ExecuteTransferTransaction)
{
  // Given
  const Hbar amount(1LL);

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TransferTransaction()
                                 .addHbarTransfer(AccountId(2ULL), amount.negated())
                                 .addHbarTransfer(AccountId(3ULL), amount)
                                 .execute(getTestClient()));

  // Then
  TransactionRecord txRecord;
  ASSERT_NO_THROW(txRecord = txResponse.getRecord(getTestClient()));
  EXPECT_EQ(txRecord.mHbarTransferList.size(), 2);
}

//-----
TEST_F(TransferTransactionIntegrationTests, TransferNothing)
{
  // Given / When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt =
                    TransferTransaction().execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TransferTransactionIntegrationTests, TransferOutOfNonOperatorAccount)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const Hbar amount(1LL);
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  TransactionRecord txRecord;
  EXPECT_NO_THROW(txRecord = TransferTransaction()
                               .addHbarTransfer(AccountId(2ULL), amount)
                               .addHbarTransfer(accountId, amount.negated())
                               .freezeWith(&getTestClient())
                               .sign(privateKey)
                               .execute(getTestClient())
                               .getRecord(getTestClient()));

  // Then
  EXPECT_EQ(txRecord.mHbarTransferList.size(), 2);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanTransferHbarWithAliasID)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const Hbar amount(1LL);
  const EvmAddress evmAddress =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(privateKey->getPublicKey())->toEvmAddress();
  const AccountId aliasId(evmAddress);

  // When
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addHbarTransfer(AccountId(2ULL), amount.negated())
                                                         .addHbarTransfer(aliasId, amount)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  AccountInfo accountInfo;
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(aliasId).execute(getTestClient()));
  EXPECT_EQ(internal::HexConverter::hexToBytes(accountInfo.mContractAccountId), evmAddress.toBytes());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(aliasId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanSpendHbarAllowance)
{
  // Given
  const std::shared_ptr<PrivateKey> allowerKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> alloweeKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const Hbar balance(10LL);
  const Hbar amount(1LL);
  AccountId allowerId;
  AccountId alloweeId;
  ASSERT_NO_THROW(allowerId = AccountCreateTransaction()
                                .setKey(allowerKey->getPublicKey())
                                .setInitialBalance(balance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());
  ASSERT_NO_THROW(alloweeId = AccountCreateTransaction()
                                .setKey(alloweeKey->getPublicKey())
                                .setInitialBalance(balance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountAllowanceApproveTransaction()
                                                         .approveHbarAllowance(allowerId, alloweeId, amount)
                                                         .freezeWith(&getTestClient())
                                                         .sign(allowerKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // When
  TransactionRecord txRecord;
  EXPECT_NO_THROW(txRecord = TransferTransaction()
                               .addHbarTransfer(AccountId(2ULL), amount)
                               .addApprovedHbarTransfer(allowerId, amount.negated())
                               .setTransactionId(TransactionId::generate(alloweeId))
                               .freezeWith(&getTestClient())
                               .sign(alloweeKey)
                               .execute(getTestClient())
                               .getRecord(getTestClient()));

  // Then
  bool transferFound = false;
  for (const auto& transfer : txRecord.mHbarTransferList)
  {
    if (transfer.mAccountId == allowerId && transfer.mAmount == amount.negated())
    {
      transferFound = true;
      break;
    }
  }

  EXPECT_TRUE(transferFound);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(allowerId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(allowerKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(alloweeId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(alloweeKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanTransferFungibleTokenToAccountWithUnlimitedTokenAssociations)
{
  // Given
  const int64_t amount = 10LL;

  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  TokenId tokenId;
  AccountId senderId;
  AccountId receiverId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(-1)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -amount)
                    .addTokenTransfer(tokenId, senderId, amount)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  ASSERT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, senderId, -amount)
                    .addTokenTransfer(tokenId, receiverId, amount)
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  AccountBalance balance;
  ASSERT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiverId).execute(getTestClient()));
  EXPECT_EQ(balance.mTokens.size(), 1);
  ASSERT_TRUE(balance.mTokens.find(tokenId) != balance.mTokens.end());
  EXPECT_EQ(balance.mTokens.at(tokenId), static_cast<uint64_t>(amount));
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanTransferFungibleTokenToAccountWithNoTokenAssociationsIfAssociated)
{
  // Given
  const int64_t amount = 10LL;

  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  TokenId tokenId;
  AccountId senderId;
  AccountId receiverId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(0)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(receiverId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -amount)
                    .addTokenTransfer(tokenId, senderId, amount)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addTokenTransfer(tokenId, senderId, -amount)
                                                         .addTokenTransfer(tokenId, receiverId, amount)
                                                         .freezeWith(&getTestClient())
                                                         .sign(senderKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  AccountBalance balance;
  ASSERT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiverId).execute(getTestClient()));
  EXPECT_EQ(balance.mTokens.size(), 1);
  ASSERT_TRUE(balance.mTokens.find(tokenId) != balance.mTokens.end());
  EXPECT_EQ(balance.mTokens.at(tokenId), static_cast<uint64_t>(amount));
}

//-----
TEST_F(TransferTransactionIntegrationTests,
       CanTransferFungibleTokenToAccountWithUnlimitedTokenAssociationsWithAllowance)
{
  // Given
  const int64_t amount = 10LL;

  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  std::shared_ptr<PrivateKey> allowancedKey;
  TokenId tokenId;
  AccountId senderId;
  AccountId receiverId;
  AccountId allowancedId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(allowancedKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(-1)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());
  ASSERT_NO_THROW(allowancedId = AccountCreateTransaction()
                                   .setKey(allowancedKey)
                                   .execute(getTestClient())
                                   .getReceipt(getTestClient())
                                   .mAccountId.value());

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(AccountAllowanceApproveTransaction()
                    .approveTokenAllowance(tokenId, senderId, allowancedId, amount)
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -amount)
                    .addTokenTransfer(tokenId, senderId, amount)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-10LL))
                    .addHbarTransfer(allowancedId, Hbar(10LL))
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addApprovedTokenTransfer(tokenId, senderId, -amount)
                                                         .addTokenTransfer(tokenId, receiverId, amount)
                                                         .setTransactionId(TransactionId::generate(allowancedId))
                                                         .freezeWith(&getTestClient())
                                                         .sign(allowancedKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  AccountBalance balance;
  ASSERT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiverId).execute(getTestClient()));
  EXPECT_EQ(balance.mTokens.size(), 1);
  ASSERT_TRUE(balance.mTokens.find(tokenId) != balance.mTokens.end());
  EXPECT_EQ(balance.mTokens.at(tokenId), static_cast<uint64_t>(amount));
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanTransferFungibleTokenWithDecimalsToAccountWithUnlimitedTokenAssociations)
{
  // Given
  const int64_t amount = 10LL;
  const uint32_t decimals = 3U;

  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  TokenId tokenId;
  AccountId senderId;
  AccountId receiverId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000)
                              .setDecimals(decimals)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(-1)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(
    TransferTransaction()
      .addTokenTransferWithDecimals(tokenId, getTestClient().getOperatorAccountId().value(), -amount, decimals)
      .addTokenTransferWithDecimals(tokenId, senderId, amount, decimals)
      .execute(getTestClient())
      .getReceipt(getTestClient()));

  // When
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TransferTransaction()
                      .addTokenTransferWithDecimals(tokenId, senderId, -amount, decimals)
                      .addTokenTransferWithDecimals(tokenId, receiverId, amount, decimals)
                      .freezeWith(&getTestClient())
                      .sign(senderKey)
                      .execute(getTestClient())
                      .getReceipt(getTestClient()));

  // Then
  AccountBalance balance;
  ASSERT_NO_THROW(balance = AccountBalanceQuery().setAccountId(receiverId).execute(getTestClient()));
  EXPECT_EQ(balance.mTokens.size(), 1);
  ASSERT_TRUE(balance.mTokens.find(tokenId) != balance.mTokens.end());
  EXPECT_EQ(balance.mTokens.at(tokenId), static_cast<uint64_t>(amount));
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanTransferNftToAccountWithUnlimitedTokenAssociations)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  TokenId tokenId;
  NftId nftId;
  AccountId senderId;
  AccountId receiverId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(-1)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());

  ASSERT_NO_THROW(nftId = NftId(tokenId,
                                TokenMintTransaction()
                                  .setTokenId(tokenId)
                                  .addMetadata({ std::byte(0x00), std::byte(0x01), std::byte(0x02) })
                                  .execute(getTestClient())
                                  .getReceipt(getTestClient())
                                  .mSerialNumbers.front()));

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addNftTransfer(nftId, getTestClient().getOperatorAccountId().value(), senderId)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addNftTransfer(nftId, senderId, receiverId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(senderKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  AccountInfo info;
  ASSERT_NO_THROW(info = AccountInfoQuery().setAccountId(receiverId).execute(getTestClient()));
  EXPECT_EQ(info.mOwnedNfts, 1);
  EXPECT_EQ(info.mTokenRelationships.size(), 1);
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanTransferNftToAccountWithNoTokenAssociationsIfAssociated)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  TokenId tokenId;
  NftId nftId;
  AccountId senderId;
  AccountId receiverId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(0)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());

  ASSERT_NO_THROW(nftId = NftId(tokenId,
                                TokenMintTransaction()
                                  .setTokenId(tokenId)
                                  .addMetadata({ std::byte(0x00), std::byte(0x01), std::byte(0x02) })
                                  .execute(getTestClient())
                                  .getReceipt(getTestClient())
                                  .mSerialNumbers.front()));

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(receiverId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addNftTransfer(nftId, getTestClient().getOperatorAccountId().value(), senderId)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addNftTransfer(nftId, senderId, receiverId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(senderKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  AccountInfo info;
  ASSERT_NO_THROW(info = AccountInfoQuery().setAccountId(receiverId).execute(getTestClient()));
  EXPECT_EQ(info.mOwnedNfts, 1);
  EXPECT_EQ(info.mTokenRelationships.size(), 1);
}

//-----
TEST_F(TransferTransactionIntegrationTests, CanTransferNftToAccountWithUnlimitedTokenAssociationsWithAllowance)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  std::shared_ptr<PrivateKey> allowancedKey;
  TokenId tokenId;
  NftId nftId;
  AccountId senderId;
  AccountId receiverId;
  AccountId allowancedId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(allowancedKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(-1)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());
  ASSERT_NO_THROW(allowancedId = AccountCreateTransaction()
                                   .setKey(allowancedKey)
                                   .execute(getTestClient())
                                   .getReceipt(getTestClient())
                                   .mAccountId.value());

  ASSERT_NO_THROW(nftId = NftId(tokenId,
                                TokenMintTransaction()
                                  .setTokenId(tokenId)
                                  .addMetadata({ std::byte(0x00), std::byte(0x01), std::byte(0x02) })
                                  .execute(getTestClient())
                                  .getReceipt(getTestClient())
                                  .mSerialNumbers.front()));

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(AccountAllowanceApproveTransaction()
                    .approveNftAllowanceAllSerials(tokenId, senderId, allowancedId)
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addNftTransfer(nftId, getTestClient().getOperatorAccountId().value(), senderId)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-10LL))
                    .addHbarTransfer(allowancedId, Hbar(10LL))
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = TransferTransaction()
                                                         .addApprovedNftTransfer(nftId, senderId, receiverId)
                                                         .setTransactionId(TransactionId::generate(allowancedId))
                                                         .freezeWith(&getTestClient())
                                                         .sign(allowancedKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  AccountInfo info;
  ASSERT_NO_THROW(info = AccountInfoQuery().setAccountId(receiverId).execute(getTestClient()));
  EXPECT_EQ(info.mOwnedNfts, 1);
  EXPECT_EQ(info.mTokenRelationships.size(), 1);
}

//-----
TEST_F(TransferTransactionIntegrationTests, CannotTransferFungibleTokenToAccountWithNoTokenAssociations)
{
  // Given
  const int64_t amount = 10LL;

  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  TokenId tokenId;
  AccountId senderId;
  AccountId receiverId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(0)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -amount)
                    .addTokenTransfer(tokenId, senderId, amount)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When / Then
  EXPECT_THROW(TransferTransaction()
                 .addTokenTransfer(tokenId, senderId, -amount)
                 .addTokenTransfer(tokenId, receiverId, amount)
                 .freezeWith(&getTestClient())
                 .sign(senderKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // NO_REMAINING_AUTOMATIC_ASSOCIATIONS
}

//-----
TEST_F(TransferTransactionIntegrationTests, CannotTransferNftToAccountWithNoTokenAssociations)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> senderKey;
  std::shared_ptr<PrivateKey> receiverKey;
  TokenId tokenId;
  NftId nftId;
  AccountId senderId;
  AccountId receiverId;

  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));
  ASSERT_NO_THROW(senderKey = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(receiverKey = ED25519PrivateKey::generatePrivateKey());

  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(senderId = AccountCreateTransaction()
                               .setKey(senderKey)
                               .execute(getTestClient())
                               .getReceipt(getTestClient())
                               .mAccountId.value());
  ASSERT_NO_THROW(receiverId = AccountCreateTransaction()
                                 .setKey(receiverKey)
                                 .setMaxAutomaticTokenAssociations(0)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mAccountId.value());

  ASSERT_NO_THROW(nftId = NftId(tokenId,
                                TokenMintTransaction()
                                  .setTokenId(tokenId)
                                  .addMetadata({ std::byte(0x00), std::byte(0x01), std::byte(0x02) })
                                  .execute(getTestClient())
                                  .getReceipt(getTestClient())
                                  .mSerialNumbers.front()));

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(senderId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
  ASSERT_NO_THROW(TransferTransaction()
                    .addNftTransfer(nftId, getTestClient().getOperatorAccountId().value(), senderId)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When / Then
  EXPECT_THROW(TransferTransaction()
                 .addNftTransfer(nftId, senderId, receiverId)
                 .freezeWith(&getTestClient())
                 .sign(senderKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // NO_REMAINING_AUTOMATIC_ASSOCIATIONS
}
