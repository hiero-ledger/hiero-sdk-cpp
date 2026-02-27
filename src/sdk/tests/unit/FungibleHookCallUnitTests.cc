// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "hooks/FungibleHookCall.h"
#include "hooks/FungibleHookType.h"
#include "hooks/HookCall.h"
#include "hooks/HookEntityId.h"
#include "hooks/HookId.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class FungibleHookCallUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mHookEntityId.setAccountId(mAccountId);
    mHookId.setEntityId(mHookEntityId);
    mHookId.setHookId(mHookIdValue);
  }

  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline int64_t getTestHookId() const { return mHookIdValue; }
  [[nodiscard]] inline const HookEntityId& getTestHookEntityId() const { return mHookEntityId; }
  [[nodiscard]] inline const HookId& getTestHookIdObject() const { return mHookId; }
  [[nodiscard]] inline FungibleHookType getTestHookType() const { return mHookType; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestCallData() const { return mCallData; }
  [[nodiscard]] inline uint64_t getTestGasLimit() const { return mGasLimit; }
  [[nodiscard]] inline int64_t getTestZeroHookId() const { return mZeroHookId; }

private:
  const AccountId mAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const int64_t mHookIdValue = 4LL;
  const int64_t mZeroHookId = 0LL;
  const FungibleHookType mHookType = FungibleHookType::PRE_TX_ALLOWANCE_HOOK;
  const std::vector<std::byte> mCallData = { std::byte(0x12), std::byte(0x34) };
  const uint64_t mGasLimit = 1000ULL;

  HookEntityId mHookEntityId;
  HookId mHookId;
};

//-----
TEST_F(FungibleHookCallUnitTests, DefaultConstruction)
{
  // Given / When
  const FungibleHookCall hookCall;

  // Then
  EXPECT_EQ(hookCall.getHookType(), FungibleHookType::UNINITIALIZED);
  EXPECT_EQ(hookCall.getHookId(), getTestZeroHookId());
  EXPECT_FALSE(hookCall.getEvmHookCall().has_value());
}

//-----
TEST_F(FungibleHookCallUnitTests, SetGetHookType)
{
  // Given
  FungibleHookCall hookCall;

  // When
  hookCall.setHookType(getTestHookType());

  // Then
  EXPECT_EQ(hookCall.getHookType(), getTestHookType());
}

//-----
TEST_F(FungibleHookCallUnitTests, SetGetHookId)
{
  // Given
  FungibleHookCall hookCall;

  // When
  hookCall.setHookId(getTestHookId());

  // Then
  EXPECT_EQ(hookCall.getHookId(), getTestHookId());
}

//-----
TEST_F(FungibleHookCallUnitTests, SetGetEvmHookCall)
{
  // Given
  FungibleHookCall hookCall;
  EvmHookCall evmHookCall;
  evmHookCall.setData(getTestCallData());
  evmHookCall.setGasLimit(getTestGasLimit());

  // When
  hookCall.setEvmHookCall(evmHookCall);

  // Then
  EXPECT_TRUE(hookCall.getEvmHookCall().has_value());
  EXPECT_EQ(hookCall.getEvmHookCall()->getData().size(), getTestCallData().size());
  EXPECT_EQ(hookCall.getEvmHookCall()->getGasLimit(), getTestGasLimit());
}

//-----
TEST_F(FungibleHookCallUnitTests, FromProtobufPreTxAllowanceHook)
{
  // Given
  proto::HookCall protoHookCall;
  protoHookCall.set_hook_id(getTestHookId());

  // When
  const FungibleHookCall hookCall =
    FungibleHookCall::fromProtobuf(protoHookCall, FungibleHookType::PRE_TX_ALLOWANCE_HOOK);

  // Then
  EXPECT_EQ(hookCall.getHookType(), FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  EXPECT_EQ(hookCall.getHookId(), getTestHookId());
}

//-----
TEST_F(FungibleHookCallUnitTests, FromProtobufPrePostTxAllowanceHook)
{
  // Given
  proto::HookCall protoHookCall;
  protoHookCall.set_hook_id(getTestHookId());

  // When
  const FungibleHookCall hookCall =
    FungibleHookCall::fromProtobuf(protoHookCall, FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);

  // Then
  EXPECT_EQ(hookCall.getHookType(), FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  EXPECT_EQ(hookCall.getHookId(), getTestHookId());
}

//-----
TEST_F(FungibleHookCallUnitTests, ToProtobufPreTxAllowanceHook)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId());

  // When
  const std::unique_ptr<proto::HookCall> protoHookCall = hookCall.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCall->hook_id(), getTestHookId());
  EXPECT_FALSE(protoHookCall->has_evm_hook_call());
}

//-----
TEST_F(FungibleHookCallUnitTests, ToProtobufPrePostTxAllowanceHook)
{
  // Given
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId());

  // When
  const std::unique_ptr<proto::HookCall> protoHookCall = hookCall.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCall->hook_id(), getTestHookId());
  EXPECT_FALSE(protoHookCall->has_evm_hook_call());
}

//-----
TEST_F(FungibleHookCallUnitTests, ToProtobufWithEvmHookCall)
{
  // Given
  EvmHookCall evmHookCall;
  evmHookCall.setData(getTestCallData());
  evmHookCall.setGasLimit(getTestGasLimit());

  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId());
  hookCall.setEvmHookCall(evmHookCall);

  // When
  const std::unique_ptr<proto::HookCall> protoHookCall = hookCall.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCall->hook_id(), getTestHookId());
  EXPECT_TRUE(protoHookCall->has_evm_hook_call());
  EXPECT_EQ(protoHookCall->evm_hook_call().data().size(), getTestCallData().size());
  EXPECT_EQ(protoHookCall->evm_hook_call().gas_limit(), getTestGasLimit());
}