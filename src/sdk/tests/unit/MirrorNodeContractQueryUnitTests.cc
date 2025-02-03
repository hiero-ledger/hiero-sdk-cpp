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
TEST_F(MirrorNodeContractQueryUnitTests, TestSetContractId)
{
  ContractId contractId(1, 2, 3);
  query.setContractId(contractId);

  EXPECT_EQ(query.getContractId().value(), contractId);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetContractEvmAddress)
{
  std::string evmAddress = "0x1234567890abcdef1234567890abcdef12345";
  query.setContractEvmAddress(evmAddress);

  EXPECT_EQ(query.getContractEvmAddress().value(), evmAddress);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetSender)
{
  AccountId sender(1, 2, 3);
  query.setSender(sender);

  EXPECT_EQ(query.getSender().value(), sender);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetSenderEvmAddress)
{
  std::string senderEvmAddress = "0x1234567890abcdef1234567890abcdef12345";
  query.setSenderEvmAddress(senderEvmAddress);

  EXPECT_EQ(query.getSenderEvmAddress().value(), senderEvmAddress);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetFunction)
{
  std::string functionName = "transfer";
  ContractFunctionParameters params;
  params.addAddress("0x1234567890abcdef1234567890abcdef12345678");
  params.addUint64(100);

  std::optional<ContractFunctionParameters> optParams = params;

  query.setFunction(functionName, optParams);

  std::vector<std::byte> expectedCallData = params.toBytes(functionName);
  EXPECT_EQ(query.getCallData(), expectedCallData);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetValue)
{
  int64_t value = 1000000000;
  query.setValue(value);

  EXPECT_EQ(query.getValue(), value);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetGasLimit)
{
  int64_t gasLimit = 3000000;
  query.setGasLimit(gasLimit);

  EXPECT_EQ(query.getGasLimit(), gasLimit);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetGasPrice)
{
  int64_t gasPrice = 20000000000; // 20 GWEI
  query.setGasPrice(gasPrice);

  EXPECT_EQ(query.getGasPrice(), gasPrice);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetBlockNumber)
{
  uint64_t blockNumber = 12345678;
  query.setBlockNumber(blockNumber);

  EXPECT_EQ(query.getBlockNumber(), blockNumber);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestSetEstimate)
{
  bool estimate = true;
  query.setEstimate(estimate);

  EXPECT_EQ(query.getEstimate(), estimate);
}

//-----
TEST_F(MirrorNodeContractQueryUnitTests, TestToJson)
{
  query.setContractEvmAddress("0xabcdef1234567890");
  query.setSenderEvmAddress("0x1234567890abcdef");
  query.setGasLimit(3000000);
  query.setGasPrice(20000000000);
  query.setValue(1000000000);
  query.setBlockNumber(12345678);
  query.setEstimate(true);

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

  EXPECT_EQ(query.toJson(), expectedJson);
}