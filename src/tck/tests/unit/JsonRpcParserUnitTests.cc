// SPDX-License-Identifier: Apache-2.0
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonRpcParser.h"

#include <Status.h>
#include <exceptions/PrecheckStatusException.h>
#include <exceptions/ReceiptStatusException.h>

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

    mParser.addMethod("throw_json_rpc_exception",
                      [](const json& params) -> json
                      {
                        throw JsonRpcException(JsonErrorType::INVALID_PARAMS,
                                               "Invalid parameters provided",
                                               json{
                                                 {"extra", "data"}
                        });
                      });

    mParser.addMethod("throw_receipt_exception",
                      [](const json& params) -> json
                      { throw Hiero::ReceiptStatusException(Hiero::TransactionId(), Hiero::Status::ACCOUNT_DELETED); });

    mParser.addMethod("throw_precheck_exception",
                      [](const json& params) -> json
                      { throw Hiero::PrecheckStatusException(Hiero::Status::INSUFFICIENT_ACCOUNT_BALANCE); });
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
TEST_F(JsonRpcParserUnitTests, HandleEmptyBatchRequest)
{
  json req = json::array();

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::INVALID_REQUEST);
  EXPECT_EQ(res["error"]["message"], "invalid request: empty batch");
}

TEST_F(JsonRpcParserUnitTests, HandleInvalidRootTypeString)
{
  std::string invalidRequest = "\"hello\"";

  std::string output = mParser.handle(invalidRequest);
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::INVALID_REQUEST);
  EXPECT_EQ(res["error"]["message"], "invalid request: expected array or object");
}

TEST_F(JsonRpcParserUnitTests, HandleInvalidRootTypeNumber)
{
  std::string invalidRequest = "123";

  std::string output = mParser.handle(invalidRequest);
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::INVALID_REQUEST);
  EXPECT_EQ(res["error"]["message"], "invalid request: expected array or object");
}

TEST_F(JsonRpcParserUnitTests, HandlerThrowsJsonRpcException)
{
  json req = {
    {"jsonrpc", "2.0"                     },
    { "method", "throw_json_rpc_exception"},
    { "id",     1                         }
  };

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::INVALID_PARAMS);
  EXPECT_EQ(res["error"]["message"], "Invalid parameters provided");
  EXPECT_EQ(res["error"]["data"]["extra"], "data");
  EXPECT_EQ(res["id"], 1);
}

TEST_F(JsonRpcParserUnitTests, HandlerThrowsReceiptStatusException)
{
  json req = {
    {"jsonrpc", "2.0"                    },
    { "method", "throw_receipt_exception"},
    { "id",     1                        }
  };

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::HIERO_ERROR);
  EXPECT_EQ(res["error"]["message"], "Hiero error");
  EXPECT_EQ(res["error"]["data"]["status"], "ACCOUNT_DELETED");
  EXPECT_TRUE(res["error"]["data"].contains("message"));
  EXPECT_EQ(res["id"], 1);
}

TEST_F(JsonRpcParserUnitTests, HandlerThrowsPrecheckStatusException)
{
  json req = {
    {"jsonrpc", "2.0"                     },
    { "method", "throw_precheck_exception"},
    { "id",     1                         }
  };

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::HIERO_ERROR);
  EXPECT_EQ(res["error"]["message"], "Hiero error");
  EXPECT_EQ(res["error"]["data"]["status"], "INSUFFICIENT_ACCOUNT_BALANCE");
  EXPECT_TRUE(res["error"]["data"].contains("message"));
  EXPECT_EQ(res["id"], 1);
}

TEST_F(JsonRpcParserUnitTests, HandlerThrowsStdException)
{
  json req = {
    {"jsonrpc", "2.0"       },
    { "method", "error_func"},
    { "id",     1           }
  };

  std::string output = mParser.handle(req.dump());
  json res = json::parse(output);

  EXPECT_EQ(res["error"]["code"], JsonErrorType::INTERNAL_ERROR);
  EXPECT_EQ(res["error"]["message"], "Internal error");
  EXPECT_EQ(res["error"]["data"]["message"], "Something broke");
  EXPECT_EQ(res["id"], 1);
}
