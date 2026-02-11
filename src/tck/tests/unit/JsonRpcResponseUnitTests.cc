// SPDX-License-Identifier: Apache-2.0
#include "json/JsonErrorType.h"
#include "json/JsonRpcResponse.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Hiero::TCK;
using nlohmann::json;

class JsonRpcResponseUnitTests : public ::testing::Test
{
};

TEST_F(JsonRpcResponseUnitTests, MakeSuccess)
{
  json id = 123;
  json result = {
    {"status", "ok"}
  };

  json response = JsonRpcResponse::makeSuccess(id, result);

  EXPECT_EQ(response["jsonrpc"], "2.0");
  EXPECT_EQ(response["id"], 123);
  EXPECT_EQ(response["result"]["status"], "ok");
  EXPECT_FALSE(response.contains("error"));
}

TEST_F(JsonRpcResponseUnitTests, MakeErrorWithoutData)
{
  json id = "abc";
  std::string message = "Something went wrong";
  int code = static_cast<int>(JsonErrorType::INVALID_PARAMS);

  json response = JsonRpcResponse::makeError(id, code, message);

  EXPECT_EQ(response["jsonrpc"], "2.0");
  EXPECT_EQ(response["id"], "abc");
  EXPECT_FALSE(response.contains("result"));

  EXPECT_TRUE(response.contains("error"));
  EXPECT_EQ(response["error"]["code"], code);
  EXPECT_EQ(response["error"]["message"], message);
  EXPECT_FALSE(response["error"].contains("data"));
}

TEST_F(JsonRpcResponseUnitTests, MakeErrorWithData)
{
  json id = nullptr;
  json data = {
    {"details", "stack trace"}
  };

  json response = JsonRpcResponse::makeError(id, static_cast<int>(JsonErrorType::INTERNAL_ERROR), "Error", data);

  EXPECT_EQ(response["error"]["data"]["details"], "stack trace");
}