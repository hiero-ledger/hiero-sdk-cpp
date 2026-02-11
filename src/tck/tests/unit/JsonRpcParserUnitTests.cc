// SPDX-License-Identifier: Apache-2.0
#include "json/JsonErrorType.h"
#include "json/JsonRpcParser.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Hiero::TCK;
using nlohmann::json;

class JsonRpcParserUnitTests : public ::testing::Test
{
protected:
  JsonRpcParser mParser;

  void SetUp() override
  {
    mParser.addMethod("echo", [](const json& params) { return params; });

    mParser.addMethod("subtract", [](const json& params) { return params[0].get<int>() - params[1].get<int>(); });

    mParser.addMethod("error_func", [](const json& params) -> json { throw std::runtime_error("Something broke"); });
  }
};

TEST_F(JsonRpcParserUnitTests, HandleSingleRequest)
{
  json req = {
    {"jsonrpc", "2.0"                 },
    { "method", "echo"                },
    { "params", { { "msg", "hello" } }},
    { "id",     1                     }
  };

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_EQ(res["result"]["msg"], "hello");
  EXPECT_EQ(res["id"], 1);
}

TEST_F(JsonRpcParserUnitTests, HandleMethodNotFound)
{
  json req = {
    {"jsonrpc", "2.0"        },
    { "method", "nonexistent"},
    { "id",     1            }
  };

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::METHOD_NOT_FOUND);
}

TEST_F(JsonRpcParserUnitTests, HandleBatchRequest)
{
  json req = json::array();

  // Request 1: subtract(42, 23) -> 19
  req.push_back({
    {"jsonrpc", "2.0"     },
    { "method", "subtract"},
    { "params", { 42, 23 }},
    { "id",     1         }
  });

  // Request 2: subtract(23, 42) -> -19
  req.push_back({
    {"jsonrpc", "2.0"     },
    { "method", "subtract"},
    { "params", { 23, 42 }},
    { "id",     2         }
  });

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_TRUE(res.is_array());
  EXPECT_EQ(res.size(), 2);

  // Order isn't guaranteed by spec, but implementation usually preserves it
  // We can find by ID to be safe
  for (const auto& r : res)
  {
    if (r["id"] == 1)
      EXPECT_EQ(r["result"], 19);
    if (r["id"] == 2)
      EXPECT_EQ(r["result"], -19);
  }
}

TEST_F(JsonRpcParserUnitTests, HandleInvalidJson)
{
  std::string badJson = "{ \"jsonrpc\": \"2.0\", \"method\": ... }"; // malformed
  std::string output = mParser.handle(badJson);
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::PARSE_ERROR);
}

TEST_F(JsonRpcParserUnitTests, HandleNotificationReturnsEmpty)
{
  json req = {
    {"jsonrpc", "2.0"                 },
    { "method", "echo"                },
    { "params", { { "msg", "hello" } }}
  };

  std::string output = mParser.handle(req.dump());
  EXPECT_TRUE(output.empty());
}