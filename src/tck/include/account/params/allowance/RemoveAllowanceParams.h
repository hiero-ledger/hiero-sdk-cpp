// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_REMOVE_ALLOWANCE_PARAMS_H_
#define HIERO_TCK_CPP_REMOVE_ALLOWANCE_PARAMS_H_

#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct that contains the parameters to remove an allowance.
 */
struct RemoveAllowanceParams
{
  /**
   * The ID of the owner account removing the allowance.
   */
  std::string mOwnerAccountId;

  /**
   * The ID of the token of which to remove the allowance.
   */
  std::string mTokenId;

  /**
   * The serial numbers of the tokens of which to remove the allowance
   */
  std::vector<std::string> mSerialNumbers;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert RemoveAllowanceParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::RemoveAllowanceParams>
{
  /**
   * Convert a JSON object to a RemoveAllowanceParams.
   *
   * @param jsonFrom The JSON object with which to fill the RemoveAllowanceParams.
   * @param params   The RemoveAllowanceParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::RemoveAllowanceParams& params)
  {
    params.mOwnerAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "ownerAccountId");
    params.mTokenId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mSerialNumbers = Hiero::TCK::getRequiredJsonParameter<std::vector<std::string>>(jsonFrom, "serialNumbers");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_REMOVE_ALLOWANCE_PARAMS_H_
