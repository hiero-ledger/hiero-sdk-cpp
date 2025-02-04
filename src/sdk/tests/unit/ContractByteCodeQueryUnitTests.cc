// SPDX-License-Identifier: Apache-2.0
#include "ContractByteCodeQuery.h"
#include "ContractId.h"

#include <gtest/gtest.h>

using namespace Hiero;

class ContractByteCodeQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
};

//-----
TEST_F(ContractByteCodeQueryUnitTests, GetSetContractId)
{
  // Given
  ContractByteCodeQuery query;

  // When
  query.setContractId(getTestContractId());

  // Then
  EXPECT_EQ(query.getContractId(), getTestContractId());
}
