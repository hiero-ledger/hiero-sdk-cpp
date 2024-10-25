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
#ifndef HEDERA_TCK_CPP_JSON_UTILS_H_
#define HEDERA_TCK_CPP_JSON_UTILS_H_

#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"

#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>

namespace Hedera::TCK
{
/**
 * Get a required parameter from a JSON object.
 *
 * @tparam T    The C++ type of parameter to get.
 * @param  json The JSON from which to get the parameter.
 * @param  name The name of the parameter to get.
 * @return The parameter as the specified type.
 * @throws JsonRpcException If the parameter doesn't exist or is not the specified type.
 */
template<typename T>
T getRequiredJsonParameter(const nlohmann::json& json, std::string_view name)
{
  if (!json.contains(name))
  {
    throw JsonRpcException(JsonErrorType::INVALID_PARAMS, "invalid parameters: " + std::string(name) + " is required");
  }

  try
  {
    return json[name].get<T>();
  }
  catch (const nlohmann::json::type_error& err)
  {
    throw JsonRpcException(JsonErrorType::INVALID_PARAMS, "invalid parameters: " + std::string(err.what()));
  }
}

/**
 * Get an optional parameter from a JSON object.
 *
 * @tparam T    The C++ type of parameter to get.
 * @param  json The JSON from which to get the parameter.
 * @param  name The name of the parameter to get.
 * @return The parameter as the specified type, std::nullopt if the parameter doesn't exist.
 * @throws JsonRpcException If the parameter doesn't exist or is not the specified type.
 */
template<typename T>
std::optional<T> getOptionalJsonParameter(const nlohmann::json& json, std::string_view name)
{
  if (!json.contains(name))
  {
    return std::optional<T>(std::nullopt);
  }

  try
  {
    return json[name].get<T>();
  }
  catch (const nlohmann::json::type_error& err)
  {
    throw JsonRpcException(JsonErrorType::INVALID_PARAMS, "invalid parameters: " + std::string(err.what()));
  }
}

/**
 * Does a JSON request have a particular key of a particular type?
 *
 * @param request The JSON request used to determine whether it contains the key.
 * @param key     The key for which to check.
 * @param type    The type of the key.
 * @return \c TRUE if the JSON request contains the key with the type, otherwise \c FALSE.
 */
bool hasKeyType(const nlohmann::json& request, std::string_view key, nlohmann::json::value_t type);

/**
 * Does a JSON request have a valid ID parameter?
 *
 * @param request The JSON request used to determine whether it contains a valid ID.
 * @return \c TRUE if the JSON request has a valid ID, otherwise \c FALSE.
 */
bool hasValidId(const nlohmann::json& request);

} // namespace Hedera::TCK

#endif // HEDERA_TCK_CPP_JSON_UTILS_H_
