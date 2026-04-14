// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CREATE_CONTRACT_PARAMS_H_
#define HIERO_TCK_CPP_CREATE_CONTRACT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ContractService
{
/**
 * Struct to hold the arguments for a `createContract` JSON-RPC method call.
 */
struct CreateContractParams
{
  /**
   * ID of the file containing contract bytecode.
   */
  std::optional<std::string> mBytecodeFileId;

  /**
   * Gas limit for contract creation.
   */
  std::optional<int64_t> mGas;

  /**
   * Admin key controlling update/delete operations for the contract.
   */
  std::optional<std::string> mAdminKey;

  /**
   * ABI-encoded constructor parameters as hex-encoded bytes.
   */
  std::optional<std::string> mConstructorParameters;

  /**
   * Initial balance in tinybars.
   */
  std::optional<std::string> mInitialBalance;

  /**
   * Memo for the contract.
   */
  std::optional<std::string> mMemo;

  /**
   * Auto-renew account ID.
   */
  std::optional<std::string> mAutoRenewAccountId;

  /**
   * Auto-renew period in seconds.
   */
  std::optional<std::string> mAutoRenewPeriod;

  /**
   * ID of the account to stake to.
   */
  std::optional<std::string> mStakedAccountId;

  /**
   * ID of the node to stake to.
   */
  std::optional<std::string> mStakedNodeId;

  /**
   * Whether the contract should decline staking rewards.
   */
  std::optional<bool> mDeclineStakingReward;

  /**
   * Maximum number of automatic token associations.
   */
  std::optional<int32_t> mMaxAutomaticTokenAssociations;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::ContractService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CreateContractParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ContractService::CreateContractParams>
{
  /**
   * Convert a JSON object to a CreateContractParams.
   *
   * @param jsonFrom The JSON object with which to fill the CreateContractParams.
   * @param params   The CreateContractParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ContractService::CreateContractParams& params)
  {
    params.mBytecodeFileId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "bytecodeFileId");
    params.mGas = Hiero::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "gas");
    params.mAdminKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "adminKey");
    params.mConstructorParameters =
      Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "constructorParameters");
    params.mInitialBalance = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "initialBalance");
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mAutoRenewAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewAccountId");
    params.mAutoRenewPeriod = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewPeriod");
    params.mStakedAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedAccountId");
    params.mStakedNodeId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedNodeId");
    params.mDeclineStakingReward = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "declineStakingReward");
    params.mMaxAutomaticTokenAssociations =
      Hiero::TCK::getOptionalJsonParameter<int32_t>(jsonFrom, "maxAutomaticTokenAssociations");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CREATE_CONTRACT_PARAMS_H_
