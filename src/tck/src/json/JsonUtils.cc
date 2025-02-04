// SPDX-License-Identifier: Apache-2.0
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <string_view>

namespace Hiero::TCK
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

} // namespace Hiero::TCK
