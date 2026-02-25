// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_ACCOUNT_INFO_PARAMS_H_
#define HIERO_TCK_CPP_GET_ACCOUNT_INFO_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for a `getAccountInfo` JSON-RPC method call.
 */
struct GetAccountInfoParams
{
  /**
   * The ID of the account to query.
   */
  std::optional<std::string> mAccountId;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert GetAccountInfoParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::GetAccountInfoParams>
{
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::GetAccountInfoParams& params)
  {
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_ACCOUNT_INFO_PARAMS_H_
