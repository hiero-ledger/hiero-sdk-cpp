// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CONTRACT_CALL_QUERY_PARAMS_H_
#define HIERO_TCK_CPP_CONTRACT_CALL_QUERY_PARAMS_H_

#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ContractService
{
/**
 * Struct to hold the arguments of the `contractCallQuery` JSON-RPC method call.
 */
struct ContractCallQueryParams
{
  /**
   * The ID of the contract to query.
   */
  std::optional<std::string> mContractId;

  /**
   * Gas limit for the contract call query.
   */
  std::optional<int64_t> mGas;

  /**
   * The function name for the call.
   */
  std::optional<std::string> mFunctionName;

  /**
   * ABI-encoded function parameters as a hex string.
   */
  std::optional<std::string> mFunctionParameters;

  /**
   * The maximum payment amount in tinybars willing to be paid for this query.
   */
  std::optional<std::string> mMaxQueryPayment;

  /**
   * The exact query payment amount in tinybars.
   */
  std::optional<std::string> mQueryPayment;
};

} // namespace Hiero::TCK::ContractService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert ContractCallQueryParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ContractService::ContractCallQueryParams>
{
  /**
   * Convert a JSON object to ContractCallQueryParams.
   *
   * @param jsonFrom The JSON object with which to fill ContractCallQueryParams.
   * @param params   The ContractCallQueryParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ContractService::ContractCallQueryParams& params)
  {
    params.mContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contractId");
    params.mGas = Hiero::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "gas");
    params.mFunctionName = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "functionName");
    params.mFunctionParameters = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "functionParameters");
    params.mMaxQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "maxQueryPayment");
    params.mQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "queryPayment");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CONTRACT_CALL_QUERY_PARAMS_H_