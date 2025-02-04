// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOKEN_ALLOWANCE_PARAMS_H_
#define HIERO_TCK_CPP_TOKEN_ALLOWANCE_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct that contains the parameters of a token allowance.
 */
struct TokenAllowanceParams
{
  /**
   * The ID of the token being allowanced.
   */
  std::string mTokenId;

  /**
   * The amount of the token to allowance.
   */
  std::string mAmount;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert TokenAllowanceParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::TokenAllowanceParams>
{
  /**
   * Convert a JSON object to a TokenAllowanceParams.
   *
   * @param jsonFrom The JSON object with which to fill the TokenAllowanceParams.
   * @param params   The TokenAllowanceParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::TokenAllowanceParams& params)
  {
    params.mTokenId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mAmount = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "amount");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_ALLOWANCE_PARAMS_H_
