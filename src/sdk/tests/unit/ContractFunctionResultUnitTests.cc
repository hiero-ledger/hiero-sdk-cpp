// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "ContractFunctionResult.h"
#include "ContractId.h"
#include "ContractLogInfo.h"
#include "EvmAddress.h"
#include "Hbar.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <contract_call_local.pb.h>
#include <cstddef>
#include <gtest/gtest.h>
#include <limits>
#include <string>
#include <vector>

using namespace Hiero;

class ContractFunctionResultUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    proto::ContractLoginfo protoContractLogInfo;
    protoContractLogInfo.set_allocated_contractid(mTestContractId.toProtobuf().release());
    protoContractLogInfo.set_bloom(internal::Utilities::byteVectorToString(mTestBloom));
    mTestLogs.push_back(ContractLogInfo::fromProtobuf(protoContractLogInfo));
  }

  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestContractCallResult() const
  {
    return mTestContractCallResult;
  }
  [[nodiscard]] inline const std::string& getTestErrorMessage() const { return mTestErrorMessage; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestBloom() const { return mTestBloom; }
  [[nodiscard]] inline const uint64_t& getTestGasUsed() const { return mTestGasUsed; }
  [[nodiscard]] inline const std::vector<ContractLogInfo>& getTestLogs() const { return mTestLogs; }
  [[nodiscard]] inline const EvmAddress& getTestEvmAddress() const { return mTestEvmAddress; }
  [[nodiscard]] inline const uint64_t& getTestGas() const { return mTestGas; }
  [[nodiscard]] inline const Hbar& getTestAmount() const { return mTestAmount; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestFunctionParameters() const
  {
    return mTestFunctionParameters;
  }
  [[nodiscard]] inline const AccountId& getTestSenderAccountId() const { return mTestSenderAccountId; }
  [[nodiscard]] inline const int64_t& getTestNonce() const { return mTestNonce; }

  [[nodiscard]] std::vector<std::byte> getTestCallResultBytes() const
  {
    return internal::HexConverter::hexToBytes(CALL_RESULT_HEX);
  }
  [[nodiscard]] std::vector<std::byte> getTestStringArrayBytes() const
  {
    return internal::HexConverter::hexToBytes(STRING_ARRAY_RESULT_HEX);
  }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const std::vector<std::byte> mTestContractCallResult = {
    std::byte(0x00), std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04), std::byte(0x05),
    std::byte(0x06), std::byte(0x07), std::byte(0x08), std::byte(0x09), std::byte(0x0A), std::byte(0x0B),
    std::byte(0x0C), std::byte(0x0D), std::byte(0x0E), std::byte(0x0F)
  };
  const std::string mTestErrorMessage = "test error message";
  const std::vector<std::byte> mTestBloom = { std::byte(0x02), std::byte(0x03), std::byte(0x04) };
  const uint64_t mTestGasUsed = 5ULL;
  std::vector<ContractLogInfo> mTestLogs;
  const EvmAddress mTestEvmAddress = EvmAddress::fromString("0x0123456789abcdef0123456789abcdef01234567");
  const uint64_t mTestGas = 6ULL;
  const Hbar mTestAmount = Hbar(7LL);
  const std::vector<std::byte> mTestFunctionParameters = { std::byte(0x08), std::byte(0x09), std::byte(0x0A) };
  const AccountId mTestSenderAccountId = AccountId(11ULL);
  const int64_t mTestNonce = 10L;

  const std::string CALL_RESULT_HEX = "00000000000000000000000000000000000000000000000000000000ffffffff"
                                      "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                                      "00000000000000000000000011223344556677889900aabbccddeeff00112233"
                                      "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
                                      "00000000000000000000000000000000000000000000000000000000000000c0"
                                      "0000000000000000000000000000000000000000000000000000000000000100"
                                      "000000000000000000000000000000000000000000000000000000000000000d"
                                      "48656c6c6f2c20776f726c642100000000000000000000000000000000000000"
                                      "0000000000000000000000000000000000000000000000000000000000000014"
                                      "48656c6c6f2c20776f726c642c20616761696e21000000000000000000000000";

  const std::string STRING_ARRAY_RESULT_HEX = "0000000000000000000000000000000000000000000000000000000000000020"
                                              "0000000000000000000000000000000000000000000000000000000000000002"
                                              "0000000000000000000000000000000000000000000000000000000000000040"
                                              "0000000000000000000000000000000000000000000000000000000000000080"
                                              "000000000000000000000000000000000000000000000000000000000000000C"
                                              "72616E646F6D2062797465730000000000000000000000000000000000000000"
                                              "000000000000000000000000000000000000000000000000000000000000000C"
                                              "72616E646F6D2062797465730000000000000000000000000000000000000000";
};

//-----
TEST_F(ContractFunctionResultUnitTests, FromProtobuf)
{
  // Given
  proto::ContractFunctionResult protoContractFunctionResult;
  protoContractFunctionResult.set_allocated_contractid(getTestContractId().toProtobuf().release());
  protoContractFunctionResult.set_contractcallresult(
    internal::Utilities::byteVectorToString(getTestContractCallResult()));
  protoContractFunctionResult.set_errormessage(getTestErrorMessage());
  protoContractFunctionResult.set_bloom(internal::Utilities::byteVectorToString(getTestBloom()));
  protoContractFunctionResult.set_gasused(getTestGasUsed());

  for (const auto& log : getTestLogs())
  {
    protoContractFunctionResult.mutable_loginfo()->AddAllocated(log.toProtobuf().release());
  }

  auto bytesValue = std::make_unique<google::protobuf::BytesValue>();
  bytesValue->set_value(internal::Utilities::byteVectorToString(getTestEvmAddress().toBytes()));
  protoContractFunctionResult.set_allocated_evm_address(bytesValue.release());

  protoContractFunctionResult.set_gasused(getTestGasUsed());
  protoContractFunctionResult.set_amount(getTestAmount().toTinybars());
  protoContractFunctionResult.set_functionparameters(
    internal::Utilities::byteVectorToString(getTestFunctionParameters()));
  protoContractFunctionResult.set_allocated_sender_id(getTestSenderAccountId().toProtobuf().release());

  auto protoContractNonceInfo = protoContractFunctionResult.add_contract_nonces();
  protoContractNonceInfo->set_allocated_contract_id(getTestContractId().toProtobuf().release());
  protoContractNonceInfo->set_nonce(getTestNonce());

  // When
  const ContractFunctionResult contractFunctionResult =
    ContractFunctionResult::fromProtobuf(protoContractFunctionResult);

  // Then
  EXPECT_EQ(contractFunctionResult.mContractId, getTestContractId());
  EXPECT_EQ(contractFunctionResult.mContractCallResult, getTestContractCallResult());
  EXPECT_EQ(contractFunctionResult.mErrorMessage, getTestErrorMessage());
  EXPECT_EQ(contractFunctionResult.mBloom, getTestBloom());
  EXPECT_EQ(contractFunctionResult.mGasUsed, getTestGasUsed());
  // No need to check logs equality here since that's already done in ContractLogInfoTest.cc
  EXPECT_EQ(contractFunctionResult.mLogs.size(), getTestLogs().size());
  ASSERT_TRUE(contractFunctionResult.mEvmAddress.has_value());
  EXPECT_EQ(contractFunctionResult.mEvmAddress->toBytes(), getTestEvmAddress().toBytes());
  EXPECT_EQ(contractFunctionResult.mGasUsed, getTestGasUsed());
  EXPECT_EQ(contractFunctionResult.mHbarAmount, getTestAmount());
  EXPECT_EQ(contractFunctionResult.mFunctionParameters, getTestFunctionParameters());
  EXPECT_EQ(contractFunctionResult.mSenderAccountId, getTestSenderAccountId());
  EXPECT_EQ(contractFunctionResult.mContractNonces.size(), 1);
  EXPECT_EQ(contractFunctionResult.mContractNonces.front().mContractId, getTestContractId());
  EXPECT_EQ(contractFunctionResult.mContractNonces.front().mNonce, getTestNonce());
}

//-----
TEST_F(ContractFunctionResultUnitTests, GetResults)
{
  // Given
  ContractFunctionResult contractFunctionResult;
  contractFunctionResult.mContractCallResult = getTestCallResultBytes();

  // When / Then
  EXPECT_TRUE(contractFunctionResult.getBool(0));
  EXPECT_EQ(contractFunctionResult.getInt32(0), -1);
  EXPECT_EQ(contractFunctionResult.getInt64(0), 4294967295);
  EXPECT_EQ(contractFunctionResult.getAddress(2), "11223344556677889900AABBCCDDEEFF00112233");
  EXPECT_EQ(contractFunctionResult.getUint32(3), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(contractFunctionResult.getUint64(3), std::numeric_limits<uint64_t>::max());
  EXPECT_EQ(contractFunctionResult.getString(4), "Hello, world!");
  EXPECT_EQ(contractFunctionResult.getString(5), "Hello, world, again!");
}

//-----
TEST_F(ContractFunctionResultUnitTests, GetStringArray)
{
  // Given
  ContractFunctionResult contractFunctionResult;
  contractFunctionResult.mContractCallResult = getTestStringArrayBytes();

  // When
  const std::vector<std::string> strings = contractFunctionResult.getStringArray(0);

  // Then
  ASSERT_EQ(strings.size(), 2);
  for (const auto& str : strings)
  {
    EXPECT_EQ(str, "random bytes");
  }
}
