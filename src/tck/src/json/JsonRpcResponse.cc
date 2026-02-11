// SPDX-License-Identifier: Apache-2.0
#include "json/JsonRpcResponse.h"

namespace Hiero::TCK
{
//-----
nlohmann::json JsonRpcResponse::makeSuccess(const nlohmann::json& id, const nlohmann::json& result)
{
  return {
    {"jsonrpc", "2.0" },
    { "id",     id    },
    { "result", result}
  };
}

//-----
nlohmann::json JsonRpcResponse::makeError(const nlohmann::json& id,
                                          int code,
                                          const std::string& message,
                                          const nlohmann::json& data)
{
  nlohmann::json errorObj = {
    {"code",     code   },
    { "message", message}
  };

  if (!data.is_null())
  {
    errorObj["data"] = data;
  }

  return {
    {"jsonrpc", "2.0"   },
    { "error",  errorObj},
    { "id",     id      }
  };
}

} // namespace Hiero::TCK
