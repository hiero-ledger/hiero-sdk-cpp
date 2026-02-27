// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "hooks/HookCall.h"
#include "hooks/HookEntityId.h"
#include "hooks/HookId.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class HookCallUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mHookEntityId.setAccountId(mAccountId);
    mFullHookId.setEntityId(mHookEntityId);
    mFullHookId.setHookId(mHookId);
    mEvmHookCall.setData(mCallData);
    mEvmHookCall.setGasLimit(mGasLimit);
  }

  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline int64_t getTestHookId() const { return mHookId; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestCallData() const { return mCallData; }
  [[nodiscard]] inline uint64_t getTestGasLimit() const { return mGasLimit; }
  [[nodiscard]] inline const HookEntityId& getTestHookEntityId() const { return mHookEntityId; }
  [[nodiscard]] inline const HookId& getTestFullHookId() const { return mFullHookId; }
  [[nodiscard]] inline const EvmHookCall& getTestEvmHookCall() const { return mEvmHookCall; }

private:
  const AccountId mAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const int64_t mHookId = 4LL;
  const std::vector<std::byte> mCallData = { std::byte(0x56), std::byte(0x78), std::byte(0x9A) };
  const uint64_t mGasLimit = 11ULL;

  HookEntityId mHookEntityId;
  HookId mFullHookId;
  EvmHookCall mEvmHookCall;
};

//-----
TEST_F(HookCallUnitTests, GetSetHookId)
{
  // Given
  HookCall hookCall;

  // When
  EXPECT_NO_THROW(hookCall.setHookId(getTestHookId()));

  // Then
  EXPECT_EQ(hookCall.getHookId(), getTestHookId());
}

//-----
TEST_F(HookCallUnitTests, GetSetEvmHookCall)
{
  // Given
  HookCall hookCall;

  // When
  EXPECT_NO_THROW(hookCall.setEvmHookCall(getTestEvmHookCall()));

  // Then
  EXPECT_TRUE(hookCall.getEvmHookCall().has_value());
  EXPECT_EQ(hookCall.getEvmHookCall()->getData(), getTestCallData());
  EXPECT_EQ(hookCall.getEvmHookCall()->getGasLimit(), getTestGasLimit());
}

//-----
TEST_F(HookCallUnitTests, ToProtobuf)
{
  // Given
  HookCall hookCall;

  hookCall.setEvmHookCall(getTestEvmHookCall());
  hookCall.setHookId(getTestHookId());

  // When
  const std::unique_ptr<proto::HookCall> protoHookCall = hookCall.toProtobuf();

  // Then
  EXPECT_TRUE(protoHookCall->has_hook_id());
  EXPECT_EQ(protoHookCall->hook_id(), getTestHookId());
  EXPECT_EQ(protoHookCall->evm_hook_call().data(), internal::Utilities::byteVectorToString(getTestCallData()));
  EXPECT_EQ(protoHookCall->evm_hook_call().gas_limit(), getTestGasLimit());
}
