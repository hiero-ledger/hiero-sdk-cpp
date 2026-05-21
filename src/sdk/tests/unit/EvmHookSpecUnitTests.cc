// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookSpec.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class EvmHookSpecUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mContractId; }

private:
  const ContractId mContractId = ContractId(1ULL, 2ULL, 3ULL);
};

//-----
TEST_F(EvmHookSpecUnitTests, GetSetContractId)
{
  // Given
  EvmHookSpec evmHookSpec;

  // When
  EXPECT_NO_THROW(evmHookSpec.setContractId(getTestContractId()));

  // Then
  EXPECT_EQ(evmHookSpec.getContractId(), getTestContractId());
}

//-----
TEST_F(EvmHookSpecUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::EvmHookSpec protoEvmHookSpec;
  protoEvmHookSpec.set_allocated_contract_id(getTestContractId().toProtobuf().release());

  // When
  const EvmHookSpec evmHookSpec = EvmHookSpec::fromProtobuf(protoEvmHookSpec);

  // Then
  EXPECT_EQ(evmHookSpec.getContractId(), getTestContractId());
}

//-----
TEST_F(EvmHookSpecUnitTests, ToProtobuf)
{
  // Given
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookSpec> protoEvmHookSpec = evmHookSpec.toProtobuf();

  // Then
  EXPECT_EQ(protoEvmHookSpec->contract_id().shardnum(), getTestContractId().mShardNum);
  EXPECT_EQ(protoEvmHookSpec->contract_id().realmnum(), getTestContractId().mRealmNum);
  EXPECT_EQ(protoEvmHookSpec->contract_id().contractnum(), getTestContractId().mContractNum);
}
