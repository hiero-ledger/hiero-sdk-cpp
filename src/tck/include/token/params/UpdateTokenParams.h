// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_UPDATE_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_UPDATE_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "token/CustomFeeSerializer.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `updateToken` JSON-RPC method call.
 */
struct UpdateTokenParams
{
  /**
   * The ID of the token to update.
   */
  std::optional<std::string> mTokenId;

  /**
   * The desired new symbol of the token.
   */
  std::optional<std::string> mSymbol;

  /**
   * The desired new name of the token.
   */
  std::optional<std::string> mName;

  /**
   * The ID of the desired new treasury account of the token.
   */
  std::optional<std::string> mTreasuryAccountId;

  /**
   * The desired new admin key of the token.
   */
  std::optional<std::string> mAdminKey;

  /**
   * The desired new KYC key of the token.
   */
  std::optional<std::string> mKycKey;

  /**
   * The desired new freeze key of the token.
   */
  std::optional<std::string> mFreezeKey;

  /**
   * The desired new wipe key of the token.
   */
  std::optional<std::string> mWipeKey;

  /**
   * The desired new supply key of the token.
   */
  std::optional<std::string> mSupplyKey;

  /**
   * The ID of the desired account to now pay auto-renewal fees.
   */
  std::optional<std::string> mAutoRenewAccountId;

  /**
   * The desired new interval of time for which to charge the auto-renew account to renew the token.
   *
   */
  std::optional<std::string> mAutoRenewPeriod;

  /**
   * The desired new expiration time of the token.
   */
  std::optional<std::string> mExpirationTime;

  /**
   * The desired new memo of the token.
   */
  std::optional<std::string> mMemo;

  /**
   * The desired new fee schedule key of the token.
   */
  std::optional<std::string> mFeeScheduleKey;

  /**
   * The desired new pause key of the token.
   */
  std::optional<std::string> mPauseKey;

  /**
   * The desired new metadata of the token.
   */
  std::optional<std::string> mMetadata;

  /**
   * The desired new metadata key of the token.
   */
  std::optional<std::string> mMetadataKey;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UpdateTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::UpdateTokenParams>
{
  /**
   * Convert a JSON object to a UpdateTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the UpdateTokenParams.
   * @param params   The UpdateTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::UpdateTokenParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mSymbol = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "symbol");
    params.mName = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "name");
    params.mTreasuryAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "treasuryAccountId");
    params.mAdminKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "adminKey");
    params.mKycKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "kycKey");
    params.mFreezeKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "freezeKey");
    params.mWipeKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "wipeKey");
    params.mSupplyKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "supplyKey");
    params.mAutoRenewAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewAccountId");
    params.mAutoRenewPeriod = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewPeriod");
    params.mExpirationTime = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "expirationTime");
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mFeeScheduleKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "feeScheduleKey");
    params.mPauseKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "pauseKey");
    params.mMetadata = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "metadata");
    params.mMetadataKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "metadataKey");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_UPDATE_TOKEN_PARAMS_H_
