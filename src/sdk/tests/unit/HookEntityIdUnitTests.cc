// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookEntityId.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class HookEntityIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }

private:
  const AccountId mAccountId = AccountId(1ULL, 2ULL, 3ULL);
};

//-----
TEST_F(HookEntityIdUnitTests, GetSetAccountId)
{
  // Given
  HookEntityId hookEntityId;

  // When
  EXPECT_NO_THROW(hookEntityId.setAccountId(getTestAccountId()));

  // Then
  EXPECT_EQ(hookEntityId.getAccountId(), getTestAccountId());
}

//-----
TEST_F(HookEntityIdUnitTests, FromProtobuf)
{
  // Given
  proto::HookEntityId protoHookEntityId;
  protoHookEntityId.set_allocated_account_id(getTestAccountId().toProtobuf().release());

  // When
  const HookEntityId hookEntityId = HookEntityId::fromProtobuf(protoHookEntityId);

  // Then
  EXPECT_EQ(hookEntityId.getAccountId(), getTestAccountId());
}

//-----
TEST_F(HookEntityIdUnitTests, ToProtobuf)
{
  // Given
  HookEntityId hookEntityId;
  hookEntityId.setAccountId(getTestAccountId());

  // When
  const std::unique_ptr<proto::HookEntityId> protoHookEntityId = hookEntityId.toProtobuf();

  // Then
  EXPECT_EQ(protoHookEntityId->account_id().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(protoHookEntityId->account_id().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(protoHookEntityId->account_id().accountnum(), getTestAccountId().mAccountNum);
}
