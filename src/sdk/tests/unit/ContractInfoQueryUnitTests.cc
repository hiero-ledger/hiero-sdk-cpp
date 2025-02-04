// SPDX-License-Identifier: Apache-2.0
#include "ContractId.h"
#include "ContractInfoQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class ContractInfoQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
};

//-----
TEST_F(ContractInfoQueryUnitTests, GetSetContractId)
{
  // Given
  ContractInfoQuery query;

  // When
  query.setContractId(getTestContractId());

  // Then
  EXPECT_EQ(query.getContractId(), getTestContractId());
}
