// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_JSON_RPC_REQUEST_H_
#define HIERO_TCK_JSON_RPC_REQUEST_H_

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK
{
/**
 * Represents a parsed and validated JSON-RPC 2.0 Request.
 * Format: { "jsonrpc": "2.0", "method": "name", "params": {...}, "id": 1 }
 */
class JsonRpcRequest
{
public:
  /**
   * Parses and validates a JSON object.
   * @param json The raw JSON object from the HTTP body.
   * @return A valid JsonRpcRequest object.
   * @throws JsonRpcException if the request is invalid.
   */
  static JsonRpcRequest parse(const nlohmann::json& json);

  [[nodiscard]] const std::string& getMethod() const;
  [[nodiscard]] const nlohmann::json& getParams() const;
  [[nodiscard]] const nlohmann::json& getId() const;
  [[nodiscard]] bool isNotification() const;

private:
  // Private constructor forces users to use parse()
  JsonRpcRequest(std::string method, nlohmann::json params, nlohmann::json id);

  // Helper methods to reduce cyclomatic complexity
  static void validateJsonRpcField(const nlohmann::json& json);
  static std::string validateAndExtractMethod(const nlohmann::json& json);
  static nlohmann::json validateAndExtractId(const nlohmann::json& json);
  static nlohmann::json validateAndExtractParams(const nlohmann::json& json);

  std::string mMethod;
  nlohmann::json mParams;
  nlohmann::json mId;
};
} // namespace Hiero::TCK

#endif // HIERO_TCK_JSON_RPC_REQUEST_H_
