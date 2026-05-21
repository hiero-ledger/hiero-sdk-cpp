// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_EXECUTE_CONTRACT_PARAMS_H_
#define HIERO_TCK_CPP_EXECUTE_CONTRACT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ContractService
{
/**
 * Struct to hold the arguments for an `executeContract` JSON-RPC method call.
 */
struct ExecuteContractParams
{
  /**
   * The ID of the contract to call.
   */
  std::optional<std::string> mContractId;

  /**
   * The maximum amount of gas to use for the function call.
   */
  std::optional<int64_t> mGas;

  /**
   * The amount to pay for the function call, in tinybars.
   */
  std::optional<std::string> mAmount;

  /**
   * The hex-encoded function parameters to pass to the function call.
   */
  std::optional<std::string> mFunctionParameters;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::ContractService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert ExecuteContractParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ContractService::ExecuteContractParams>
{
  /**
   * Convert a JSON object to an ExecuteContractParams.
   *
   * @param jsonFrom The JSON object with which to fill the ExecuteContractParams.
   * @param params   The ExecuteContractParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ContractService::ExecuteContractParams& params)
  {
    params.mContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contractId");
    params.mGas = Hiero::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "gas");
    params.mAmount = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "amount");
    params.mFunctionParameters = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "functionParameters");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_EXECUTE_CONTRACT_PARAMS_H_
