// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Hbar.h"
#include "HbarTransfer.h"
#include "hooks/FungibleHookCall.h"
#include "hooks/FungibleHookType.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>

using namespace Hiero;

class HbarTransferUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const int64_t& getTestAmount() const { return mAmount; }
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline const Hbar& getTestHbarAmount() const { return mHbarAmount; }
  [[nodiscard]] inline bool getTestApproval() const { return mApproval; }
  [[nodiscard]] inline const FungibleHookCall& getTestHookCall() const { return mHookCall; }
  [[nodiscard]] inline int64_t getTestHookId1() const { return mHookId1; }
  [[nodiscard]] inline int64_t getTestHookId2() const { return mHookId2; }
  [[nodiscard]] inline int64_t getTestHookId3() const { return mHookId3; }
  [[nodiscard]] inline int64_t getTestZeroHookId() const { return mZeroHookId; }

private:
  const int64_t mAmount = 10LL;
  const AccountId mAccountId = AccountId(10ULL);
  const Hbar mHbarAmount = Hbar(100ULL);
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

// Tests serialization of Hiero::HbarTransfer -> proto::AccountAmount.
TEST_F(HbarTransferUnitTests, SerializeTransferToProtobuf)
{
  // Given
  const HbarTransfer testTransfer(getTestAccountId(), Hbar(getTestAmount(), HbarUnit::TINYBAR()), false);

  // When
  const auto protoAccountAmountPtr = std::unique_ptr<proto::AccountAmount>(testTransfer.toProtobuf());

  // Then
  EXPECT_EQ(protoAccountAmountPtr->accountid().accountnum(), getTestAccountId().mAccountNum);
  EXPECT_EQ(protoAccountAmountPtr->amount(), getTestAmount());
  EXPECT_FALSE(protoAccountAmountPtr->is_approval());
}

// Tests deserialization of proto::AccountAmount -> Hiero::HbarTransfer.
TEST_F(HbarTransferUnitTests, DeserializeTransferFromProtobuf)
{
  // Given
  const AccountId testAccountId = getTestAccountId();
  const int64_t testAmount = getTestAmount();
  proto::AccountAmount testProtoAccountAmount;
  testProtoAccountAmount.set_allocated_accountid(testAccountId.toProtobuf().release());
  testProtoAccountAmount.set_amount(testAmount);
  testProtoAccountAmount.set_is_approval(true);

  // When
  const HbarTransfer transfer = HbarTransfer::fromProtobuf(testProtoAccountAmount);

  // Then
  EXPECT_EQ(transfer.mAccountId, testAccountId);
  EXPECT_EQ(transfer.mAmount.toTinybars(), testAmount);
  EXPECT_TRUE(transfer.mIsApproved);
}

// Tests serialization & deserialization of Hiero::HbarTransfer -> proto::AccountAmount -> Hiero::HbarTransfer.
TEST_F(HbarTransferUnitTests, ProtoTransfer)
{
  AccountId accountId(10ULL);
  int64_t amount = 10LL;

  proto::AccountAmount protoAccountAmount;
  protoAccountAmount.set_allocated_accountid(accountId.toProtobuf().release());
  protoAccountAmount.set_amount(amount);
  protoAccountAmount.set_is_approval(true);

  HbarTransfer transfer = HbarTransfer::fromProtobuf(protoAccountAmount);
  EXPECT_EQ(transfer.mAccountId, accountId);
  EXPECT_EQ(transfer.mAmount.toTinybars(), amount);
  EXPECT_TRUE(transfer.mIsApproved);

  accountId.mAccountNum = 15ULL;
  amount = 15LL;

  transfer.mAccountId = accountId;
  transfer.mAmount = Hbar(amount, HbarUnit::TINYBAR());
  transfer.mIsApproved = false;

  const auto protoAccountAmountPtr = std::unique_ptr<proto::AccountAmount>(transfer.toProtobuf());
  EXPECT_EQ(protoAccountAmountPtr->accountid().accountnum(), accountId.mAccountNum);
  EXPECT_EQ(protoAccountAmountPtr->amount(), amount);
  EXPECT_FALSE(protoAccountAmountPtr->is_approval());
}

//-----
TEST_F(HbarTransferUnitTests, ConstructWithoutHookCall)
{
  // Given / When
  const HbarTransfer transfer(getTestAccountId(), getTestHbarAmount(), getTestApproval());

  // Then
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestHbarAmount());
  EXPECT_EQ(transfer.mIsApproved, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), FungibleHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestZeroHookId());
}

//-----
TEST_F(HbarTransferUnitTests, ConstructWithHookCall)
{
  // Given / When
  const HbarTransfer transfer(getTestAccountId(), getTestHbarAmount(), getTestApproval(), getTestHookCall());

  // Then
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestHbarAmount());
  EXPECT_EQ(transfer.mIsApproved, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), getTestHookCall().getHookType());
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestHookCall().getHookId());
}

//-----
TEST_F(HbarTransferUnitTests, FromProtobufWithPreTxAllowanceHook)
{
  // Given
  proto::AccountAmount proto;
  proto.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  proto.set_amount(getTestHbarAmount().toTinybars());
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId2());
  proto.set_allocated_pre_tx_allowance_hook(hookCallProto);

  // When
  const HbarTransfer transfer = HbarTransfer::fromProtobuf(proto);

  // Then
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestHbarAmount());
  EXPECT_EQ(transfer.mIsApproved, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestHookId2());
}

//-----
TEST_F(HbarTransferUnitTests, FromProtobufWithPrePostTxAllowanceHook)
{
  // Given
  proto::AccountAmount proto;
  proto.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  proto.set_amount(getTestHbarAmount().toTinybars());
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId3());
  proto.set_allocated_pre_post_tx_allowance_hook(hookCallProto);

  // When
  const HbarTransfer transfer = HbarTransfer::fromProtobuf(proto);

  // Then
  EXPECT_EQ(transfer.mAccountId, getTestAccountId());
  EXPECT_EQ(transfer.mAmount, getTestHbarAmount());
  EXPECT_EQ(transfer.mIsApproved, getTestApproval());
  EXPECT_EQ(transfer.mHookCall.getHookType(), FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  EXPECT_EQ(transfer.mHookCall.getHookId(), getTestHookId3());
}

//-----
TEST_F(HbarTransferUnitTests, ToProtobufWithPreTxAllowanceHook)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId1());
  const HbarTransfer transfer(getTestAccountId(), getTestHbarAmount(), getTestApproval(), hookCall);

  // When
  const std::unique_ptr<proto::AccountAmount> proto = transfer.toProtobuf();

  // Then
  EXPECT_EQ(proto->accountid().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(proto->accountid().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(proto->accountid().accountnum(), getTestAccountId().mAccountNum);
  EXPECT_EQ(proto->amount(), getTestHbarAmount().toTinybars());
  EXPECT_EQ(proto->is_approval(), getTestApproval());
  EXPECT_TRUE(proto->has_pre_tx_allowance_hook());
  EXPECT_EQ(proto->pre_tx_allowance_hook().hook_id(), getTestHookId1());
  EXPECT_FALSE(proto->has_pre_post_tx_allowance_hook());
}

//-----
TEST_F(HbarTransferUnitTests, ToProtobufWithPrePostTxAllowanceHook)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId2());
  const HbarTransfer transfer(getTestAccountId(), getTestHbarAmount(), getTestApproval(), hookCall);

  // When
  const std::unique_ptr<proto::AccountAmount> proto = transfer.toProtobuf();

  // Then
  EXPECT_EQ(proto->accountid().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(proto->accountid().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(proto->accountid().accountnum(), getTestAccountId().mAccountNum);
  EXPECT_EQ(proto->amount(), getTestHbarAmount().toTinybars());
  EXPECT_EQ(proto->is_approval(), getTestApproval());
  EXPECT_FALSE(proto->has_pre_tx_allowance_hook());
  EXPECT_TRUE(proto->has_pre_post_tx_allowance_hook());
  EXPECT_EQ(proto->pre_post_tx_allowance_hook().hook_id(), getTestHookId2());
}

//-----
TEST_F(HbarTransferUnitTests, ToStringWithHookCall)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId3());
  const HbarTransfer transfer(getTestAccountId(), getTestHbarAmount(), getTestApproval(), hookCall);

  // When
  const std::string result = transfer.toString();

  // Then
  EXPECT_TRUE(result.find("PRE_TX_ALLOWANCE_HOOK") != std::string::npos);
  EXPECT_TRUE(result.find("mHookType") != std::string::npos);
}
