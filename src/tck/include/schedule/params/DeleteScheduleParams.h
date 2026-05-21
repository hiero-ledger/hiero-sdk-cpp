// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_DELETE_SCHEDULE_PARAMS_H_
#define HIERO_TCK_CPP_DELETE_SCHEDULE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "nlohmann/json.hpp"

#include "json/JsonUtils.h"
#include <optional>
#include <string>

namespace Hiero::TCK::ScheduleService
{
/**
 * Struct to hold the arguments for a `deleteSchedule` Json-RPC method call.
 */
struct DeleteScheduleParams
{
  /**
   * The ID of the schedule to delete.
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
 * JSON serializer template specialisation required to convert DeleteScheduleParams argument properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ScheduleService::DeleteScheduleParams>
{
  /**
   * Convert a JSON object to a DeleteScheduleParams.
   *
   * @param jsonFrom The JSON object with which to fill the DeleteScheduleParams.
   * @param params   The DeleteScheduleParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ScheduleService::DeleteScheduleParams& params)
  {
    params.mScheduleId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "scheduleId");

    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_DELETE_SCHEDULE_PARAMS_H_
