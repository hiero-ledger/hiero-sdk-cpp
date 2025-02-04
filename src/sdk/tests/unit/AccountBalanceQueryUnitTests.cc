// SPDX-License-Identifier: Apache-2.0
#include "AccountBalanceQuery.h"
#include "AccountId.h"
#include "ContractId.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountBalanceQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }

private:
  const AccountId mTestAccountId = AccountId(3ULL);
  const ContractId mTestContractId = ContractId(3ULL);
};

//-----
TEST_F(AccountBalanceQueryUnitTests, ConstructAccountBalanceQuery)
{
  // Given \ When
  AccountBalanceQuery query;

  // Then
  EXPECT_FALSE(query.getAccountId());
  EXPECT_FALSE(query.getContractId());
}

//-----
TEST_F(AccountBalanceQueryUnitTests, SetAccountId)
{
  // Given
  AccountBalanceQuery query;

  // When
  query.setAccountId(getTestAccountId());

  // Then
  EXPECT_EQ(query.getAccountId(), getTestAccountId());
}

//-----
TEST_F(AccountBalanceQueryUnitTests, SetContractId)
{
  // Given
  AccountBalanceQuery query;

  // When
  query.setContractId(getTestContractId());

  // Then
  EXPECT_EQ(query.getContractId(), getTestContractId());
}

//-----
TEST_F(AccountBalanceQueryUnitTests, ResetAccountId)
{
  // Given
  AccountBalanceQuery query;
  query.setAccountId(getTestAccountId());

  // When
  query.setContractId(getTestContractId());

  // Then
  EXPECT_FALSE(query.getAccountId());
  EXPECT_TRUE(query.getContractId());
}

//-----
TEST_F(AccountBalanceQueryUnitTests, ResetContractId)
{
  // Given
  AccountBalanceQuery query;
  query.setContractId(getTestContractId());

  // When
  query.setAccountId(getTestAccountId());

  // Then
  EXPECT_TRUE(query.getAccountId());
  EXPECT_FALSE(query.getContractId());
}
