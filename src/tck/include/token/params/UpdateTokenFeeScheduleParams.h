// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_UPDATE_TOKEN_FEE_SCHEDULE_PARAMS_H_
#define HIERO_TCK_CPP_UPDATE_TOKEN_FEE_SCHEDULE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "token/CustomFeeSerializer.h"
#include "json/JsonUtils.h"

#include <CustomFee.h>

#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `updateTokenFeeSchedule` JSON-RPC method call.
 */
struct UpdateTokenFeeScheduleParams
{
  /**
   * The ID of the token of which to update the fee schedule.
   */
  std::optional<std::string> mTokenId;

  /**
   * The desired new fees to assess for the token.
   */
  std::optional<std::vector<std::shared_ptr<CustomFee>>> mCustomFees;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UpdateTokenFeeScheduleParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::UpdateTokenFeeScheduleParams>
{
  /**
   * Convert a JSON object to a UpdateTokenFeeScheduleParams.
   *
   * @param jsonFrom The JSON object with which to fill the UpdateTokenFeeScheduleParams.
   * @param params   The UpdateTokenFeeScheduleParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::UpdateTokenFeeScheduleParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mCustomFees =
      Hiero::TCK::getOptionalJsonParameter<std::vector<std::shared_ptr<Hiero::CustomFee>>>(jsonFrom, "customFees");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_UPDATE_TOKEN_FEE_SCHEDULE_PARAMS_H_
