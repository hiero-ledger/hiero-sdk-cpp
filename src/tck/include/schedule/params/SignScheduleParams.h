// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_SIGN_SCHEDULE_PARAMS_H_
#define HIERO_TCK_CPP_SIGN_SCHEDULE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>

#include <optional>
#include <string>

namespace Hiero::TCK::ScheduleService
{
/**
 * Struct to hold the arguments for a `signSchedule` JSON-RPC method call.
 */
struct SignScheduleParams
{
  /**
   * The ID of the schedule to sign.
   */
  std::optional<std::string> mScheduleId;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::ScheduleService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert SignScheduleParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ScheduleService::SignScheduleParams>
{
  /**
   * Convert a JSON object to a SignScheduleParams.
   *
   * @param jsonFrom The JSON object with which to fill the SignScheduleParams.
   * @param params   The SignScheduleParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ScheduleService::SignScheduleParams& params)
  {
    params.mScheduleId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "scheduleId");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_SIGN_SCHEDULE_PARAMS_H_
