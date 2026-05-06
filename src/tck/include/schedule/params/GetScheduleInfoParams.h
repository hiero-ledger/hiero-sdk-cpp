// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_
#define HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_

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
};

} // namespace Hiero::TCK::ScheduleService

namespace nlohmann
{
template<>
struct adl_serializer<Hiero::TCK::ScheduleService::GetScheduleInfoParams>
{
  /**
   * From JSON.
   */
  static Hiero::TCK::ScheduleService::GetScheduleInfoParams from_json(const json& j)
  {
    Hiero::TCK::ScheduleService::GetScheduleInfoParams params;
    if (j.contains("scheduleId") && !j["scheduleId"].is_null())
    {
      params.mScheduleId = j["scheduleId"].get<std::string>();
    }

    return params;
  }
};
} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_SCHEDULE_INFO_PARAMS_H_