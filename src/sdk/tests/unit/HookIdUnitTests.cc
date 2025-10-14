// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookId.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class HookIdUnitTests : public ::testing::Test
{
protected:
  void SetUp() override { mHookEntityId.setAccountId(mAccountId); }

  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline int64_t getTestHookId() const { return mHookId; }
  [[nodiscard]] inline const HookEntityId& getTestHookEntityId() const { return mHookEntityId; }

private:
  const AccountId mAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const int64_t mHookId = 4LL;

  HookEntityId mHookEntityId;
};

//-----
TEST_F(HookIdUnitTests, GetSetEntityId)
{
  // Given
  HookId hookId;

  // When
  EXPECT_NO_THROW(hookId.setEntityId(getTestHookEntityId()));

  // Then
  EXPECT_TRUE(hookId.getEntityId().getAccountId().has_value());
  EXPECT_EQ(hookId.getEntityId().getAccountId().value(), getTestAccountId());
}

//-----
TEST_F(HookIdUnitTests, GetSetHookId)
{
  // Given
  HookId hookId;

  // When
  EXPECT_NO_THROW(hookId.setHookId(getTestHookId()));

  // Then
  EXPECT_EQ(hookId.getHookId(), getTestHookId());
}

//-----
TEST_F(HookIdUnitTests, FromProtobuf)
{
  // Given
  proto::HookId protoHookId;
  protoHookId.set_allocated_entity_id(getTestHookEntityId().toProtobuf().release());
  protoHookId.set_hook_id(getTestHookId());

  // When
  const HookId hookId = HookId::fromProtobuf(protoHookId);

  // Then
  EXPECT_TRUE(hookId.getEntityId().getAccountId().has_value());
  EXPECT_EQ(hookId.getEntityId().getAccountId().value(), getTestAccountId());
  EXPECT_EQ(hookId.getHookId(), getTestHookId());
}

//-----
TEST_F(HookIdUnitTests, ToProtobuf)
{
  // Given
  HookId hookId;
  hookId.setEntityId(getTestHookEntityId());
  hookId.setHookId(getTestHookId());

  // When
  const std::unique_ptr<proto::HookId> protoHookId = hookId.toProtobuf();

  // Then
  EXPECT_TRUE(protoHookId->has_entity_id());
  EXPECT_TRUE(protoHookId->entity_id().has_account_id());
  EXPECT_EQ(protoHookId->entity_id().account_id().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(protoHookId->entity_id().account_id().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(protoHookId->entity_id().account_id().accountnum(), getTestAccountId().mAccountNum);
}
