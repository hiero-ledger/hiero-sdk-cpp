// SPDX-License-Identifier: Apache-2.0
#include "AccountRecordsQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountRecordsQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }

private:
  const AccountId mTestAccountId = AccountId(1ULL);
};

//-----
TEST_F(AccountRecordsQueryUnitTests, ConstructAccountRecordsQuery)
{
  // Given / When
  AccountRecordsQuery query;

  // Then
  EXPECT_EQ(query.getAccountId(), AccountId());
}

//-----
TEST_F(AccountRecordsQueryUnitTests, SetAccountId)
{
  // Given
  AccountRecordsQuery query;

  // When
  query.setAccountId(getTestAccountId());

  // Then
  EXPECT_EQ(query.getAccountId(), getTestAccountId());
}
