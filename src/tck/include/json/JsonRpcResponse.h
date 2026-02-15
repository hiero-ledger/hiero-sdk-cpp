// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_JSON_RPC_RESPONSE_H_
#define HIERO_TCK_JSON_RPC_RESPONSE_H_

#include "JsonErrorType.h"

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK
{
/**
 * Utility class for constructing JSON-RPC 2.0 response objects.
 */
class JsonRpcResponse
{
public:
  /**
   * Construct a successful JSON-RPC 2.0 response.
   *
   * @param id     The request ID.
   * @param result The result payload.
   * @return The JSON response object.
   */
  static nlohmann::json makeSuccess(const nlohmann::json& id, const nlohmann::json& result);

  /**
   * Construct an error JSON-RPC 2.0 response.
   *
   * @param id      The request ID (may be nullptr).
   * @param code    The error code.
   * @param message The error message.
   * @param data    Optional additional error data.
   * @return The JSON response object.
   */
  static nlohmann::json makeError(const nlohmann::json& id,
                                  JsonErrorType code,
                                  const std::string& message,
                                  const nlohmann::json& data = nullptr);
};

} // namespace Hiero::TCK

#endif // HIERO_TCK_JSON_RPC_RESPONSE_H_
