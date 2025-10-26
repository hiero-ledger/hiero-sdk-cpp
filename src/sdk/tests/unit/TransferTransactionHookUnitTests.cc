// SPDX-License-Identifier: Apache-2.0
#include "TransferTransaction.h"
#include "hooks/FungibleHookCall.h"
#include "hooks/FungibleHookType.h"
#include "hooks/NftHookCall.h"
#include "hooks/NftHookType.h"

#include <gtest/gtest.h>
#include <services/transaction.pb.h>

using namespace Hiero;

class TransferTransactionHookUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mTransaction.setNodeAccountIds({ AccountId(1ULL) });
    mTransaction.setTransactionId(TransactionId::generate(AccountId(1ULL)));
  }

  [[nodiscard]] inline const AccountId& getTestAccountId1() const { return mAccountId1; }
  [[nodiscard]] inline const AccountId& getTestAccountId2() const { return mAccountId2; }
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTokenId; }
  [[nodiscard]] inline const NftId& getTestNftId() const { return mNftId; }
  [[nodiscard]] inline const Hbar& getTestAmount() const { return mAmount; }
  [[nodiscard]] inline const FungibleHookCall& getTestFungibleHookCall() const { return mFungibleHookCall; }
  [[nodiscard]] inline const NftHookCall& getTestNftHookCall() const { return mNftHookCall; }
  [[nodiscard]] inline TransferTransaction& getTransaction() { return mTransaction; }

private:
  const AccountId mAccountId1 = AccountId(10ULL);
  const AccountId mAccountId2 = AccountId(20ULL);
  const TokenId mTokenId = TokenId(30ULL);
  const NftId mNftId = NftId(mTokenId, 40ULL);
  const Hbar mAmount = Hbar(50ULL);
  const FungibleHookCall mFungibleHookCall = []() {
    FungibleHookCall hookCall;
    hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
    hookCall.setHookId(123LL);
    return hookCall;
  }();
  const NftHookCall mNftHookCall = []() {
    NftHookCall hookCall;
    hookCall.setHookType(NftHookType::PRE_HOOK);
    hookCall.setHookId(456LL);
    return hookCall;
  }();

  TransferTransaction mTransaction;
};

//-----
TEST_F(TransferTransactionHookUnitTests, AddHbarTransferWithHook)
{
  // Given
  TransferTransaction& transaction = getTransaction();

  // When
  EXPECT_NO_THROW(transaction.addHbarTransferWithHook(getTestAccountId1(), getTestAmount(), getTestFungibleHookCall()));

  // Then
  const auto hbarTransfers = transaction.getHbarTransfers();
  EXPECT_FALSE(hbarTransfers.empty());
  EXPECT_EQ(hbarTransfers.cbegin()->first, getTestAccountId1());
  EXPECT_EQ(hbarTransfers.cbegin()->second, getTestAmount());
}

//-----
TEST_F(TransferTransactionHookUnitTests, AddTokenTransferWithHook)
{
  // Given
  TransferTransaction& transaction = getTransaction();

  // When
  EXPECT_NO_THROW(transaction.addTokenTransferWithHook(
    getTestTokenId(), getTestAccountId1(), getTestAmount().toTinybars(), getTestFungibleHookCall()));

  // Then
  const auto tokenTransfers = transaction.getTokenTransfers();
  EXPECT_FALSE(tokenTransfers.empty());
  EXPECT_EQ(tokenTransfers.cbegin()->first, getTestTokenId());
  EXPECT_EQ(tokenTransfers.cbegin()->second.cbegin()->first, getTestAccountId1());
  EXPECT_EQ(tokenTransfers.cbegin()->second.cbegin()->second, getTestAmount().toTinybars());
}

//-----
TEST_F(TransferTransactionHookUnitTests, AddNftTransferWithHook)
{
  // Given
  TransferTransaction& transaction = getTransaction();

  // When
  EXPECT_NO_THROW(transaction.addNftTransferWithHook(
    getTestNftId(), getTestAccountId1(), getTestAccountId2(), getTestNftHookCall(), getTestNftHookCall()));

  // Then
  const auto nftTransfers = transaction.getNftTransfers();
  EXPECT_FALSE(nftTransfers.empty());
  EXPECT_EQ(nftTransfers.cbegin()->first, getTestNftId().mTokenId);
  EXPECT_EQ(nftTransfers.cbegin()->second.size(), 1);
  EXPECT_EQ(nftTransfers.cbegin()->second.cbegin()->mNftId, getTestNftId());
  EXPECT_EQ(nftTransfers.cbegin()->second.cbegin()->mSenderAccountId, getTestAccountId1());
  EXPECT_EQ(nftTransfers.cbegin()->second.cbegin()->mReceiverAccountId, getTestAccountId2());
}

//-----
TEST_F(TransferTransactionHookUnitTests, AddHbarTransferWithHookFrozen)
{
  // Given
  TransferTransaction& transaction = getTransaction();
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.addHbarTransferWithHook(getTestAccountId1(), getTestAmount(), getTestFungibleHookCall()),
               IllegalStateException);
}

//-----
TEST_F(TransferTransactionHookUnitTests, AddTokenTransferWithHookFrozen)
{
  // Given
  TransferTransaction& transaction = getTransaction();
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.addTokenTransferWithHook(
                 getTestTokenId(), getTestAccountId1(), getTestAmount().toTinybars(), getTestFungibleHookCall()),
               IllegalStateException);
}

//-----
TEST_F(TransferTransactionHookUnitTests, AddNftTransferWithHookFrozen)
{
  // Given
  TransferTransaction& transaction = getTransaction();
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.addNftTransferWithHook(
                 getTestNftId(), getTestAccountId1(), getTestAccountId2(), getTestNftHookCall(), getTestNftHookCall()),
               IllegalStateException);
}

//-----
TEST_F(TransferTransactionHookUnitTests, ConstructTransferTransactionFromProtobufWithHooks)
{
  // Given
  auto body = std::make_unique<proto::CryptoTransferTransactionBody>();

  // Add Hbar transfer with hook
  auto accountAmount = std::make_unique<proto::AccountAmount>();
  accountAmount->set_allocated_accountid(getTestAccountId1().toProtobuf().release());
  accountAmount->set_amount(getTestAmount().toTinybars());
  accountAmount->set_is_approval(false);

  proto::HookCall hookCallProto;
  hookCallProto.set_hook_id(789LL);
  accountAmount->set_allocated_pre_tx_allowance_hook(hookCallProto.release());
  body->mutable_hbar_transfers()->AddAllocated(accountAmount.release());

  // Add Token transfer with hook
  auto tokenTransfer = std::make_unique<proto::TokenTransferList>();
  tokenTransfer->set_allocated_token(getTestTokenId().toProtobuf().release());

  auto tokenAccountAmount = std::make_unique<proto::AccountAmount>();
  tokenAccountAmount->set_allocated_accountid(getTestAccountId2().toProtobuf().release());
  tokenAccountAmount->set_amount(getTestAmount().toTinybars());
  tokenAccountAmount->set_is_approval(false);

  proto::HookCall tokenHookCallProto;
  tokenHookCallProto.set_hook_id(101112LL);
  tokenAccountAmount->set_allocated_pre_post_tx_allowance_hook(tokenHookCallProto.release());
  tokenTransfer->mutable_transfers()->AddAllocated(tokenAccountAmount.release());
  body->mutable_token_transfers()->AddAllocated(tokenTransfer.release());

  // Add NFT transfer with hooks
  auto nftTransfer = std::make_unique<proto::NftTransfer>();
  nftTransfer->set_allocated_tokenid(getTestNftId().mTokenId.toProtobuf().release());
  nftTransfer->set_allocated_senderaccountid(getTestAccountId1().toProtobuf().release());
  nftTransfer->set_allocated_receiveraccountid(getTestAccountId2().toProtobuf().release());
  nftTransfer->set_serialnumber(static_cast<int64_t>(getTestNftId().mSerialNum));
  nftTransfer->set_is_approval(false);

  proto::HookCall senderHookCallProto;
  senderHookCallProto.set_hook_id(131415LL);
  nftTransfer->set_allocated_pre_tx_sender_allowance_hook(senderHookCallProto.release());

  proto::HookCall receiverHookCallProto;
  receiverHookCallProto.set_hook_id(161718LL);
  nftTransfer->set_allocated_pre_post_tx_receiver_allowance_hook(receiverHookCallProto.release());
  body->mutable_nft_transfers()->AddAllocated(nftTransfer.release());

  // When
  const TransferTransaction transaction = TransferTransaction::fromTransactionBodyProtobuf(*body);

  // Then
  const auto hbarTransfers = transaction.getHbarTransfers();
  EXPECT_FALSE(hbarTransfers.empty());
  EXPECT_EQ(hbarTransfers.cbegin()->first, getTestAccountId1());
  EXPECT_EQ(hbarTransfers.cbegin()->second, getTestAmount());

  const auto tokenTransfers = transaction.getTokenTransfers();
  EXPECT_FALSE(tokenTransfers.empty());
  EXPECT_EQ(tokenTransfers.cbegin()->first, getTestTokenId());

  const auto nftTransfers = transaction.getNftTransfers();
  EXPECT_FALSE(nftTransfers.empty());
  EXPECT_EQ(nftTransfers.cbegin()->first, getTestNftId().mTokenId);
}

//-----
TEST_F(TransferTransactionHookUnitTests, ToTransactionBodyProtobufWithHooks)
{
  // Given
  TransferTransaction& transaction = getTransaction();
  transaction.addHbarTransferWithHook(getTestAccountId1(), getTestAmount(), getTestFungibleHookCall());
  transaction.addTokenTransferWithHook(
    getTestTokenId(), getTestAccountId2(), getTestAmount().toTinybars(), getTestFungibleHookCall());
  transaction.addNftTransferWithHook(
    getTestNftId(), getTestAccountId1(), getTestAccountId2(), getTestNftHookCall(), getTestNftHookCall());

  // When
  const std::unique_ptr<proto::CryptoTransferTransactionBody> body = transaction.toTransactionBodyProtobuf();

  // Then
  EXPECT_EQ(body->hbar_transfers_size(), 1);
  EXPECT_EQ(body->token_transfers_size(), 1);
  EXPECT_EQ(body->nft_transfers_size(), 1);

  // Verify Hbar transfer hook
  const auto& hbarTransfer = body->hbar_transfers(0);
  EXPECT_TRUE(hbarTransfer.has_pre_tx_allowance_hook());
  EXPECT_EQ(hbarTransfer.pre_tx_allowance_hook().hook_id(), getTestFungibleHookCall().getHookId());

  // Verify Token transfer hook
  const auto& tokenTransfer = body->token_transfers(0);
  EXPECT_EQ(tokenTransfer.transfers_size(), 1);
  const auto& tokenAccountAmount = tokenTransfer.transfers(0);
  EXPECT_TRUE(tokenAccountAmount.has_pre_tx_allowance_hook());
  EXPECT_EQ(tokenAccountAmount.pre_tx_allowance_hook().hook_id(), getTestFungibleHookCall().getHookId());

  // Verify NFT transfer hooks
  const auto& nftTransfer = body->nft_transfers(0);
  EXPECT_TRUE(nftTransfer.has_pre_tx_sender_allowance_hook());
  EXPECT_EQ(nftTransfer.pre_tx_sender_allowance_hook().hook_id(), getTestNftHookCall().getHookId());
  EXPECT_TRUE(nftTransfer.has_pre_tx_receiver_allowance_hook());
  EXPECT_EQ(nftTransfer.pre_tx_receiver_allowance_hook().hook_id(), getTestNftHookCall().getHookId());
}
