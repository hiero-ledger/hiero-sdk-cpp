// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_JSON_RPC_PARSER_H_
#define HIERO_TCK_JSON_RPC_PARSER_H_

#include <functional>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK
{
/**
 * Central utility for parsing JSON-RPC 2.0 requests and dispatching them to registered handlers.
 *
 * Handles protocol validation, method dispatch, batch requests, and error formatting
 * according to the JSON-RPC 2.0 specification.
 */
class JsonRpcParser
{
public:
  /**
   * Callback type for registered methods. Takes JSON params and returns a JSON result.
   */
  using MethodHandler = std::function<nlohmann::json(const nlohmann::json&)>;

  /**
   * Register a method handler.
   *
   * @param name    The JSON-RPC method name.
   * @param handler The function to execute when this method is called.
   * @throws std::invalid_argument If the name starts with "rpc." or is already registered.
   */
  void addMethod(const std::string& name, MethodHandler handler);

  /**
   * Parse and handle a raw JSON-RPC request string (single or batch).
   *
   * @param body The raw JSON string from the HTTP request body.
   * @return The JSON-RPC response string, or empty string for notification-only requests.
   */
  std::string handle(const std::string& body);

private:
  /**
   * Handle a single parsed JSON-RPC request object.
   *
   * @param requestJson The parsed JSON object.
   * @return The JSON response object, or nullptr for notifications.
   */
  nlohmann::json handleSingleRequest(const nlohmann::json& requestJson);

  /**
   * Handle a batch of JSON-RPC requests.
   *
   * @param batchRequest The parsed JSON array of requests.
   * @return The JSON-RPC batch response string.
   */
  std::string handleBatchRequest(const nlohmann::json& batchRequest);

  /**
   * Create an error response for an exception.
   *
   * @param requestId The request ID.
   * @param ex The exception.
   * @return The JSON error response object.
   */
  static nlohmann::json createExceptionErrorResponse(const nlohmann::json& requestId, const std::exception& ex);

  /**
   * Map of registered method names to their handlers.
   */
  std::map<std::string, MethodHandler> mMethods;
};

} // namespace Hiero::TCK

#endif // HIERO_TCK_JSON_RPC_PARSER_H_
