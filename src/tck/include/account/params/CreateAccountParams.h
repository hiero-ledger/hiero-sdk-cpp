// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CREATE_ACCOUNT_PARAMS_H_
#define HIERO_TCK_CPP_CREATE_ACCOUNT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for a `createAccount` JSON-RPC method call.
 */
struct CreateAccountParams
{
  /**
   * The desired key for the account.
   */
  std::optional<std::string> mKey;

  /**
   * The desired initial balance for the account.
   */
  std::optional<std::string> mInitialBalance;

  /**
   * Should the new account require a receiver signature?
   */
  std::optional<bool> mReceiverSignatureRequired;

  /**
   * The desired amount of time in seconds to renew the new account.
   */
  std::optional<std::string> mAutoRenewPeriod;

  /**
   * The desired memo for the new account.
   */
  std::optional<std::string> mMemo;

  /**
   * The desired maximum number of automatic token associations for the account.
   */
  std::optional<int32_t> mMaxAutoTokenAssociations;

  /**
   * The ID of the desired account to which the new account should stake.
   */
  std::optional<std::string> mStakedAccountId;

  /**
   * The ID of the desired node to which the new account should stake.
   */
  std::optional<std::string> mStakedNodeId;

  /**
   * Should the new account decline staking rewards?
   */
  std::optional<bool> mDeclineStakingReward;

  /**
   * The desired alias for the new account.
   */
  std::optional<std::string> mAlias;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CreateAccountParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::CreateAccountParams>
{
  /**
   * Convert a JSON object to a CreateAccountParams.
   *
   * @param jsonFrom The JSON object with which to fill the CreateAccountParams.
   * @param params   The CreateAccountParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::CreateAccountParams& params)
  {
    params.mKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "key");
    params.mInitialBalance = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "initialBalance");
    params.mReceiverSignatureRequired =
      Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "receiverSignatureRequired");
    params.mAutoRenewPeriod = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewPeriod");
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mMaxAutoTokenAssociations =
      Hiero::TCK::getOptionalJsonParameter<int32_t>(jsonFrom, "maxAutoTokenAssociations");
    params.mStakedAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedAccountId");
    params.mStakedNodeId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedNodeId");
    params.mDeclineStakingReward = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "declineStakingReward");
    params.mAlias = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "alias");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CREATE_ACCOUNT_PARAMS_H_
