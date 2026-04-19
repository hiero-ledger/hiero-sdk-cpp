// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_DELETE_CONTRACT_PARAMS_H_
#define HIERO_TCK_CPP_DELETE_CONTRACT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ContractService
{
/**
 * Struct to hold the arguments for a `deleteContract` JSON-RPC method call.
 */
struct DeleteContractParams
{
  /**
   * The ID of the contract to delete.
   */
  std::optional<std::string> mContractId;

  /**
   * The ID of the account to which to transfer remaining balances.
   */
  std::optional<std::string> mTransferAccountId;

  /**
   * The ID of the contract to which to transfer remaining balances.
   */
  std::optional<std::string> mTransferContractId;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;

  /**
   * If set to true, marks this as a system-initiated permanent removal
   * (should always fail for user transactions).
   */
  std::optional<bool> mPermanentRemoval;
};

} // namespace Hiero::TCK::ContractService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert DeleteContractParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ContractService::DeleteContractParams>
{
  /**
   * Convert a JSON object to a DeleteContractParams.
   *
   * @param jsonFrom The JSON object with which to fill the DeleteContractParams.
   * @param params   The DeleteContractParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ContractService::DeleteContractParams& params)
  {
    params.mContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contractId");
    params.mTransferAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "transferAccountId");
    params.mTransferContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "transferContractId");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
    params.mPermanentRemoval = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "permanentRemoval");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_DELETE_CONTRACT_PARAMS_H_
