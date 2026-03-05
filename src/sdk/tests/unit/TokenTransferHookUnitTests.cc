// SPDX-License-Identifier: Apache-2.0
#include "TokenTransfer.h"
#include "hooks/FungibleHookCall.h"
#include "hooks/FungibleHookType.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>

using namespace Hiero;

class TokenTransferHookUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTokenId; }
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline int64_t getTestAmount() const { return mAmount; }
  [[nodiscard]] inline uint32_t getTestExpectedDecimals() const { return mExpectedDecimals; }
  [[nodiscard]] inline bool getTestApproval() const { return mApproval; }
  [[nodiscard]] inline const FungibleHookCall& getTestHookCall() const { return mHookCall; }
  [[nodiscard]] inline int64_t getTestHookId1() const { return mHookId1; }
  [[nodiscard]] inline int64_t getTestHookId2() const { return mHookId2; }
  [[nodiscard]] inline int64_t getTestHookId3() const { return mHookId3; }
  [[nodiscard]] inline int64_t getTestZeroHookId() const { return mZeroHookId; }

private:
  const TokenId mTokenId = TokenId(1ULL, 2ULL, 3ULL);
  const AccountId mAccountId = AccountId(4ULL, 5ULL, 6ULL);
  const int64_t mAmount = 1000LL;
  const uint32_t mExpectedDecimals = 6U;
  const bool mApproval = true;
  const int64_t mHookId1 = 123LL;
  const int64_t mHookId2 = 456LL;
  const int64_t mHookId3 = 789LL;
  const int64_t mZeroHookId = 0LL;
  const FungibleHookCall mHookCall = []()
  {
    FungibleHookCall hookCall;
    hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
    hookCall.setHookId(123LL);
    return hookCall;
  }();
};

//-----
TEST_F(TokenTransferHookUnitTests, ConstructWithoutHookCall)
{
  // Given / When
  const TokenTransfer transfer(
    getTestTokenId(), getTestAccountId(), getTestAmount(), getTestExpectedDecimals(), getTestApproval());

  // Then
  EXPECT_EQ(transfer.mTokenId, getTestTokenId());
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestAmount());
  EXPECT_EQ(transfer.mExpectedDecimals, getTestExpectedDecimals());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), FungibleHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestZeroHookId());
}

//-----
TEST_F(TokenTransferHookUnitTests, ConstructWithHookCall)
{
  // Given / When
  const TokenTransfer transfer(
    getTestTokenId(), getTestAccountId(), getTestAmount(), getTestApproval(), getTestHookCall());

  // Then
  EXPECT_EQ(transfer.mTokenId, getTestTokenId());
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestAmount());
  EXPECT_EQ(transfer.mExpectedDecimals, 0U); // Should be 0 when not specified
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), getTestHookCall().getHookType());
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestHookCall().getHookId());
}

//-----
TEST_F(TokenTransferHookUnitTests, FromProtobufWithPreTxAllowanceHook)
{
  // Given
  proto::AccountAmount proto;
  proto.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  proto.set_amount(getTestAmount());
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId2());
  proto.set_allocated_pre_tx_allowance_hook(hookCallProto);

  // When
  const TokenTransfer transfer = TokenTransfer::fromProtobuf(proto, getTestTokenId(), getTestExpectedDecimals());

  // Then
  EXPECT_EQ(transfer.mTokenId, getTestTokenId());
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestAmount());
  EXPECT_EQ(transfer.mExpectedDecimals, getTestExpectedDecimals());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestHookId2());
}

//-----
TEST_F(TokenTransferHookUnitTests, FromProtobufWithPrePostTxAllowanceHook)
{
  // Given
  proto::AccountAmount proto;
  proto.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  proto.set_amount(getTestAmount());
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId3());
  proto.set_allocated_pre_post_tx_allowance_hook(hookCallProto);

  // When
  const TokenTransfer transfer = TokenTransfer::fromProtobuf(proto, getTestTokenId(), getTestExpectedDecimals());

  // Then
  EXPECT_EQ(transfer.mTokenId, getTestTokenId());
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestAmount());
  EXPECT_EQ(transfer.mExpectedDecimals, getTestExpectedDecimals());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestHookId3());
}

//-----
TEST_F(TokenTransferHookUnitTests, ToProtobufWithPreTxAllowanceHook)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId1());
  const TokenTransfer transfer(getTestTokenId(), getTestAccountId(), getTestAmount(), getTestApproval(), hookCall);

  // When
  const std::unique_ptr<proto::AccountAmount> proto = transfer.toProtobuf();

  // Then
  EXPECT_EQ(proto->accountid().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(proto->accountid().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(proto->accountid().accountnum(), getTestAccountId().mAccountNum);
  EXPECT_EQ(proto->amount(), getTestAmount());
  EXPECT_EQ(proto->is_approval(), getTestApproval());
  EXPECT_TRUE(proto->has_pre_tx_allowance_hook());
  EXPECT_EQ(proto->pre_tx_allowance_hook().hook_id(), getTestHookId1());
  EXPECT_FALSE(proto->has_pre_post_tx_allowance_hook());
}

//-----
TEST_F(TokenTransferHookUnitTests, ToProtobufWithPrePostTxAllowanceHook)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId2());
  const TokenTransfer transfer(getTestTokenId(), getTestAccountId(), getTestAmount(), getTestApproval(), hookCall);

  // When
  const std::unique_ptr<proto::AccountAmount> proto = transfer.toProtobuf();

  // Then
  EXPECT_EQ(proto->accountid().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(proto->accountid().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(proto->accountid().accountnum(), getTestAccountId().mAccountNum);
  EXPECT_EQ(proto->amount(), getTestAmount());
  EXPECT_EQ(proto->is_approval(), getTestApproval());
  EXPECT_FALSE(proto->has_pre_tx_allowance_hook());
  EXPECT_TRUE(proto->has_pre_post_tx_allowance_hook());
  EXPECT_EQ(proto->pre_post_tx_allowance_hook().hook_id(), getTestHookId2());
}

//-----
TEST_F(TokenTransferHookUnitTests, ToStringWithHookCall)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId3());
  const TokenTransfer transfer(getTestTokenId(), getTestAccountId(), getTestAmount(), getTestApproval(), hookCall);

  // When
  const std::string result = transfer.toString();

  // Then
  EXPECT_TRUE(result.find("PRE_POST_TX_ALLOWANCE_HOOK") != std::string::npos);
  EXPECT_TRUE(result.find("mHookType") != std::string::npos);
}
