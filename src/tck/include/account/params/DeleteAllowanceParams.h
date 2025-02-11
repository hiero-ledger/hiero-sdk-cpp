// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_DELETE_ALLOWANCE_PARAMS_H_
#define HIERO_TCK_CPP_DELETE_ALLOWANCE_PARAMS_H_

#include "account/params/allowance/RemoveAllowanceParams.h"
#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for a `deleteAllowance` JSON-RPC method call.
 */
struct DeleteAllowanceParams
{
  /**
   * The allowances to delete.
   */
  std::vector<RemoveAllowanceParams> mAllowances;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert DeleteAllowanceParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::DeleteAllowanceParams>
{
  /**
   * Convert a JSON object to a DeleteAllowanceParams.
   *
   * @param jsonFrom The JSON object with which to fill the DeleteAllowanceParams.
   * @param params   The DeleteAllowanceParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::DeleteAllowanceParams& params)
  {
    params.mAllowances =
      Hiero::TCK::getRequiredJsonParameter<std::vector<Hiero::TCK::AccountService::RemoveAllowanceParams>>(
        jsonFrom, "allowances");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_DELETE_ALLOWANCE_PARAMS_H_
