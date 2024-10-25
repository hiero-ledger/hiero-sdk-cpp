/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef HEDERA_TCK_CPP_UPDATE_ACCOUNT_PARAMS_H_
#define HEDERA_TCK_CPP_UPDATE_ACCOUNT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace Hedera::TCK::AccountService
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
  std::optional<int64_t> mAutoRenewPeriod;

  /**
   * The new expiration time to which to extend this account.
   */
  std::optional<int64_t> mExpirationTime;

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
  std::optional<int64_t> mStakedNodeId;

  /**
   * Should the account now decline staking rewards?
   */
  std::optional<bool> mDeclineStakingReward;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hedera::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UpdateAccountParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hedera::TCK::AccountService::UpdateAccountParams>
{
  /**
   * Convert a JSON object to a UpdateAccountParams.
   *
   * @param jsonFrom The JSON object with which to fill the UpdateAccountParams.
   * @param params   The UpdateAccountParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hedera::TCK::AccountService::UpdateAccountParams& params)
  {
    params.mAccountId = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mKey = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "key");
    params.mAutoRenewPeriod = Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "autoRenewPeriod");
    params.mExpirationTime = Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "expirationTime");
    params.mReceiverSignatureRequired =
      Hedera::TCK::getOptionalJsonParameter<bool>(jsonFrom, "receiverSignatureRequired");
    params.mMemo = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mMaxAutoTokenAssociations =
      Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "maxAutoTokenAssociations");
    params.mStakedAccountId = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedAccountId");
    params.mStakedNodeId = Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "stakedNodeId");
    params.mDeclineStakingReward = Hedera::TCK::getOptionalJsonParameter<bool>(jsonFrom, "declineStakingReward");
    params.mCommonTxParams =
      Hedera::TCK::getOptionalJsonParameter<Hedera::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HEDERA_TCK_CPP_UPDATE_ACCOUNT_PARAMS_H_
