// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "impl/MirrorNodeGateway.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace Hiero;

class MirrorNodeGatewayIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::string& getAccountIdStr() const { return mAccountIdStr; }
  [[nodiscard]] inline const std::string& getMirrorNetworkUrl() const { return mMirrorNetworkUrl; }

private:
  const std::string mAccountIdStr = "0.0.3";
  const std::string mMirrorNetworkUrl = "http://127.0.0.1:5551";
};

//-----
TEST_F(MirrorNodeGatewayIntegrationTests, AccountInfoQuery)
{
  // Given
  const std::string& accountIdStr = getAccountIdStr();

  // When
  json response;
  ASSERT_NO_THROW(response = internal::MirrorNodeGateway::MirrorNodeQuery(
                    getMirrorNetworkUrl(), { accountIdStr }, internal::MirrorNodeGateway::ACCOUNT_INFO_QUERY.data()););

  // Then
  ASSERT_FALSE(response.empty());           // checks if any data
  EXPECT_TRUE(response["_status"].empty()); // if status is in json then not found
}

//-----
TEST_F(MirrorNodeGatewayIntegrationTests, ContractInfoQuery)
{
  // Given
  const std::string& contractIdStr = getAccountIdStr();

  // When
  json response;
  ASSERT_NO_THROW(
    response = internal::MirrorNodeGateway::MirrorNodeQuery(
      getMirrorNetworkUrl(), { contractIdStr }, internal::MirrorNodeGateway::CONTRACT_INFO_QUERY.data()););

  // Then
  ASSERT_FALSE(response.empty());            // checks if any data
  EXPECT_FALSE(response["_status"].empty()); // no such contract exists then should have _status not found
}

//-----
TEST_F(MirrorNodeGatewayIntegrationTests, TokenRelationshipQuery)
{
  // Given
  const std::string& accountIdStr = getAccountIdStr();

  // When
  json response;
  ASSERT_NO_THROW(
    response = internal::MirrorNodeGateway::MirrorNodeQuery(
      getMirrorNetworkUrl(), { accountIdStr }, internal::MirrorNodeGateway::TOKEN_RELATIONSHIPS_QUERY.data()););

  // Then
  ASSERT_FALSE(response.empty());           // checks if any data
  EXPECT_TRUE(response["_status"].empty()); // if status is in json then not found
}

//-----
TEST_F(MirrorNodeGatewayIntegrationTests, TokensBalancesQuery)
{
  // Given
  const std::string& accountIdStr = getAccountIdStr();

  // When
  json response;
  ASSERT_NO_THROW(
    response = internal::MirrorNodeGateway::MirrorNodeQuery(
      getMirrorNetworkUrl(), { accountIdStr }, internal::MirrorNodeGateway::TOKEN_BALANCES_QUERY.data()););

  // Then
  ASSERT_FALSE(response.empty());           // checks if any data
  EXPECT_TRUE(response["_status"].empty()); // no such contract exists then should have _status not found
}
