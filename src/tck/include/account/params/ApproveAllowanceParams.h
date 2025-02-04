// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_APPROVE_ALLOWANCE_PARAMS_H_
#define HIERO_TCK_CPP_APPROVE_ALLOWANCE_PARAMS_H_

#include "account/params/allowance/AllowanceParams.h"
#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for a `approveAllowance` JSON-RPC method call.
 */
struct ApproveAllowanceParams
{
  /**
   * The allowances to approve.
   */
  std::vector<AllowanceParams> mAllowances;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert ApproveAllowanceParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::ApproveAllowanceParams>
{
  /**
   * Convert a JSON object to a ApproveAllowanceParams.
   *
   * @param jsonFrom The JSON object with which to fill the ApproveAllowanceParams.
   * @param params   The ApproveAllowanceParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::ApproveAllowanceParams& params)
  {
    params.mAllowances = Hiero::TCK::getRequiredJsonParameter<std::vector<Hiero::TCK::AccountService::AllowanceParams>>(
      jsonFrom, "allowances");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_APPROVE_ALLOWANCE_PARAMS_H_
