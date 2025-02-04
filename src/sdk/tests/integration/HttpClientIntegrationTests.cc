// SPDX-License-Identifier: Apache-2.0

#include "BaseIntegrationTest.h"
#include "impl/HttpClient.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

using json = nlohmann::json;
using namespace Hiero;

class HttpClientIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::string& getURL() const { return mUrl; }
  [[nodiscard]] inline const std::string& getJsonMirrorNetworkTag() const { return mJsonMirrorNetworkTag; }
  [[nodiscard]] inline const std::string& getAccountIdStr() const { return mAccountIdStr; }

private:
  const std::string mUrl = "http://127.0.0.1:5551/api/v1/accounts/";
  const std::string mJsonMirrorNetworkTag = "mirrorNetwork";
  const std::string mAccountIdStr = "0.0.3";
};

//-----
TEST_F(HttpClientIntegrationTests, GETAccountFromLocalMirrorNode)
{
  // Given
  const std::string& mirrorNetworkTag = getJsonMirrorNetworkTag();
  const std::string& accountIdStr = getAccountIdStr();
  std::string mUrl(getURL());
  mUrl += accountIdStr;

  // When
  std::string response;
  ASSERT_NO_THROW(response = internal::HttpClient::invokeREST(mUrl, "GET"));

  // Then
  json responseData = json::parse(response);

  ASSERT_FALSE(responseData.empty());
  EXPECT_FALSE(responseData["account"].empty());
  EXPECT_FALSE(responseData["created_timestamp"].empty());

  EXPECT_EQ(responseData["account"], accountIdStr);
}
