// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_
#define HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::ScheduleService
{
/**
 * Struct to hold the parameters for getting schedule info
 */
struct GetScheduleInfoParams
{
  /**
   * The schedule id
   */
  std::optional<std::string> mScheduleId;

  /**
   * The query payment
   */
  std::optional<std::string> mQueryPayment;

  /**
   * The max query payment
   */
  std::optional<std::string> mMaxQueryPayment;
};

} // namespace Hiero::TCK::ScheduleService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert GetScheduleInfoParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ScheduleService::GetScheduleInfoParams>
{
  static void from_json(const json& jsonFrom, Hiero::TCK::ScheduleService::GetScheduleInfoParams& params)
  {
    params.mScheduleId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "scheduleId");
    params.mQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "queryPayment");
    params.mMaxQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "maxQueryPayment");
  }
};
} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_
