// SPDX-License-Identifier: Apache-2.0
#include "json/JsonRpcRequest.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"

namespace Hiero::TCK
{
//-----
JsonRpcRequest::JsonRpcRequest(std::string method, nlohmann::json params, nlohmann::json id)
  : mMethod(std::move(method))
  , mParams(std::move(params))
  , mId(std::move(id))
{
}

//-----
JsonRpcRequest JsonRpcRequest::parse(const nlohmann::json& json)
{
  validateJsonRpcField(json);
  std::string method = validateAndExtractMethod(json);
  nlohmann::json id = validateAndExtractId(json);
  nlohmann::json params = validateAndExtractParams(json);

  return JsonRpcRequest(std::move(method), std::move(params), std::move(id));
}

//-----
void JsonRpcRequest::validateJsonRpcField(const nlohmann::json& json)
{
  if (!json.contains("jsonrpc") || json["jsonrpc"] != "2.0")
  {
    throw JsonRpcException(JsonErrorType::INVALID_REQUEST, R"(invalid request: missing jsonrpc field set to "2.0")");
  }
}

//-----
std::string JsonRpcRequest::validateAndExtractMethod(const nlohmann::json& json)
{
  if (!json.contains("method") || !json["method"].is_string())
  {
    throw JsonRpcException(JsonErrorType::INVALID_REQUEST, "invalid request: method field must be a string");
  }
  return json["method"].get<std::string>();
}

//-----
nlohmann::json JsonRpcRequest::validateAndExtractId(const nlohmann::json& json)
{
  if (!json.contains("id"))
  {
    return nullptr;
  }

  nlohmann::json id = json["id"];
  if (!id.is_string() && !id.is_number() && !id.is_null())
  {
    throw JsonRpcException(JsonErrorType::INVALID_REQUEST,
                           "invalid request: id field must be a number, string or null");
  }
  return id;
}

//-----
nlohmann::json JsonRpcRequest::validateAndExtractParams(const nlohmann::json& json)
{
  if (!json.contains("params"))
  {
    return nlohmann::json::object();
  }

  nlohmann::json params = json["params"];
  if (!params.is_array() && !params.is_object() && !params.is_null())
  {
    throw JsonRpcException(JsonErrorType::INVALID_REQUEST,
                           "invalid request: params field must be an array, object or null");
  }
  return params;
}

//-----
const std::string& JsonRpcRequest::getMethod() const
{
  return mMethod;
}

//-----
const nlohmann::json& JsonRpcRequest::getParams() const
{
  return mParams;
}

//-----
const nlohmann::json& JsonRpcRequest::getId() const
{
  return mId;
}

//-----
bool JsonRpcRequest::isNotification() const
{
  return mId.is_null();
}

} // namespace Hiero::TCK
