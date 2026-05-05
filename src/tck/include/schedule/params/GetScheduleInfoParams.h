// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_
#define HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_

#include "json/JsonUtils.h"
#include "nlohmann/json.hpp"

#include <optional>
#include <string>

namespace Hiero::TCK::ScheduleService
{
/**
 * Struct to hold the arguments for a `getScheduleInfo` JSON-RPC method call.
 */
struct GetScheduleInfoParams
{
  /**
   * The ID of the schedule to query.
   */
  std::optional<std::string> mScheduleId;

  /**
   * The query payment (in tinybars) to use for this query.
   */
  std::optional<std::string> mQueryPayment;

  /**
   * The maximum query payment (in tinybars) to use for this query.
   */
  std::optional<std::string> mMaxQueryPayment;
};

} // namespace Hiero::TCK::ScheduleService

namespace nlohmann
{
/**
 * JSON serializer template specialisation required to convert GetScheduleInfoParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ScheduleService::GetScheduleInfoParams>
{
  /**
   * Convert a JSON object to a GetScheduleInfoParams.
   *
   * @param jsonFrom The JSON object which will fill the GetScheduleInfoParams.
   * @param params   The GetScheduleInfoParams to fill from the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ScheduleService::GetScheduleInfoParams& params)
  {
    params.mScheduleId      = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "scheduleId");
    params.mQueryPayment    = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "queryPayment");
    params.mMaxQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "maxQueryPayment");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_
