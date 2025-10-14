// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookCall.h"
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
TEST_F(HookCallUnitTests, GetSetFullHookId)
{
  // Given
  HookCall hookCall;

  // When
  EXPECT_NO_THROW(hookCall.setFullHookId(getTestFullHookId()));

  // Then
  EXPECT_TRUE(hookCall.getFullHookId()->getEntityId().getAccountId().has_value());
  EXPECT_EQ(hookCall.getFullHookId()->getEntityId().getAccountId().value(), getTestAccountId());
  EXPECT_EQ(hookCall.getFullHookId()->getHookId(), getTestHookId());
}

//-----
TEST_F(HookCallUnitTests, GetSetFullHookIdResetsHookId)
{
  // Given
  HookCall hookCall;

  // When
  EXPECT_NO_THROW(hookCall.setHookId(getTestHookId()));
  EXPECT_NO_THROW(hookCall.setFullHookId(getTestFullHookId()));

  // Then
  EXPECT_FALSE(hookCall.getHookId().has_value());
}

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
TEST_F(HookCallUnitTests, GetSetHookIdResetsFullHookId)
{
  // Given
  HookCall hookCall;

  // When
  EXPECT_NO_THROW(hookCall.setFullHookId(getTestFullHookId()));
  EXPECT_NO_THROW(hookCall.setHookId(getTestHookId()));

  // Then
  EXPECT_FALSE(hookCall.getFullHookId().has_value());
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
TEST_F(HookCallUnitTests, FromProtobuf)
{
  // Given
  proto::HookCall protoHookCallFullHookId;
  proto::HookCall protoHookCallHookId;

  protoHookCallFullHookId.set_allocated_full_hook_id(getTestFullHookId().toProtobuf().release());
  protoHookCallFullHookId.set_allocated_evm_hook_call(getTestEvmHookCall().toProtobuf().release());

  protoHookCallHookId.set_hook_id(getTestHookId());

  // When
  const HookCall hookCallFullHookId = HookCall::fromProtobuf(protoHookCallFullHookId);
  const HookCall hookCallHookId = HookCall::fromProtobuf(protoHookCallHookId);

  // Then
  EXPECT_TRUE(hookCallFullHookId.getFullHookId()->getEntityId().getAccountId().has_value());
  EXPECT_EQ(hookCallFullHookId.getFullHookId()->getEntityId().getAccountId().value(), getTestAccountId());
  EXPECT_EQ(hookCallFullHookId.getFullHookId()->getHookId(), getTestHookId());

  EXPECT_TRUE(hookCallFullHookId.getEvmHookCall().has_value());
  EXPECT_EQ(hookCallFullHookId.getEvmHookCall()->getData(), getTestCallData());
  EXPECT_EQ(hookCallFullHookId.getEvmHookCall()->getGasLimit(), getTestGasLimit());

  EXPECT_EQ(hookCallHookId.getHookId(), getTestHookId());
}

//-----
TEST_F(HookCallUnitTests, ToProtobuf)
{
  // Given
  HookCall hookCallFullHookId;
  HookCall hookCallHookId;

  hookCallFullHookId.setFullHookId(getTestFullHookId());
  hookCallFullHookId.setEvmHookCall(getTestEvmHookCall());

  hookCallHookId.setHookId(getTestHookId());

  // When
  const std::unique_ptr<proto::HookCall> protoHookCallFullHookId = hookCallFullHookId.toProtobuf();
  const std::unique_ptr<proto::HookCall> protoHookCallHookId = hookCallHookId.toProtobuf();

  // Then
  EXPECT_TRUE(protoHookCallFullHookId->has_full_hook_id());
  EXPECT_TRUE(protoHookCallFullHookId->full_hook_id().has_entity_id());
  EXPECT_TRUE(protoHookCallFullHookId->full_hook_id().entity_id().has_account_id());
  EXPECT_EQ(protoHookCallFullHookId->full_hook_id().entity_id().account_id().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(protoHookCallFullHookId->full_hook_id().entity_id().account_id().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(protoHookCallFullHookId->full_hook_id().entity_id().account_id().accountnum(),
            getTestAccountId().mAccountNum);
  EXPECT_EQ(protoHookCallFullHookId->full_hook_id().hook_id(), getTestHookId());
  EXPECT_TRUE(protoHookCallFullHookId->has_evm_hook_call());
  EXPECT_EQ(protoHookCallFullHookId->evm_hook_call().data(),
            internal::Utilities::byteVectorToString(getTestCallData()));
  EXPECT_EQ(protoHookCallFullHookId->evm_hook_call().gas_limit(), getTestGasLimit());

  EXPECT_TRUE(protoHookCallHookId->has_hook_id());
  EXPECT_FALSE(protoHookCallHookId->has_full_hook_id());
  EXPECT_FALSE(protoHookCallHookId->has_evm_hook_call());
  EXPECT_EQ(protoHookCallHookId->hook_id(), getTestHookId());
}
