// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "AccountInfoQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountInfoQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }

private:
  const AccountId mTestAccountId = AccountId(1ULL);
};

//-----
TEST_F(AccountInfoQueryUnitTests, GetSetAccountId)
{
  // Given
  AccountInfoQuery query;

  // When
  query.setAccountId(getTestAccountId());

  // Then
  EXPECT_EQ(query.getAccountId(), getTestAccountId());
}
