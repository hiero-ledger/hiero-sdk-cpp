// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_UPDATE_ACCOUNT_PARAMS_H_
#define HIERO_TCK_CPP_UPDATE_ACCOUNT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for an `updateAccount` JSON-RPC method call.
 */
struct UpdateAccountParams
{
  /**
   * The ID of the account to update.
   */
  std::optional<std::string> mAccountId;

  /**
   * The desired new key for the account.
   */
  std::optional<std::string> mKey;

  /**
   * The desired new amount of time in seconds to renew the account.
   */
  std::optional<std::string> mAutoRenewPeriod;

  /**
   * The new expiration time to which to extend this account.
   */
  std::optional<std::string> mExpirationTime;

  /**
   * Should the account now require a receiver signature?
   */
  std::optional<bool> mReceiverSignatureRequired;

  /**
   * The desired new memo for the account.
   */
  std::optional<std::string> mMemo;

  /**
   * The desired new maximum number of automatic token associations for the account.
   */
  std::optional<int32_t> mMaxAutoTokenAssociations;

  /**
   * The ID of the new desired account to which the account should stake.
   */
  std::optional<std::string> mStakedAccountId;

  /**
   * The ID of the new desired node to which the account should stake.
   */
  std::optional<std::string> mStakedNodeId;

  /**
   * Should the account now decline staking rewards?
   */
  std::optional<bool> mDeclineStakingReward;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UpdateAccountParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::UpdateAccountParams>
{
  /**
   * Convert a JSON object to a UpdateAccountParams.
   *
   * @param jsonFrom The JSON object with which to fill the UpdateAccountParams.
   * @param params   The UpdateAccountParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::UpdateAccountParams& params)
  {
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "key");
    params.mAutoRenewPeriod = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewPeriod");
    params.mExpirationTime = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "expirationTime");
    params.mReceiverSignatureRequired =
      Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "receiverSignatureRequired");
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mMaxAutoTokenAssociations =
      Hiero::TCK::getOptionalJsonParameter<int32_t>(jsonFrom, "maxAutoTokenAssociations");
    params.mStakedAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedAccountId");
    params.mStakedNodeId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedNodeId");
    params.mDeclineStakingReward = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "declineStakingReward");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_UPDATE_ACCOUNT_PARAMS_H_
