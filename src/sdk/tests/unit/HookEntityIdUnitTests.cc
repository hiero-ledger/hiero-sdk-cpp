// SPDX-License-Identifier: Apache-2.0
#include "ContractId.h"
#include "hooks/HookEntityId.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>

using namespace Hiero;

class HookEntityIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mContractId; }

private:
  const AccountId mAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const ContractId mContractId = ContractId(1ULL, 2ULL, 3ULL);
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

//-----
TEST_F(HookEntityIdUnitTests, GetSetContractId)
{
  // Given
  HookEntityId hookEntityId;

  // When
  EXPECT_NO_THROW(hookEntityId.setContractId(getTestContractId()));

  // Then
  EXPECT_EQ(hookEntityId.getContractId(), getTestContractId());
}

//-----
TEST_F(HookEntityIdUnitTests, FromProtobufWithContractId)
{
  // Given
  proto::HookEntityId protoHookEntityId;
  protoHookEntityId.set_allocated_contract_id(getTestContractId().toProtobuf().release());

  // When
  const HookEntityId hookEntityId = HookEntityId::fromProtobuf(protoHookEntityId);

  // Then
  EXPECT_EQ(hookEntityId.getContractId(), getTestContractId());
}

//-----
TEST_F(HookEntityIdUnitTests, ToProtobufWithContractId)
{
  // Given
  HookEntityId hookEntityId;
  hookEntityId.setContractId(getTestContractId());

  // When
  const std::unique_ptr<proto::HookEntityId> protoHookEntityId = hookEntityId.toProtobuf();

  // Then
  EXPECT_EQ(protoHookEntityId->contract_id().shardnum(), getTestContractId().mShardNum);
  EXPECT_EQ(protoHookEntityId->contract_id().realmnum(), getTestContractId().mRealmNum);
  ASSERT_TRUE(getTestContractId().mContractNum.has_value());
  EXPECT_EQ(static_cast<uint64_t>(protoHookEntityId->contract_id().contractnum()),
            getTestContractId().mContractNum.value());
}

//-----
TEST_F(HookEntityIdUnitTests, SetContractIdResetsAccountId)
{
  // Given
  HookEntityId hookEntityId;
  hookEntityId.setAccountId(getTestAccountId());
  ASSERT_TRUE(hookEntityId.getAccountId().has_value());

  // When
  hookEntityId.setContractId(getTestContractId());

  // Then
  EXPECT_FALSE(hookEntityId.getAccountId().has_value());
  EXPECT_EQ(hookEntityId.getContractId(), getTestContractId());
}

//-----
TEST_F(HookEntityIdUnitTests, SetAccountIdResetsContractId)
{
  // Given
  HookEntityId hookEntityId;
  hookEntityId.setContractId(getTestContractId());
  ASSERT_TRUE(hookEntityId.getContractId().has_value());

  // When
  hookEntityId.setAccountId(getTestAccountId());

  // Then
  EXPECT_FALSE(hookEntityId.getContractId().has_value());
  EXPECT_EQ(hookEntityId.getAccountId(), getTestAccountId());
}
