// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CONTRACT_INFO_QUERY_PARAMS_H_
#define HIERO_TCK_CPP_CONTRACT_INFO_QUERY_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ContractService
{
/**
 * Struct to hold the arguments of the `contractInfoQuery` JSON-RPC method call.
 */
struct ContractInfoQueryParams
{
  /**
   * The ID of the contract to query.
   */
  std::optional<std::string> mContractId;

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
 * JSON serializer template specialization required to convert ContractInfoQueryParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ContractService::ContractInfoQueryParams>
{
  /**
   * Convert a JSON object to ContractInfoQueryParams.
   *
   * @param jsonFrom The JSON object with which to fill ContractInfoQueryParams.
   * @param params   The ContractInfoQueryParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ContractService::ContractInfoQueryParams& params)
  {
    params.mContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contractId");
    params.mMaxQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "maxQueryPayment");
    params.mQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "queryPayment");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CONTRACT_INFO_QUERY_PARAMS_H_