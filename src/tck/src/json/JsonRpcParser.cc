// SPDX-License-Identifier: Apache-2.0
#include "json/JsonRpcParser.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonRpcRequest.h"
#include "json/JsonRpcResponse.h"

#include <Status.h>
#include <exceptions/PrecheckStatusException.h>
#include <exceptions/ReceiptStatusException.h>

namespace Hiero::TCK
{
//-----
void JsonRpcParser::addMethod(const std::string& name, MethodHandler handler)
{
  if (name.find("rpc.") == 0)
  {
    throw std::invalid_argument(R"(invalid method name: method name must not contain "rpc.")");
  }

  if (mMethods.find(name) != mMethods.end())
  {
    throw std::invalid_argument("invalid method name: method already added");
  }

  mMethods[name] = std::move(handler);
}

//-----
std::string JsonRpcParser::handle(const std::string& body)
{
  try
  {
    auto jsonRequest = nlohmann::json::parse(body);

    if (jsonRequest.is_array())
    {
      return handleBatchRequest(jsonRequest);
    }

    if (jsonRequest.is_object())
    {
      nlohmann::json result = handleSingleRequest(jsonRequest);
      return result.is_null() ? "" : result.dump();
    }

    return JsonRpcResponse::makeError(
             nullptr, JsonErrorType::INVALID_REQUEST, "invalid request: expected array or object")
      .dump();
  }
  catch (const nlohmann::json::parse_error& ex)
  {
    return JsonRpcResponse::makeError(nullptr, JsonErrorType::PARSE_ERROR, std::string("parse error: ") + ex.what())
      .dump();
  }
}

//-----
std::string JsonRpcParser::handleBatchRequest(const nlohmann::json& batchRequest)
{
  if (batchRequest.empty())
  {
    return JsonRpcResponse::makeError(nullptr, JsonErrorType::INVALID_REQUEST, "invalid request: empty batch").dump();
  }

  nlohmann::json batchResponse = nlohmann::json::array();
  for (const auto& req : batchRequest)
  {
    nlohmann::json result = handleSingleRequest(req);
    if (!result.is_null())
    {
      batchResponse.push_back(result);
    }
  }

  return batchResponse.empty() ? "" : batchResponse.dump();
}

//-----
nlohmann::json JsonRpcParser::handleSingleRequest(const nlohmann::json& requestJson)
{
  nlohmann::json requestId = nullptr;

  try
  {
    JsonRpcRequest request = JsonRpcRequest::parse(requestJson);
    requestId = request.getId();

    auto it = mMethods.find(request.getMethod());
    if (it == mMethods.end())
    {
      throw JsonRpcException(JsonErrorType::METHOD_NOT_FOUND, "method not found: " + request.getMethod());
    }

    nlohmann::json result = it->second(request.getParams());

    if (request.isNotification())
    {
      return nullptr;
    }

    return JsonRpcResponse::makeSuccess(requestId, result);
  }
  catch (const JsonRpcException& e)
  {
    return JsonRpcResponse::makeError(requestId, e.getCode(), e.getMessage(), e.getData());
  }
  catch (const std::exception& ex)
  {
    return createExceptionErrorResponse(requestId, ex);
  }
}

//-----
nlohmann::json JsonRpcParser::createExceptionErrorResponse(const nlohmann::json& requestId, const std::exception& ex)
{
  if (const auto* receiptEx = dynamic_cast<const Hiero::ReceiptStatusException*>(&ex))
  {
    return JsonRpcResponse::makeError(
      requestId,
      JsonErrorType::HIERO_ERROR,
      "Hiero error",
      nlohmann::json{
        {"status",   gStatusToString.at(receiptEx->mStatus)},
        { "message", receiptEx->what()                     }
    });
  }

  if (const auto* precheckEx = dynamic_cast<const Hiero::PrecheckStatusException*>(&ex))
  {
    return JsonRpcResponse::makeError(
      requestId,
      JsonErrorType::HIERO_ERROR,
      "Hiero error",
      nlohmann::json{
        {"status",   gStatusToString.at(precheckEx->mStatus)},
        { "message", precheckEx->what()                     }
    });
  }

  return JsonRpcResponse::makeError(requestId,
                                    JsonErrorType::INTERNAL_ERROR,
                                    "Internal error",
                                    nlohmann::json{
                                      {"message", ex.what()}
  });
}

} // namespace Hiero::TCK
