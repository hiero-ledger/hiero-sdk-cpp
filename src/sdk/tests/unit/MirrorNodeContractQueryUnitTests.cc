// SPDX-License-Identifier: Apache-2.0
#include "ContractId.h"
#include "impl/MirrorNodeContractEstimateGasQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class MirrorNodeContractQueryUnitTests : public ::testing::Test
{
protected:
  MirrorNodeContractEstimateGasQuery query;
};

//-----

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetContractId)
{
  // Given
  ContractId contractId(1, 2, 3);

  // When
  query.setContractId(contractId);

  // Then
  EXPECT_EQ(query.getContractId().value(), contractId);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetContractEvmAddress)
{
  // Given
  std::string evmAddress = "0x1234567890abcdef1234567890abcdef12345";

  // When
  query.setContractEvmAddress(evmAddress);

  // Then
  EXPECT_EQ(query.getContractEvmAddress().value(), evmAddress);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetSender)
{
  // Given
  AccountId sender(1, 2, 3);

  // When
  query.setSender(sender);

  // Then
  EXPECT_EQ(query.getSender().value(), sender);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetSenderEvmAddress)
{
  // Given
  std::string senderEvmAddress = "0x1234567890abcdef1234567890abcdef12345";

  // When
  query.setSenderEvmAddress(senderEvmAddress);

  // Then
  EXPECT_EQ(query.getSenderEvmAddress().value(), senderEvmAddress);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetFunction)
{
  // Given
  std::string functionName = "transfer";
  ContractFunctionParameters params;
  params.addAddress("0x1234567890abcdef1234567890abcdef12345678");
  params.addUint64(100);
  std::optional<ContractFunctionParameters> optParams = params;

  // When
  query.setFunction(functionName, optParams);

  // Then
  std::vector<std::byte> expectedCallData = params.toBytes(functionName);
  EXPECT_EQ(query.getCallData(), expectedCallData);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetValue)
{
  // Given
  int64_t value = 1000000000;

  // When
  query.setValue(value);

  // Then
  EXPECT_EQ(query.getValue(), value);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetGasLimit)
{
  // Given
  int64_t gasLimit = 3000000;

  // When
  query.setGasLimit(gasLimit);

  // Then
  EXPECT_EQ(query.getGasLimit(), gasLimit);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetGasPrice)
{
  // Given
  int64_t gasPrice = 20000000000; // 20 GWEI

  // When
  query.setGasPrice(gasPrice);

  // Then
  EXPECT_EQ(query.getGasPrice(), gasPrice);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetBlockNumber)
{
  // Given
  uint64_t blockNumber = 12345678;

  // When
  query.setBlockNumber(blockNumber);

  // Then
  EXPECT_EQ(query.getBlockNumber(), blockNumber);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, SetEstimate)
{
  // Given
  bool estimate = true;

  // When
  query.setEstimate(estimate);

  // Then
  EXPECT_EQ(query.getEstimate(), estimate);
}

//-----

//-----
TEST_F(MirrorNodeContractQueryUnitTests, ToJson)
{
  // Given
  query.setContractEvmAddress("0xabcdef1234567890");
  query.setSenderEvmAddress("0x1234567890abcdef");
  query.setGasLimit(3000000);
  query.setGasPrice(20000000000);
  query.setValue(1000000000);
  query.setBlockNumber(12345678);
  query.setEstimate(true);

  // When
  json result = query.toJson();

  // Then
  json expectedJson = {
    {"to",           "0xabcdef1234567890"},
    { "from",        "0x1234567890abcdef"},
    { "gas",         3000000             },
    { "gasPrice",    20000000000         },
    { "value",       1000000000          },
    { "blockNumber", "12345678"          },
    { "estimate",    true                },
    { "data",        ""                  }
  };
  EXPECT_EQ(result, expectedJson);
}