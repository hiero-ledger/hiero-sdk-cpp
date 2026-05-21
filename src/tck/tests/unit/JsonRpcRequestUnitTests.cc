// SPDX-License-Identifier: Apache-2.0
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonRpcRequest.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Hiero::TCK;
using nlohmann::json;

class JsonRpcRequestUnitTests : public ::testing::Test
{
};

TEST_F(JsonRpcRequestUnitTests, ParseValidRequest)
{
  json j = {
    {"jsonrpc", "2.0"                 },
    { "method", "testMethod"          },
    { "params", { { "key", "value" } }},
    { "id",     1                     }
  };

  auto req = JsonRpcRequest::parse(j);
  EXPECT_EQ(req.getMethod(), "testMethod");
  EXPECT_EQ(req.getId(), 1);
  EXPECT_EQ(req.getParams()["key"], "value");
  EXPECT_FALSE(req.isNotification());
}

TEST_F(JsonRpcRequestUnitTests, ParseNotification)
{
  json j = {
    {"jsonrpc", "2.0"   },
    { "method", "notify"},
    { "params", {}      }
  };

  auto req = JsonRpcRequest::parse(j);
  EXPECT_EQ(req.getMethod(), "notify");
  EXPECT_TRUE(req.getId().is_null());
  EXPECT_TRUE(req.isNotification());
}

TEST_F(JsonRpcRequestUnitTests, ThrowsOnMissingVersion)
{
  json j = {
    {"method", "test"},
    { "id",    1     }
  };

  try
  {
    JsonRpcRequest::parse(j);
    FAIL() << "Expected JsonRpcException";
  }
  catch (const JsonRpcException& e)
  {
    EXPECT_EQ(e.getCode(), JsonErrorType::INVALID_REQUEST);
  }
}

TEST_F(JsonRpcRequestUnitTests, ThrowsOnWrongVersion)
{
  json j = {
    {"jsonrpc", "1.0" },
    { "method", "test"},
    { "id",     1     }
  };

  EXPECT_THROW(JsonRpcRequest::parse(j), JsonRpcException);
}

TEST_F(JsonRpcRequestUnitTests, ThrowsOnMissingMethod)
{
  json j = {
    {"jsonrpc", "2.0"},
    { "id",     1    }
  };

  EXPECT_THROW(JsonRpcRequest::parse(j), JsonRpcException);
}

TEST_F(JsonRpcRequestUnitTests, ThrowsOnInvalidIdType)
{
  json j = {
    {"jsonrpc", "2.0" },
    { "method", "test"},
    { "id",     true  }  // Boolean is not a valid ID type
  };

  EXPECT_THROW(JsonRpcRequest::parse(j), JsonRpcException);
}

TEST_F(JsonRpcRequestUnitTests, ThrowsOnMethodNotString)
{
  json j = {
    {"jsonrpc", "2.0"},
    { "method", 123  },
    { "id",     1    }
  };

  try
  {
    JsonRpcRequest::parse(j);
    FAIL() << "Expected JsonRpcException";
  }
  catch (const JsonRpcException& e)
  {
    EXPECT_EQ(e.getCode(), JsonErrorType::INVALID_REQUEST);
    EXPECT_EQ(e.getMessage(), "invalid request: method field must be a string");
  }
}

TEST_F(JsonRpcRequestUnitTests, ThrowsOnParamsInvalidTypeString)
{
  json j = {
    {"jsonrpc", "2.0"    },
    { "method", "test"   },
    { "params", "invalid"},
    { "id",     1        }
  };

  try
  {
    JsonRpcRequest::parse(j);
    FAIL() << "Expected JsonRpcException";
  }
  catch (const JsonRpcException& e)
  {
    EXPECT_EQ(e.getCode(), JsonErrorType::INVALID_REQUEST);
    EXPECT_EQ(e.getMessage(), "invalid request: params field must be an array, object or null");
  }
}

TEST_F(JsonRpcRequestUnitTests, ThrowsOnParamsInvalidTypeBoolean)
{
  json j = {
    {"jsonrpc", "2.0" },
    { "method", "test"},
    { "params", true  },
    { "id",     1     }
  };

  try
  {
    JsonRpcRequest::parse(j);
    FAIL() << "Expected JsonRpcException";
  }
  catch (const JsonRpcException& e)
  {
    EXPECT_EQ(e.getCode(), JsonErrorType::INVALID_REQUEST);
    EXPECT_EQ(e.getMessage(), "invalid request: params field must be an array, object or null");
  }
}
