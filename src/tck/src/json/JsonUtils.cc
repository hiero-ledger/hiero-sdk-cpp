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
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <string_view>

namespace Hedera::TCK
{
//-----
bool hasKeyType(const nlohmann::json& request, std::string_view key, nlohmann::json::value_t type)
{
  return request.contains(key) && request.at(key).type() == type;
}

//-----
bool hasValidId(const nlohmann::json& request)
{
  return request.contains("id") && (request["id"].is_number() || request["id"].is_string() || request["id"].is_null());
}

} // namespace Hedera::TCK
