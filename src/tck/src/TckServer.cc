// SPDX-License-Identifier: Apache-2.0
// Windows build requires this to be included first for some reason.
#include <Transaction.h> // NOLINT

#include "TckServer.h"
#include "account/params/CreateAccountParams.h"
#include "account/params/DeleteAccountParams.h"
#include "account/params/UpdateAccountParams.h"
#include "key/params/GenerateKeyParams.h"
#include "sdk/params/ResetParams.h"
#include "sdk/params/SetupParams.h"
#include "token/params/BurnTokenParams.h"
#include "token/params/CreateTokenParams.h"
#include "token/params/DeleteTokenParams.h"
#include "token/params/MintTokenParams.h"
#include "token/params/UpdateTokenParams.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonUtils.h"

#include <Status.h>
#include <exceptions/PrecheckStatusException.h>
#include <exceptions/ReceiptStatusException.h>

#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Hiero::TCK
{
namespace
{
// Code for a successful response.
constexpr auto HTTP_SUCCESSFUL_RESPONSE = 200;
// Code for a no-content response.
constexpr auto HTTP_NO_CONTENT_RESPONSE = 204;
} // namespace

//-----
TckServer::TckServer()
{
  setupHttpHandler();
}

//-----
TckServer::TckServer(int port)
  : mPort(port)
{
  setupHttpHandler();
}

//-----
void TckServer::add(const std::string& name, const MethodHandle& func)
{
  // Make sure the name is valid.
  if (name.find("rpc.") != std::string::npos)
  {
    throw std::invalid_argument(R"(invalid method name: method name must not contain "rpc.")");
  }

  // Make sure the function hasn't already been added.
  if (mMethods.find(name) != mMethods.end())
  {
    throw std::invalid_argument("invalid method name: method already added");
  }

  mMethods.try_emplace(name, func);
}

//-----
void TckServer::add(const std::string& name, const NotificationHandle& func)
{
  // Make sure the name is valid.
  if (name.find("rpc.") != std::string::npos)
  {
    throw std::invalid_argument(R"(invalid notification name: notification name must not contain "rpc.")");
  }

  // Make sure the function hasn't already been added.
  if (mNotifications.find(name) != mNotifications.end())
  {
    throw std::invalid_argument("invalid notification name: notification already added");
  }

  mNotifications.try_emplace(name, func);
}

//-----
void TckServer::startServer()
{
  mHttpServer.listen("localhost", mPort);
}

//-----
template<typename ParamsType>
TckServer::MethodHandle TckServer::getHandle(nlohmann::json (*method)(const ParamsType&))
{
  return [method](const nlohmann::json& params) { return method(params.get<ParamsType>()); };
}

//-----
template<typename ParamsType>
TckServer::NotificationHandle TckServer::getHandle(void (*notification)(const ParamsType&))
{
  return [notification](const nlohmann::json& params) { return notification(params.get<ParamsType>()); };
}

//-----
void TckServer::handleHttpRequest(const httplib::Request& request, httplib::Response& response)
{
  if (const std::string jsonResponse = handleJsonRequest(request.body); jsonResponse.empty())
  {
    response.status = HTTP_NO_CONTENT_RESPONSE;
  }
  else
  {
    response.status = HTTP_SUCCESSFUL_RESPONSE;
    response.set_content(jsonResponse, "application/json");
  }
}

//-----
std::string TckServer::handleJsonRequest(const std::string& request)
{
  try
  {
    const nlohmann::json jsonRequest = nlohmann::json::parse(request);

    // If the JSON request is an array, then it's a batch request. Handle each request individually.
    if (jsonRequest.is_array())
    {
      nlohmann::json results = nlohmann::json::array();
      for (const nlohmann::json& req : jsonRequest)
      {
        const nlohmann::json result = handleSingleRequest(req);
        if (!result.is_null())
        {
          results.push_back(result);
        }
      }

      return results.dump();
    }

    // If the JSON request is an object, then it's a single request.
    if (jsonRequest.is_object())
    {
      std::cout << "Handling request " << jsonRequest.dump() << std::endl;
      const nlohmann::json result = handleSingleRequest(jsonRequest);
      std::cout << "Request response: " << result.dump() << std::endl;
      return result.is_null() ? "" : result.dump();
    }

    // clang-format off
    return nlohmann::json {
      { "jsonrpc", "2.0" },
      { "id", nullptr },
      { "error", {
        { "code", JsonErrorType::INVALID_REQUEST },
        { "message", "invalid request: expected array or object" }
      }}
    }.dump();
    // clang-format on
  }

  catch (const nlohmann::json::parse_error& ex)
  {
    // clang-format off
    return nlohmann::json {
      { "jsonrpc", "2.0" },
      { "id", nullptr },
      { "error", {
        { "code", JsonErrorType::PARSE_ERROR },
        { "message", std::string("parse error: ") + ex.what() }
      }}
    }.dump();
    // clang-format on
  }
}

//-----
nlohmann::json TckServer::handleSingleRequest(const nlohmann::json& request)
{
  // Grab the ID if one exists for this request.
  nlohmann::json requestId = nullptr;
  if (hasValidId(request))
  {
    requestId = request["id"];
  }

  try
  {
    // Make sure the JSON RPC protocol is described and is exactly "2.0".
    if (!hasKeyType(request, "jsonrpc", nlohmann::json::value_t::string) || request["jsonrpc"] != "2.0")
    {
      throw JsonRpcException(JsonErrorType::INVALID_REQUEST, R"(invalid request: missing jsonrpc field set to "2.0")");
    }

    // Make sure the method field exists and contains a string.
    if (!hasKeyType(request, "method", nlohmann::json::value_t::string))
    {
      throw JsonRpcException(JsonErrorType::INVALID_REQUEST, "invalid request: method field must be a string");
    }

    // Make sure that, if there exists an ID field, that it is a number, string, or null.
    if (request.contains("id") && !request["id"].is_number() && !request["id"].is_string() && !request["id"].is_null())
    {
      throw JsonRpcException(JsonErrorType::INVALID_REQUEST,
                             "invalid request: id field must be a number, string or null");
    }

    // Make sure that, if a params field exists, it is an array, object, or null.
    if (request.contains("params") && !request["params"].is_array() && !request["params"].is_object() &&
        !request["params"].is_null())
    {
      throw JsonRpcException(JsonErrorType::INVALID_REQUEST,
                             "invalid request: params field must be an array, object or null");
    }

    const std::string& name = request["method"];
    const bool hasParams = request.contains("params") && !request["params"].empty();

    // If there's an ID, that indicates a method call.
    if (!requestId.is_null())
    {
      auto method = mMethods.find(name);
      if (method == mMethods.end())
      {
        throw JsonRpcException(JsonErrorType::METHOD_NOT_FOUND, "method not found: " + name);
      }

      return {
        {"jsonrpc", "2.0"                                                                  },
        { "id",     requestId                                                              },
        { "result", method->second(hasParams ? request["params"] : nlohmann::json::array())}
      };
    }

    // No request ID indicates a notification. Make sure the notification function exists.
    auto notification = mNotifications.find(name);
    if (notification == mNotifications.end())
    {
      throw JsonRpcException(JsonErrorType::METHOD_NOT_FOUND, "notification not found: " + name);
    }

    // Execute the notification function and return a null JSON object to signify the notification executed correctly.
    notification->second(hasParams ? request["params"] : nlohmann::json::array());
    return {};
  }

  // If there was a JsonRpcException, put its error data into a JSON object and return it.
  catch (const JsonRpcException& ex)
  {
    nlohmann::json error = {
      {"code",     ex.getCode()   },
      { "message", ex.getMessage()}
    };

    if (!ex.getData().is_null())
    {
      error["data"] = ex.getData();
    }

    return nlohmann::json{
      {"jsonrpc", "2.0"    },
      { "id",     requestId},
      { "error",  error    }
    };
  }

  // PrecheckStatusExceptions and ReceiptStatusExceptions should be Hiero errors.
  catch (const ReceiptStatusException& ex)
  {
    // clang-format off
    return {
      { "jsonrpc", "2.0" },
      { "id", requestId },
      { "error", {
        { "code", JsonErrorType::HIERO_ERROR },
        { "message", "Hiero error" },
        { "data", {
          { "status", gStatusToString.at(ex.mStatus) },
          { "message", ex.what() }
        }}
      }}
    };
    // clang-format on
  }

  catch (const PrecheckStatusException& ex)
  {
    // clang-format off
    return {
      { "jsonrpc", "2.0" },
      { "id", requestId },
      { "error", {
        { "code", JsonErrorType::HIERO_ERROR },
        { "message", "Hiero error" },
        { "data", {
          { "status", gStatusToString.at(ex.mStatus) },
          { "message", ex.what() }
        }}
      }}
    };
    // clang-format on
  }

  // If there was any other exception, resolve as an internal error.
  catch (const std::exception& ex)
  {
    // clang-format off
    return {
      { "jsonrpc", "2.0" },
      { "id",      requestId },
      { "error", {
        { "code", JsonErrorType::INTERNAL_ERROR },
        { "message", "Internal error" },
        { "data", {
          { "message", ex.what() }
        }}
      }}
    };
    // clang-format on
  }
}

//-----
void TckServer::setupHttpHandler()
{
  mHttpServer.Post("/",
                   [this](const httplib::Request& request, httplib::Response& response)
                   { handleHttpRequest(request, response); });
}

/**
 * Explicit template instantiations.
 */
template TckServer::MethodHandle TckServer::getHandle<AccountService::CreateAccountParams>(
  nlohmann::json (*method)(const AccountService::CreateAccountParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::DeleteAccountParams>(
  nlohmann::json (*method)(const AccountService::DeleteAccountParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::UpdateAccountParams>(
  nlohmann::json (*method)(const AccountService::UpdateAccountParams&));

template TckServer::MethodHandle TckServer::getHandle<KeyService::GenerateKeyParams>(
  nlohmann::json (*method)(const KeyService::GenerateKeyParams&));

template TckServer::MethodHandle TckServer::getHandle<SdkClient::ResetParams>(
  nlohmann::json (*method)(const SdkClient::ResetParams&));
template TckServer::MethodHandle TckServer::getHandle<SdkClient::SetupParams>(
  nlohmann::json (*method)(const SdkClient::SetupParams&));

template TckServer::MethodHandle TckServer::getHandle<TokenService::BurnTokenParams>(
  nlohmann::json (*method)(const TokenService::BurnTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::CreateTokenParams>(
  nlohmann::json (*method)(const TokenService::CreateTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::DeleteTokenParams>(
  nlohmann::json (*method)(const TokenService::DeleteTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::MintTokenParams>(
  nlohmann::json (*method)(const TokenService::MintTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::UpdateTokenParams>(
  nlohmann::json (*method)(const TokenService::UpdateTokenParams&));

} // namespace Hiero::TCK
