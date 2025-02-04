// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_JSON_ERROR_TYPE_H_
#define HIERO_TCK_CPP_JSON_ERROR_TYPE_H_

namespace Hiero::TCK
{
/**
 * Enum used to describe an error seen while handling a JSON RPC request.
 */
enum class JsonErrorType : short
{
  /**
   * Invalid JSON was received by the server. An error occurred on the server while parsing the JSON text.
   */
  PARSE_ERROR = -32700,
  /**
   * The JSON sent is not a valid request object.
   */
  INVALID_REQUEST = -32600,
  /**
   * The method does not exist/is not available.
   */
  METHOD_NOT_FOUND = -32601,
  /**
   * Invalid method parameter(s).
   */
  INVALID_PARAMS = -32602,
  /**
   * Internal JSON-RPC error.
   */
  INTERNAL_ERROR = -32603,
  /**
   * The Hiero C++ SDK failed to execute the JSON request.
   */
  HIERO_ERROR = -32001
};

} // namespace Hiero::TCK

#endif // HIERO_TCK_CPP_JSON_ERROR_TYPE_H_
