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
#ifndef HEDERA_TCK_CPP_CREATE_ACCOUNT_PARAMS_H_
#define HEDERA_TCK_CPP_CREATE_ACCOUNT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hedera::TCK::AccountService
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
  std::optional<int64_t> mInitialBalance;

  /**
   * Should the new account require a receiver signature?
   */
  std::optional<bool> mReceiverSignatureRequired;

  /**
   * The desired amount of time in seconds to renew the new account.
   */
  std::optional<int64_t> mAutoRenewPeriod;

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
  std::optional<int64_t> mStakedNodeId;

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

} // namespace Hedera::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CreateAccountParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hedera::TCK::AccountService::CreateAccountParams>
{
  /**
   * Convert a JSON object to a CreateAccountParams.
   *
   * @param jsonFrom The JSON object with which to fill the CreateAccountParams.
   * @param params   The CreateAccountParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hedera::TCK::AccountService::CreateAccountParams& params)
  {
    params.mKey = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "key");
    params.mInitialBalance = Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "initialBalance");
    params.mReceiverSignatureRequired =
      Hedera::TCK::getOptionalJsonParameter<bool>(jsonFrom, "receiverSignatureRequired");
    params.mAutoRenewPeriod = Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "autoRenewPeriod");
    params.mMemo = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mMaxAutoTokenAssociations =
      Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "maxAutoTokenAssociations");
    params.mStakedAccountId = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "stakedAccountId");
    params.mStakedNodeId = Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "stakedNodeId");
    params.mDeclineStakingReward = Hedera::TCK::getOptionalJsonParameter<bool>(jsonFrom, "declineStakingReward");
    params.mAlias = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "alias");
    params.mCommonTxParams =
      Hedera::TCK::getOptionalJsonParameter<Hedera::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HEDERA_TCK_CPP_CREATE_ACCOUNT_PARAMS_H_
