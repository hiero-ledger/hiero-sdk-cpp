// SPDX-License-Identifier: Apache-2.0
#include "ContractId.h"
#include "ContractNonceInfo.h"
#include "impl/Utilities.h"

#include <contract_types.pb.h>
#include <cstddef>
#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class ContractNonceInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const int64_t& getTestNonce() const { return mTestNonce; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const int64_t mTestNonce = 2L;
};

//-----
TEST_F(ContractNonceInfoUnitTests, FromProtobuf)
{
  // Given
  proto::ContractNonceInfo protoContractNonceInfo;
  protoContractNonceInfo.set_allocated_contract_id(getTestContractId().toProtobuf().release());
  protoContractNonceInfo.set_nonce(getTestNonce());

  // When
  const ContractNonceInfo contractNonceInfo = ContractNonceInfo::fromProtobuf(protoContractNonceInfo);

  // Then
  EXPECT_EQ(contractNonceInfo.mContractId, getTestContractId());
  EXPECT_EQ(contractNonceInfo.mNonce, getTestNonce());
}

//-----
TEST_F(ContractNonceInfoUnitTests, ToProtobuf)
{
  // Given
  const ContractNonceInfo testContractNonceInfo = ContractNonceInfo(getTestContractId(), getTestNonce());

  // When
  const std::unique_ptr<proto::ContractNonceInfo> protoContractNonceInfo = testContractNonceInfo.toProtobuf();

  // Then
  EXPECT_EQ(protoContractNonceInfo.get()->contract_id().shardnum(),
            static_cast<int64_t>(getTestContractId().mShardNum));
  EXPECT_EQ(protoContractNonceInfo.get()->contract_id().realmnum(),
            static_cast<int64_t>(getTestContractId().mRealmNum));
  ASSERT_EQ(protoContractNonceInfo.get()->contract_id().contract_case(), proto::ContractID::ContractCase::kContractNum);
  EXPECT_EQ(protoContractNonceInfo.get()->contract_id().contractnum(),
            static_cast<int64_t>(getTestContractId().mContractNum.value()));
  EXPECT_EQ(protoContractNonceInfo.get()->nonce(), getTestNonce());
}

//-----
TEST_F(ContractNonceInfoUnitTests, ToBytes)
{
  // Given
  const ContractNonceInfo testContractNonceInfo = ContractNonceInfo(getTestContractId(), getTestNonce());

  // When
  const std::vector<std::byte> bytes = testContractNonceInfo.toBytes();

  // Then
  EXPECT_EQ(bytes, internal::Utilities::stringToByteVector(testContractNonceInfo.toProtobuf()->SerializeAsString()));
}

//-----
TEST_F(ContractNonceInfoUnitTests, FromBytes)
{
  // Given
  proto::ContractNonceInfo protoContractNonceInfo;
  protoContractNonceInfo.set_allocated_contract_id(getTestContractId().toProtobuf().release());
  protoContractNonceInfo.set_nonce(getTestNonce());

  // When
  ContractNonceInfo contractNonceInfo =
    ContractNonceInfo::fromBytes(internal::Utilities::stringToByteVector(protoContractNonceInfo.SerializeAsString()));

  // Then
  EXPECT_EQ(contractNonceInfo.mContractId.mShardNum, getTestContractId().mShardNum);
  EXPECT_EQ(contractNonceInfo.mContractId.mRealmNum, getTestContractId().mRealmNum);
  EXPECT_EQ(contractNonceInfo.mContractId.mContractNum, getTestContractId().mContractNum);
  EXPECT_EQ(contractNonceInfo.mNonce, getTestNonce());
}
