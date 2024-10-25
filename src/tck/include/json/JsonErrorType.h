/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef HEDERA_TCK_CPP_JSON_ERROR_TYPE_H_
#define HEDERA_TCK_CPP_JSON_ERROR_TYPE_H_

namespace Hedera::TCK
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
   * The Hedera C++ SDK failed to execute the JSON request.
   */
  HEDERA_ERROR = -32001
};

} // namespace Hedera::TCK

#endif // HEDERA_TCK_CPP_JSON_ERROR_TYPE_H_
