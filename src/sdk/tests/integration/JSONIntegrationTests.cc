// SPDX-License-Identifier: Apache-2.0
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JSONIntegrationTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::string getPathToJSON() const { return mFilePath.string(); }

private:
  const std::filesystem::path mFilePath = (std::filesystem::current_path() / "local_node.json").string();
};

// Tests JSON parsing of a configuration file for the Local Node.
TEST_F(JSONIntegrationTests, ParseJSONConfig)
{
  // Given
  std::string testPathToJSON = getPathToJSON();
  std::ifstream testInputFile(testPathToJSON, std::ios::in);

  try
  {
    // When
    json jsonData = json::parse(testInputFile);

    // Then
    EXPECT_FALSE(jsonData.empty());
    EXPECT_FALSE(jsonData["network"].empty());
    EXPECT_FALSE(jsonData["network"]["0.0.3"].empty());
    EXPECT_TRUE(jsonData["network"]["0.0.3"].is_string());
    EXPECT_FALSE(jsonData["operator"].empty());
    EXPECT_FALSE(jsonData["operator"]["accountId"].empty());
    EXPECT_TRUE(jsonData["operator"]["accountId"].is_string());
    EXPECT_FALSE(jsonData["operator"]["privateKey"].empty());
    EXPECT_TRUE(jsonData["operator"]["privateKey"].is_string());
  }
  catch (json::parse_error& error)
  {
    EXPECT_TRUE(false);
  }

  testInputFile.close();
}