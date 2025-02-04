// SPDX-License-Identifier: Apache-2.0
#include "AccountStakersQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountStakersQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }

private:
  const AccountId mTestAccountId = AccountId(1ULL);
};

//-----
TEST_F(AccountStakersQueryUnitTests, SetAccountId)
{
  // Given
  AccountStakersQuery query;

  // When
  query.setAccountId(getTestAccountId());

  // Then
  EXPECT_EQ(query.getAccountId(), getTestAccountId());
}
