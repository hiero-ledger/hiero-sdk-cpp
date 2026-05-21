// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_UPDATE_CONTRACT_PARAMS_H_
#define HIERO_TCK_CPP_UPDATE_CONTRACT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ContractService
{
/**
 * Struct to hold the arguments for an `updateContract` JSON-RPC method call.
 */
struct UpdateContractParams
{
  /**
   * The ID of the contract to update.
   */
  std::optional<std::string> mContractId;

  /**
   * The desired new admin key for the contract.
   */
  std::optional<std::string> mAdminKey;

  /**
   * The new expiration time to which to extend this contract.
   */
  std::optional<std::string> mExpirationTime;

  /**
   * The ID of the account to which the contract should auto-renew.
   */
  std::optional<std::string> mAutoRenewAccountId;

  /**
   * The desired new amount of time in seconds to renew the contract.
   */
  std::optional<std::string> mAutoRenewPeriod;

  /**
   * The desired new memo for the contract.
   */
  std::optional<std::string> mMemo;

  /**
   * The maximum number of automatic token associations for the contract.
   */
  std::optional<int32_t> mMaxAutomaticTokenAssociations;

  /**
   * The ID of the account to which the contract should stake.
   */
  std::optional<std::string> mStakedAccountId;

  /**
   * The ID of the node to which the contract should stake.
   */
  std::optional<std::string> mStakedNodeId;

  /**
   * Should the contract decline staking rewards?
   */
  std::optional<bool> mDeclineStakingReward;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::ContractService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UpdateContractParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ContractService::UpdateContractParams>
{
  /**
   * Convert a JSON object to a UpdateContractParams.
   *
   * @param jsonFrom The JSON object with which to fill the UpdateContractParams.
   * @param params   The UpdateContractParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ContractService::UpdateContractParams& params)
  {
    params.mContractId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contractId");
    params.mAdminKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "adminKey");
    params.mExpirationTime = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "expirationTime");
    params.mAutoRenewAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewAccountId");
    params.mAutoRenewPeriod = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewPeriod");
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mMaxAutomaticTokenAssociations =
      Hiero::TCK::getOptionalJsonParameter<int32_t>(jsonFrom, "maxAutomaticTokenAssociations");
    params.mStakedAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedAccountId");
    params.mStakedNodeId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedNodeId");
    params.mDeclineStakingReward = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "declineStakingReward");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_UPDATE_CONTRACT_PARAMS_H_
