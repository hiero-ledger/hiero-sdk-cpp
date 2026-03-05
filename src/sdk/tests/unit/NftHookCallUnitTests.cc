// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "hooks/HookCall.h"
#include "hooks/HookEntityId.h"
#include "hooks/HookId.h"
#include "hooks/NftHookCall.h"
#include "hooks/NftHookType.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class NftHookCallUnitTests : public ::testing::Test
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
  [[nodiscard]] inline NftHookType getTestHookType() const { return mHookType; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestCallData1() const { return mCallData1; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestCallData2() const { return mCallData2; }
  [[nodiscard]] inline uint64_t getTestGasLimit1() const { return mGasLimit1; }
  [[nodiscard]] inline uint64_t getTestGasLimit2() const { return mGasLimit2; }
  [[nodiscard]] inline int64_t getTestZeroHookId() const { return mZeroHookId; }

private:
  const AccountId mAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const int64_t mHookIdValue = 4LL;
  const int64_t mZeroHookId = 0LL;
  const NftHookType mHookType = NftHookType::PRE_HOOK;
  const std::vector<std::byte> mCallData1 = { std::byte(0x12), std::byte(0x34) };
  const std::vector<std::byte> mCallData2 = { std::byte(0x56), std::byte(0x78) };
  const uint64_t mGasLimit1 = 1000ULL;
  const uint64_t mGasLimit2 = 2000ULL;

  HookEntityId mHookEntityId;
  HookId mHookId;
};

//-----
TEST_F(NftHookCallUnitTests, DefaultConstruction)
{
  // Given / When
  const NftHookCall hookCall;

  // Then
  EXPECT_EQ(hookCall.getHookType(), NftHookType::UNINITIALIZED);
  EXPECT_EQ(hookCall.getHookId(), getTestZeroHookId());
  EXPECT_FALSE(hookCall.getEvmHookCall().has_value());
}

//-----
TEST_F(NftHookCallUnitTests, SetHookType)
{
  // Given
  NftHookCall hookCall;

  // When
  hookCall.setHookType(getTestHookType());

  // Then
  EXPECT_EQ(hookCall.getHookType(), getTestHookType());
}

//-----
TEST_F(NftHookCallUnitTests, SetHookId)
{
  // Given
  NftHookCall hookCall;

  // When
  hookCall.setHookId(getTestHookId());

  // Then
  EXPECT_EQ(hookCall.getHookId(), getTestHookId());
}

//-----
TEST_F(NftHookCallUnitTests, SetEvmHookCall)
{
  // Given
  NftHookCall hookCall;
  EvmHookCall evmHookCall;
  evmHookCall.setData(getTestCallData1());
  evmHookCall.setGasLimit(getTestGasLimit1());

  // When
  hookCall.setEvmHookCall(evmHookCall);

  // Then
  EXPECT_TRUE(hookCall.getEvmHookCall().has_value());
  EXPECT_EQ(hookCall.getEvmHookCall()->getData().size(), getTestCallData1().size());
  EXPECT_EQ(hookCall.getEvmHookCall()->getGasLimit(), getTestGasLimit1());
}

//-----
TEST_F(NftHookCallUnitTests, FromProtobufPreHook)
{
  // Given
  proto::HookCall protoHookCall;
  protoHookCall.set_hook_id(getTestHookId());

  // When
  const NftHookCall hookCall = NftHookCall::fromProtobuf(protoHookCall, NftHookType::PRE_HOOK);

  // Then
  EXPECT_EQ(hookCall.getHookType(), NftHookType::PRE_HOOK);
  EXPECT_EQ(hookCall.getHookId(), getTestHookId());
}

//-----
TEST_F(NftHookCallUnitTests, FromProtobufPrePostHook)
{
  // Given
  proto::HookCall protoHookCall;
  protoHookCall.set_hook_id(getTestHookId());

  // When
  const NftHookCall hookCall = NftHookCall::fromProtobuf(protoHookCall, NftHookType::PRE_POST_HOOK);

  // Then
  EXPECT_EQ(hookCall.getHookType(), NftHookType::PRE_POST_HOOK);
  EXPECT_EQ(hookCall.getHookId(), getTestHookId());
}

//-----
TEST_F(NftHookCallUnitTests, ToProtobufPreHook)
{
  // Given
  NftHookCall hookCall;
  hookCall.setHookType(NftHookType::PRE_HOOK);
  hookCall.setHookId(getTestHookId());

  // When
  const std::unique_ptr<proto::HookCall> protoHookCall = hookCall.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCall->hook_id(), getTestHookId());
  EXPECT_FALSE(protoHookCall->has_evm_hook_call());
}

//-----
TEST_F(NftHookCallUnitTests, ToProtobufPrePostHook)
{
  // Given
  NftHookCall hookCall;
  hookCall.setHookType(NftHookType::PRE_POST_HOOK);
  hookCall.setHookId(getTestHookId());

  // When
  const std::unique_ptr<proto::HookCall> protoHookCall = hookCall.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCall->hook_id(), getTestHookId());
  EXPECT_FALSE(protoHookCall->has_evm_hook_call());
}

//-----
TEST_F(NftHookCallUnitTests, ToProtobufWithEvmHookCall)
{
  // Given
  EvmHookCall evmHookCall;
  evmHookCall.setData(getTestCallData2());
  evmHookCall.setGasLimit(getTestGasLimit2());

  NftHookCall hookCall;
  hookCall.setHookType(NftHookType::PRE_HOOK);
  hookCall.setHookId(getTestHookId());
  hookCall.setEvmHookCall(evmHookCall);

  // When
  const std::unique_ptr<proto::HookCall> protoHookCall = hookCall.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCall->hook_id(), getTestHookId());
  EXPECT_TRUE(protoHookCall->has_evm_hook_call());
  EXPECT_EQ(protoHookCall->evm_hook_call().data().size(), getTestCallData2().size());
  EXPECT_EQ(protoHookCall->evm_hook_call().gas_limit(), getTestGasLimit2());
}