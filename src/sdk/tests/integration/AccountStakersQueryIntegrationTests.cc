// SPDX-License-Identifier: Apache-2.0
#include "AccountStakersQuery.h"
#include "BaseIntegrationTest.h"
#include "ProxyStaker.h"
#include "exceptions/PrecheckStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountStakersQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(AccountStakersQueryIntegrationTests, ExecuteAccountStakersQuery)
{
  // Given / When / Then
  EXPECT_THROW(const AccountStakers accountStakers =
                 AccountStakersQuery().setAccountId(AccountId(2ULL)).execute(getTestClient()),
               PrecheckStatusException); // NOT_SUPPORTED
}
