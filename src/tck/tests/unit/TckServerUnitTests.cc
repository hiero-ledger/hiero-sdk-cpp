// SPDX-License-Identifier: Apache-2.0
#include "json/JsonErrorType.h"
#include "json/JsonRpcParser.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Hiero::TCK
{
class TckServerUnitTests : public ::testing::Test
{
protected:
  JsonRpcParser parser;

  void SetUp() override
  {
    parser.addMethod("echo", [](const json& params) -> json { return params; });
    parser.addMethod("testMethod", [](const json&) -> json { return json("success"); });
  }
};

TEST_F(TckServerUnitTests, AddMethodSuccessfully)
{
  JsonRpcParser p;
  EXPECT_NO_THROW(p.addMethod("newMethod", [](const json&) -> json { return json("ok"); }));
}

TEST_F(TckServerUnitTests, AddMethodWithRpcPrefixFails)
{
  JsonRpcParser p;
  EXPECT_THROW(p.addMethod("rpc.test", [](const json&) -> json { return nullptr; }), std::invalid_argument);
}

TEST_F(TckServerUnitTests, AddDuplicateMethodFails)
{
  JsonRpcParser p;
  p.addMethod("dup", [](const json&) -> json { return nullptr; });
  EXPECT_THROW(p.addMethod("dup", [](const json&) -> json { return nullptr; }), std::invalid_argument);
}

TEST_F(TckServerUnitTests, ValidSingleRequestReturnsResponse)
{
  json request = {
    {"jsonrpc", "2.0"                 },
    { "method", "echo"                },
    { "params", { { "msg", "hello" } }},
    { "id",     1                     }
  };
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  EXPECT_EQ(response["result"]["msg"], "hello");
  EXPECT_EQ(response["id"], 1);
  EXPECT_EQ(response["jsonrpc"], "2.0");
}

TEST_F(TckServerUnitTests, ValidBatchRequestReturnsArray)
{
  json request = json::array({
    {{ "jsonrpc", "2.0" },  { "method", "echo" }, { "params", { { "v", 1 } } }, { "id", 1 }},
    { { "jsonrpc", "2.0" }, { "method", "echo" }, { "params", { { "v", 2 } } }, { "id", 2 }}
  });
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  ASSERT_TRUE(response.is_array());
  EXPECT_EQ(response.size(), 2);
  EXPECT_EQ(response[0]["result"]["v"], 1);
  EXPECT_EQ(response[1]["result"]["v"], 2);
}

TEST_F(TckServerUnitTests, InvalidJsonReturnsParseError)
{
  std::string responseStr = parser.handle("{ unclosed json");

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::PARSE_ERROR);
}

TEST_F(TckServerUnitTests, NonArrayNonObjectReturnsInvalidRequest)
{
  std::string responseStr = parser.handle("\"just a string\"");

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

TEST_F(TckServerUnitTests, MissingJsonRpcReturnsInvalidRequest)
{
  json request = {
    {"method", "test"},
    { "id",    1     }
  };
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

TEST_F(TckServerUnitTests, WrongJsonRpcVersionReturnsInvalidRequest)
{
  json request = {
    {"jsonrpc", "1.0" },
    { "method", "test"},
    { "id",     1     }
  };
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

TEST_F(TckServerUnitTests, MissingMethodReturnsInvalidRequest)
{
  json request = {
    {"jsonrpc", "2.0"},
    { "id",     1    }
  };
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

TEST_F(TckServerUnitTests, InvalidIdTypeReturnsInvalidRequest)
{
  json request = {
    {"jsonrpc", "2.0" },
    { "method", "test"},
    { "id",     true  }
  };
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

TEST_F(TckServerUnitTests, InvalidParamsTypeReturnsInvalidRequest)
{
  json request = {
    {"jsonrpc", "2.0" },
    { "method", "test"},
    { "params", 42    },
    { "id",     1     }
  };
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::INVALID_REQUEST);
}

TEST_F(TckServerUnitTests, UnknownMethodReturnsMethodNotFound)
{
  json request = {
    {"jsonrpc", "2.0"              },
    { "method", "nonExistentMethod"},
    { "id",     1                  }
  };
  std::string responseStr = parser.handle(request.dump());

  json response = json::parse(responseStr);
  EXPECT_EQ(response["error"]["code"], JsonErrorType::METHOD_NOT_FOUND);
}

TEST_F(TckServerUnitTests, NotificationExecutesWithoutResponse)
{
  bool executed = false;
  JsonRpcParser p;
  p.addMethod("notify",
              [&](const json&) -> json
              {
                executed = true;
                return nullptr;
              });

  json request = {
    {"jsonrpc", "2.0"   },
    { "method", "notify"}
  };
  std::string responseStr = p.handle(request.dump());

  EXPECT_TRUE(executed);
  EXPECT_TRUE(responseStr.empty());
}

} // namespace Hiero::TCK
