// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CONTRACT_BYTE_CODE_QUERY_PARAMS_H_
#define HIERO_TCK_CPP_CONTRACT_BYTE_CODE_QUERY_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ContractService
{
/**
 * Struct to hold the arguments of the `contractByteCodeQuery` JSON-RPC method call.
 */
struct ContractByteCodeQueryParams
{
  /**
   * The ID of the contract of which to request the byte code.
   */
  std::optional<std::string> mContractId;

  /**
   * The maximum payment amount willing to be paid for this query in tinybars.
   */
  std::optional<std::string> mMaxQueryPayment;

  /**
   * The explicit payment amount for this query in tinybars.
   */
  std::optional<std::string> mQueryPayment;
};
} // namespace Hiero::TCK::ContractService

namespace nlohmann
{
/**
 * JSON serialiser template specialisation required to convert contractByteCodeQuery arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ContractService::ContractByteCodeQueryParams>
{
  /**
   * Convert a Json object to a ContractByteCodeQueryParams.
   *
   * @param jsonFrom The Json object with which to fill the ContractByteCodeQueryParams.
   * @param params The ContractByteCodeQueryParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ContractService::ContractByteCodeQueryParams& params)
  {
    params.mContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contractId");

    params.mMaxQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "maxQueryPayment");

    params.mQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "queryPayment");
  }
};
} // namespace nlohmann

#endif // HIERO_TCK_CPP_CONTRACT_BYTE_CODE_QUERY_PARAMS_H_
