// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "ContractCallQuery.h"
#include "ContractId.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class ContractCallQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const uint64_t& getTestGas() const { return mTestGas; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestFunctionParameters() const
  {
    return mTestFunctionParameters;
  }
  [[nodiscard]] inline const AccountId& getTestSenderAccountId() const { return mTestSenderAccountId; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const uint64_t mTestGas = 2ULL;
  const std::vector<std::byte> mTestFunctionParameters = { std::byte(0x03), std::byte(0x04), std::byte(0x05) };
  const AccountId mTestSenderAccountId = AccountId(6ULL);
};

//-----
TEST_F(ContractCallQueryUnitTests, GetSetContractId)
{
  // Given
  ContractCallQuery query;

  // When
  query.setContractId(getTestContractId());

  // Then
  EXPECT_EQ(query.getContractId(), getTestContractId());
}

//-----
TEST_F(ContractCallQueryUnitTests, GetSetGas)
{
  // Given
  ContractCallQuery query;

  // When
  query.setGas(getTestGas());

  // Then
  EXPECT_EQ(query.getGas(), getTestGas());
}

//-----
TEST_F(ContractCallQueryUnitTests, GetSetFunctionParameters)
{
  // Given
  ContractCallQuery query;

  // When
  query.setFunctionParameters(getTestFunctionParameters());

  // Then
  EXPECT_EQ(query.getFunctionParameters(), getTestFunctionParameters());
}

//-----
TEST_F(ContractCallQueryUnitTests, GetSetSenderAccountId)
{
  // Given
  ContractCallQuery query;

  // When
  query.setSenderAccountId(getTestSenderAccountId());

  // Then
  EXPECT_EQ(query.getSenderAccountId(), getTestSenderAccountId());
}
