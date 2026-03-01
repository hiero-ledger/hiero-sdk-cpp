// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_ACCOUNT_BALANCE_PARAMS_H_
#define HIERO_TCK_CPP_GET_ACCOUNT_BALANCE_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for a  `getAccountBalance` JSON RPC method call.
 */
struct GetAccountBalanceParams
{
  /**
   * The ID of the account to get the Balance.
   */
  std::optional<std::string> mAccountId;

  /**
   * The ID of the contract to query.
   */
  std::optional<std::string> mContractId;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serialiser template specialisation is required to conver the GetAccountBalance arguements properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::GetAccountBalanceParams>
{
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::GetAccountBalanceParams& params)
  {
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contractId");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_ACCOUNT_BALANCE_PARAMS_H_
