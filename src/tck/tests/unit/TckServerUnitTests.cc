// SPDX-License-Identifier: Apache-2.0
#include "TckServer.h"
#include "json/JsonErrorType.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Hiero::TCK
{
class TckServerUnitTests : public ::testing::Test
{
protected:
  TckServer server;

  // Helper to invoke the private handleJsonRequest
  std::string handleJsonRequest(const std::string& request) { return server.handleJsonRequest(request); }
};

//-----
TEST_F(TckServerUnitTests, AddMethodSuccessfully)
{
  // Given/When/Then
  EXPECT_NO_THROW(
    server.add("testMethod", std::function<json(const json&)>([](const json&) { return json("success"); })));
}

//-----
TEST_F(TckServerUnitTests, AddMethodWithRpcPrefixFails)
{
  // Given/When/Then
  EXPECT_THROW(server.add("rpc.test", std::function<json(const json&)>([](const json&) { return nullptr; })),
               std::invalid_argument);
}

//-----
TEST_F(TckServerUnitTests, AddDuplicateMethodFails)
{
  // Given
  server.add("testMethod", std::function<json(const json&)>([](const json&) { return nullptr; }));

  // When/Then
  EXPECT_THROW(server.add("testMethod", std::function<json(const json&)>([](const json&) { return nullptr; })),
               std::invalid_argument);
}

//-----
TEST_F(TckServerUnitTests, AddNotificationSuccessfully)
{
  // Given/When/Then
  EXPECT_NO_THROW(server.add("testNotify", [](const json&) {}));
}

//-----
TEST_F(TckServerUnitTests, AddNotificationWithRpcPrefixFails)
{
  // Given/When/Then
  EXPECT_THROW(server.add("rpc.notify", [](const json&) {}), std::invalid_argument);
}

//-----
TEST_F(TckServerUnitTests, AddDuplicateNotificationFails)
{
  // Given
  server.add("testNotify", [](const json&) {});

  // When/ Then
  EXPECT_THROW(server.add("testNotify", [](const json&) {}), std::invalid_argument);
}

//-----
TEST_F(TckServerUnitTests, ValidSingleRequestReturnsResponse)
{
  // Given
  server.add("echo", std::function<json(const json&)>([](const json& params) { return params; }));

  // When
  json request = {
    {"jsonrpc", "2.0"  },
    { "method", "echo" },
    { "params", "hello"},
    { "id",     1      }
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["result"], json::array({ "hello" }));
  EXPECT_EQ(response["id"], 1);
  EXPECT_EQ(response["jsonrpc"], "2.0");
}

//-----
TEST_F(TckServerUnitTests, ValidBatchRequestReturnsArray)
{
  // Given
  server.add("echo", std::function<json(const json&)>([](const json& params) { return params; }));

  // When
  json request = json::array({
    {{ "jsonrpc", "2.0" },  { "method", "echo" }, { "params", 1 }, { "id", 1 }},
    { { "jsonrpc", "2.0" }, { "method", "echo" }, { "params", 2 }, { "id", 2 }}
  });
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  ASSERT_TRUE(response.is_array());
  EXPECT_EQ(response.size(), 2);
  EXPECT_EQ(response[0]["result"], json::array({ 1 }));
  EXPECT_EQ(response[1]["result"], json::array({ 2 }));
}

//-----
TEST_F(TckServerUnitTests, InvalidJsonReturnsParseError)
{
  // Given/When
  std::string responseStr = handleJsonRequest("{ unclosed json");

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::PARSE_ERROR);
}

//-----
TEST_F(TckServerUnitTests, NonArrayNonObjectReturnsInvalidRequest)
{
  // Given/When
  std::string responseStr = handleJsonRequest("\"just a string\"");

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

//-----
TEST_F(TckServerUnitTests, MissingJsonRpcReturnsInvalidRequest)
{
  // Given/When
  json request = {
    {"method", "test"},
    { "id",    1     }
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

//-----
TEST_F(TckServerUnitTests, WrongJsonRpcVersionReturnsInvalidRequest)
{
  // Given/When
  json request = {
    {"jsonrpc", "1.0" },
    { "method", "test"},
    { "id",     1     }
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

//-----
TEST_F(TckServerUnitTests, MissingMethodReturnsInvalidRequest)
{
  // Given/When
  json request = {
    {"jsonrpc", "2.0"},
    { "id",     1    }
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

//-----
TEST_F(TckServerUnitTests, InvalidIdTypeReturnsInvalidRequest)
{
  // Given/When
  json request = {
    {"jsonrpc", "2.0" },
    { "method", "test"},
    { "id",     true  }
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

//-----
TEST_F(TckServerUnitTests, InvalidParamsTypeReturnsInvalidRequest)
{
  // Given/When
  json request = {
    {"jsonrpc", "2.0" },
    { "method", "test"},
    { "params", 42    },
    { "id",     1     }
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

//-----
TEST_F(TckServerUnitTests, UnknownMethodReturnsMethodNotFound)
{
  // Given/When
  json request = {
    {"jsonrpc", "2.0"              },
    { "method", "nonExistentMethod"},
    { "id",     1                  }
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::METHOD_NOT_FOUND);
}

//-----
TEST_F(TckServerUnitTests, NotificationExecutesWithoutResponse)
{
  // Given
  bool executed = false;
  server.add("notify", [&](const json&) { executed = true; });

  // When
  json request = {
    {"jsonrpc", "2.0"   },
    { "method", "notify"}
  };
  std::string responseStr = handleJsonRequest(request.dump());

  // Then
  EXPECT_TRUE(executed);
  EXPECT_TRUE(responseStr.empty());
}

} // namespace Hiero::TCK
