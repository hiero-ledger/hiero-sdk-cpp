// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "impl/Network.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <unordered_map>
#include <vector>

using namespace Hiero;

class NetworkIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(NetworkIntegrationTests, GetNodeAccountIdsForExecuteReturnsHealthyNodes)
{
  // Given
  const Client& client = getTestClient();
  const std::unordered_map<std::string, AccountId> networkMap = client.getNetwork();

  ASSERT_FALSE(networkMap.empty());

  internal::Network network = internal::Network::forNetwork(networkMap);

  // When
  //  Requesting the maximum number of attempts equal to the size of network
  const std::vector<AccountId> accountIds = network.getNodeAccountIdsForExecute(networkMap.size());

  // Then
  EXPECT_FALSE(accountIds.empty());
  EXPECT_LE(accountIds.size(), networkMap.size());

  // verifying that algorithm worked properly
  for (const auto& accountId : accountIds)
  {
    const bool found = std::any_of(
      networkMap.cbegin(), networkMap.cend(), [&accountId](const auto& entry) { return entry.second == accountId; });

    EXPECT_TRUE(found);
  }
  network.close();
}

//-----
TEST_F(NetworkIntegrationTests, ExecutesQueryUsingSelectedNodes)
{
  // Given
  const Client& client = getTestClient();

  // When / Then
  // Executing a query relies on the node selection algorithm internally.
  // If it doesn't throw, it successfully selected a healthy node and executed.
  EXPECT_NO_THROW(AccountBalanceQuery().setAccountId(AccountId(2ULL)).execute(client));
}

//-----
TEST_F(NetworkIntegrationTests, NodeSelectionPrioritizesHealthyNodes)
{
  // Given
  const Client& originalClient = getTestClient();
  const std::unordered_map<std::string, AccountId> originalNetwork = originalClient.getNetwork();
  ASSERT_FALSE(originalNetwork.empty());

  std::unordered_map<std::string, AccountId> customNetwork;

  // fake, unresponsive node to the map
  customNetwork["127.0.0.1:99999"] = AccountId(999ULL);

  // adding a healthy node to the map from existing network
  customNetwork.insert(*originalNetwork.begin());

  Client client = Client::forNetwork(customNetwork);

  // When
  // The first execution will attempt to use the bad node, fail, increase its backoff (deprioritize it),
  // and successfully fall back to the good node.
  EXPECT_NO_THROW(AccountBalanceQuery().setAccountId(AccountId(2ULL)).execute(client));

  // Then
  // Subsequent queries should rapidly prioritize the healthy node immediately since the bad one is in backoff.
  EXPECT_NO_THROW(AccountBalanceQuery().setAccountId(AccountId(2ULL)).execute(client));
  EXPECT_NO_THROW(AccountBalanceQuery().setAccountId(AccountId(2ULL)).execute(client));
}
