// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_HBAR_ALLOWANCE_PARAMS_H_
#define HIERO_TCK_CPP_HBAR_ALLOWANCE_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct that contains the parameters of an Hbar allowance.
 */
struct HbarAllowanceParams
{
  /**
   * The amount of Hbar to allowance.
   */
  std::string mAmount;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert HbarAllowanceParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::HbarAllowanceParams>
{
  /**
   * Convert a JSON object to a HbarAllowanceParams.
   *
   * @param jsonFrom The JSON object with which to fill the HbarAllowanceParams.
   * @param params   The HbarAllowanceParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::HbarAllowanceParams& params)
  {
    params.mAmount = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "amount");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_HBAR_ALLOWANCE_PARAMS_H_
