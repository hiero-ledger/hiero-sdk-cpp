// SPDX-License-Identifier: Apache-2.0
#include "ContractId.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class ContractIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const uint64_t& getTestShardNum() const { return mShardNum; }
  [[nodiscard]] inline const uint64_t& getTestRealmNum() const { return mRealmNum; }
  [[nodiscard]] inline const uint64_t& getTestContractNum() const { return mContractNum; }
  [[nodiscard]] inline const EvmAddress& getTestEvmAddress() const { return mEvmAddress; }

private:
  const uint64_t mShardNum = 1ULL;
  const uint64_t mRealmNum = 2ULL;
  const uint64_t mContractNum = 3ULL;
  const EvmAddress mEvmAddress = EvmAddress::fromString("0x0123456789abcdef0123456789abcdef01234567");
};

//-----
TEST_F(ContractIdUnitTests, ConstructWithContractNum)
{
  // Given / When
  const ContractId contractId(getTestContractNum());

  // Then
  EXPECT_EQ(contractId.mShardNum, 0ULL);
  EXPECT_EQ(contractId.mRealmNum, 0ULL);
  ASSERT_TRUE(contractId.mContractNum.has_value());
  EXPECT_EQ(contractId.mContractNum, getTestContractNum());
  EXPECT_FALSE(contractId.mEvmAddress.has_value());
}
//-----
TEST_F(ContractIdUnitTests, ConstructWithEvmAddress)
{
  // Given / When
  const ContractId contractId(getTestEvmAddress());

  // Then
  EXPECT_EQ(contractId.mShardNum, 0ULL);
  EXPECT_EQ(contractId.mRealmNum, 0ULL);
  EXPECT_FALSE(contractId.mContractNum.has_value());
  ASSERT_TRUE(contractId.mEvmAddress.has_value());
  EXPECT_EQ(contractId.mEvmAddress->toBytes(), getTestEvmAddress().toBytes());
}

//-----
TEST_F(ContractIdUnitTests, ConstructWithShardRealmContractNum)
{
  // Given / When
  const ContractId contractId(getTestShardNum(), getTestRealmNum(), getTestContractNum());

  // Then
  EXPECT_EQ(contractId.mShardNum, getTestShardNum());
  EXPECT_EQ(contractId.mRealmNum, getTestRealmNum());
  ASSERT_TRUE(contractId.mContractNum.has_value());
  EXPECT_EQ(contractId.mContractNum, getTestContractNum());
  EXPECT_FALSE(contractId.mEvmAddress.has_value());
}

//-----
TEST_F(ContractIdUnitTests, ConstructWithShardRealmEvmAddress)
{
  // Given / When
  const ContractId contractId(getTestShardNum(), getTestRealmNum(), getTestEvmAddress());

  // Then
  EXPECT_EQ(contractId.mShardNum, getTestShardNum());
  EXPECT_EQ(contractId.mRealmNum, getTestRealmNum());
  EXPECT_FALSE(contractId.mContractNum.has_value());
  ASSERT_TRUE(contractId.mEvmAddress.has_value());
  EXPECT_EQ(contractId.mEvmAddress->toBytes(), getTestEvmAddress().toBytes());
}

//-----
TEST_F(ContractIdUnitTests, CompareContractIds)
{
  // Given / When / Then
  EXPECT_TRUE(ContractId() == ContractId());
  EXPECT_TRUE(ContractId(getTestContractNum()) == ContractId(getTestContractNum()));
  EXPECT_TRUE(ContractId(getTestEvmAddress()) == ContractId(getTestEvmAddress()));
  EXPECT_TRUE(ContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()) ==
              ContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()));
  EXPECT_FALSE(ContractId(getTestContractNum()) == ContractId(getTestContractNum() - 1ULL));
  EXPECT_FALSE(ContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()) ==
               ContractId(getTestShardNum() - 1ULL, getTestRealmNum(), getTestContractNum()));
  EXPECT_FALSE(ContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()) ==
               ContractId(getTestShardNum(), getTestRealmNum() - 1ULL, getTestContractNum()));
}

//-----
TEST_F(ContractIdUnitTests, FromProtobuf)
{
  // Given
  proto::ContractID protoContractIdContractNum;
  protoContractIdContractNum.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoContractIdContractNum.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoContractIdContractNum.set_contractnum(static_cast<int64_t>(getTestContractNum()));

  proto::ContractID protoContractIdEvmAddress;
  protoContractIdEvmAddress.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoContractIdEvmAddress.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoContractIdEvmAddress.set_evm_address(internal::Utilities::byteVectorToString(getTestEvmAddress().toBytes()));

  // When
  const ContractId contractIdContractNum = ContractId::fromProtobuf(protoContractIdContractNum);
  const ContractId contractIdEvmAddress = ContractId::fromProtobuf(protoContractIdEvmAddress);

  // Then
  EXPECT_EQ(contractIdContractNum.mShardNum, getTestShardNum());
  EXPECT_EQ(contractIdContractNum.mRealmNum, getTestRealmNum());
  ASSERT_TRUE(contractIdContractNum.mContractNum.has_value());
  EXPECT_EQ(contractIdContractNum.mContractNum, getTestContractNum());
  EXPECT_FALSE(contractIdContractNum.mEvmAddress.has_value());

  EXPECT_EQ(contractIdEvmAddress.mShardNum, getTestShardNum());
  EXPECT_EQ(contractIdEvmAddress.mRealmNum, getTestRealmNum());
  EXPECT_FALSE(contractIdEvmAddress.mContractNum.has_value());
  ASSERT_TRUE(contractIdEvmAddress.mEvmAddress.has_value());
  EXPECT_EQ(contractIdEvmAddress.mEvmAddress->toBytes(), getTestEvmAddress().toBytes());
}

//-----
TEST_F(ContractIdUnitTests, Clone)
{
  // Given
  const ContractId contractIdContractNum(getTestShardNum(), getTestRealmNum(), getTestContractNum());
  const ContractId contractIdEvmAddress(getTestShardNum(), getTestRealmNum(), getTestEvmAddress());

  // When
  const std::unique_ptr<Key> clonedContractIdContractNum = contractIdContractNum.clone();
  const std::unique_ptr<Key> clonedContractIdEvmAddress = contractIdEvmAddress.clone();

  // Then
  EXPECT_EQ(clonedContractIdContractNum->toBytes(), contractIdContractNum.toBytes());
  EXPECT_EQ(clonedContractIdEvmAddress->toBytes(), contractIdEvmAddress.toBytes());
}

//-----
TEST_F(ContractIdUnitTests, ToProtobufKey)
{
  // Given
  const ContractId contractIdContractNum(getTestShardNum(), getTestRealmNum(), getTestContractNum());
  const ContractId contractIdEvmAddress(getTestShardNum(), getTestRealmNum(), getTestEvmAddress());

  // When
  const std::unique_ptr<proto::Key> protoContractIdContractNum = contractIdContractNum.toProtobufKey();
  const std::unique_ptr<proto::Key> protoContractIdEvmAddress = contractIdEvmAddress.toProtobufKey();

  // Then
  ASSERT_TRUE(protoContractIdContractNum->has_contractid());
  ASSERT_TRUE(protoContractIdEvmAddress->has_contractid());

  EXPECT_EQ(protoContractIdContractNum->contractid().shardnum(), static_cast<int64_t>(getTestShardNum()));
  EXPECT_EQ(protoContractIdContractNum->contractid().realmnum(), static_cast<int64_t>(getTestRealmNum()));
  ASSERT_EQ(protoContractIdContractNum->contractid().contract_case(), proto::ContractID::ContractCase::kContractNum);
  EXPECT_EQ(protoContractIdContractNum->contractid().contractnum(), static_cast<int64_t>(getTestContractNum()));

  EXPECT_EQ(protoContractIdEvmAddress->contractid().shardnum(), static_cast<int64_t>(getTestShardNum()));
  EXPECT_EQ(protoContractIdEvmAddress->contractid().realmnum(), static_cast<int64_t>(getTestRealmNum()));
  ASSERT_EQ(protoContractIdEvmAddress->contractid().contract_case(), proto::ContractID::ContractCase::kEvmAddress);
  EXPECT_EQ(protoContractIdEvmAddress->contractid().evm_address(),
            internal::Utilities::byteVectorToString(getTestEvmAddress().toBytes()));
}

//-----
TEST_F(ContractIdUnitTests, ToProtobuf)
{
  // Given
  const ContractId contractIdContractNum(getTestShardNum(), getTestRealmNum(), getTestContractNum());
  const ContractId contractIdEvmAddress(getTestShardNum(), getTestRealmNum(), getTestEvmAddress());

  // When
  const std::unique_ptr<proto::ContractID> protoContractIdContractNum = contractIdContractNum.toProtobuf();
  const std::unique_ptr<proto::ContractID> protoContractIdEvmAddress = contractIdEvmAddress.toProtobuf();

  // Then
  EXPECT_EQ(protoContractIdContractNum->shardnum(), static_cast<int64_t>(getTestShardNum()));
  EXPECT_EQ(protoContractIdContractNum->realmnum(), static_cast<int64_t>(getTestRealmNum()));
  ASSERT_EQ(protoContractIdContractNum->contract_case(), proto::ContractID::ContractCase::kContractNum);
  EXPECT_EQ(protoContractIdContractNum->contractnum(), static_cast<int64_t>(getTestContractNum()));

  EXPECT_EQ(protoContractIdEvmAddress->shardnum(), static_cast<int64_t>(getTestShardNum()));
  EXPECT_EQ(protoContractIdEvmAddress->realmnum(), static_cast<int64_t>(getTestRealmNum()));
  ASSERT_EQ(protoContractIdEvmAddress->contract_case(), proto::ContractID::ContractCase::kEvmAddress);
  EXPECT_EQ(protoContractIdEvmAddress->evm_address(),
            internal::Utilities::byteVectorToString(getTestEvmAddress().toBytes()));
}

//-----
TEST_F(ContractIdUnitTests, ToString)
{
  // Given
  const ContractId contractIdDefault;
  const ContractId contractIdContractNum(getTestContractNum());
  const ContractId contractIdEvmAddress(getTestEvmAddress());
  const ContractId contractIdShardRealmContractNum(getTestShardNum(), getTestRealmNum(), getTestContractNum());
  const ContractId contractIdShardRealmEvmAddress(getTestShardNum(), getTestRealmNum(), getTestEvmAddress());

  // When
  const std::string strContractIdDefault = contractIdDefault.toString();
  const std::string strContractIdContractNum = contractIdContractNum.toString();
  const std::string strContractIdEvmAddress = contractIdEvmAddress.toString();
  const std::string strContractIdShardRealmContractNum = contractIdShardRealmContractNum.toString();
  const std::string strContractIdShardRealmEvmAddress = contractIdShardRealmEvmAddress.toString();

  // Then
  EXPECT_EQ(strContractIdDefault, "0.0.0");
  EXPECT_EQ(strContractIdContractNum, "0.0." + std::to_string(getTestContractNum()));
  EXPECT_EQ(strContractIdEvmAddress, "0.0." + getTestEvmAddress().toString());
  EXPECT_EQ(strContractIdShardRealmContractNum,
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestContractNum()));
  EXPECT_EQ(strContractIdShardRealmEvmAddress,
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              getTestEvmAddress().toString());
}
