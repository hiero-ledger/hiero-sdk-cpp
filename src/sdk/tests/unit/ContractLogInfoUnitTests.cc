// SPDX-License-Identifier: Apache-2.0
#include "ContractId.h"
#include "ContractLogInfo.h"
#include "impl/Utilities.h"

#include <contract_call_local.pb.h>
#include <cstddef>
#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class ContractLogInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestBloom() const { return mTestBloom; }
  [[nodiscard]] inline const std::vector<std::vector<std::byte>>& getTestTopics() const { return mTestTopics; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestData() const { return mTestBloom; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const std::vector<std::byte> mTestBloom = { std::byte(0x02), std::byte(0x03), std::byte(0x04) };
  const std::vector<std::vector<std::byte>> mTestTopics = {
    {std::byte(0x05),  std::byte(0x06), std::byte(0x07)},
    { std::byte(0x08), std::byte(0x09), std::byte(0x0A)},
    { std::byte(0x0B), std::byte(0x0C), std::byte(0x0D)}
  };
  const std::vector<std::byte> mTestData = { std::byte(0x0E), std::byte(0x0F) };
};

//-----
TEST_F(ContractLogInfoUnitTests, FromProtobuf)
{
  // Given
  proto::ContractLoginfo protoContractLogInfo;
  protoContractLogInfo.set_allocated_contractid(getTestContractId().toProtobuf().release());
  protoContractLogInfo.set_bloom(internal::Utilities::byteVectorToString(getTestBloom()));

  for (const auto& topic : getTestTopics())
  {
    protoContractLogInfo.add_topic(internal::Utilities::byteVectorToString(topic));
  }

  protoContractLogInfo.set_data(internal::Utilities::byteVectorToString(getTestData()));

  // When
  const ContractLogInfo contractLogInfo = ContractLogInfo::fromProtobuf(protoContractLogInfo);

  // Then
  EXPECT_EQ(contractLogInfo.mContractId, getTestContractId());

  ASSERT_EQ(contractLogInfo.mBloom.size(), getTestBloom().size());
  for (int i = 0; i < contractLogInfo.mBloom.size(); ++i)
  {
    EXPECT_EQ(contractLogInfo.mBloom.at(i), getTestBloom().at(i));
  }

  ASSERT_EQ(contractLogInfo.mTopics.size(), getTestTopics().size());
  for (int i = 0; i < contractLogInfo.mTopics.size(); ++i)
  {
    ASSERT_EQ(contractLogInfo.mTopics.at(i).size(), getTestTopics().at(i).size());
    for (int j = 0; j < contractLogInfo.mTopics.at(i).size(); ++j)
    {
      EXPECT_EQ(contractLogInfo.mTopics.at(i).at(j), getTestTopics().at(i).at(j));
    }
  }

  ASSERT_EQ(contractLogInfo.mData.size(), getTestData().size());
  for (int i = 0; i < contractLogInfo.mData.size(); ++i)
  {
    EXPECT_EQ(contractLogInfo.mData.at(i), getTestData().at(i));
  }
}

//-----
TEST_F(ContractLogInfoUnitTests, ToProtobuf)
{
  // Given
  ContractLogInfo contractLogInfo;
  contractLogInfo.mContractId = getTestContractId();
  contractLogInfo.mBloom = getTestBloom();
  contractLogInfo.mTopics = getTestTopics();
  contractLogInfo.mData = getTestData();

  // When
  const std::unique_ptr<proto::ContractLoginfo> protoContractLogInfo = contractLogInfo.toProtobuf();

  // Then
  EXPECT_EQ(ContractId::fromProtobuf(protoContractLogInfo->contractid()), getTestContractId());
  EXPECT_EQ(protoContractLogInfo->bloom(), internal::Utilities::byteVectorToString(getTestBloom()));

  ASSERT_EQ(protoContractLogInfo->topic_size(), getTestTopics().size());
  for (int i = 0; i < protoContractLogInfo->topic_size(); ++i)
  {
    EXPECT_EQ(protoContractLogInfo->topic(i), internal::Utilities::byteVectorToString(getTestTopics().at(i)));
  }

  EXPECT_EQ(protoContractLogInfo->data(), internal::Utilities::byteVectorToString(getTestData()));
}
