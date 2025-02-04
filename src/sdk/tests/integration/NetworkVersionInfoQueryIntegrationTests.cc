// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "NetworkVersionInfo.h"
#include "NetworkVersionInfoQuery.h"
#include "TransactionReceipt.h"

#include <gtest/gtest.h>

using namespace Hiero;

class NetworkVersionInfoQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(NetworkVersionInfoQueryIntegrationTests, ExecuteNetworkVersionInfoQuery)
{
  // Given / When / Then
  EXPECT_NO_THROW(const NetworkVersionInfo networkVersionInfo = NetworkVersionInfoQuery().execute(getTestClient()));
}
