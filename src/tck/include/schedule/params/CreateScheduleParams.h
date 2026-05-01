// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CREATE_SCHEDULE_PARAMS_H_
#define HIERO_TCK_CPP_CREATE_SCHEDULE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "nlohmann/json.hpp"
#include "json/JsonUtils.h"

#include <optional>
#include <string>

namespace Hiero::TCK::ScheduleService
{
/**
 * Struct to hold the arguments for a `createSchedule` JSON-RPC method call.
 */
struct CreateScheduleParams
{
  /**
   * Nested JSON object describing the inner transaction to schedule.
   */
  nlohmann::json mScheduledTransaction;

  /**
   * Optional memo for the schedule.
   */
  std::optional<std::string> mMemo;

  /**
   * Optional admin key for the schedule.
   */
  std::optional<std::string> mAdminKey;

  /**
   * Optional account ID that will pay for the scheduled transaction.
   */
  std::optional<std::string> mPayerAccountId;

  /**
   * Optional expiration time for the schedule.
   */
  std::optional<std::string> mExpirationTime;

  /**
   * Optional flag to wait for expiry before executing the scheduled transaction.
   */
  std::optional<bool> mWaitForExpiry;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::ScheduleService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CreateScheduleParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::ScheduleService::CreateScheduleParams>
{
  /**
   * Convert a Json Object to a CreateScheduleParams
   *
   * @param jsonFrom The json object which will fill the CreateScheduleParams.
   * @param params The CreateScheduleParam to fill the json object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::ScheduleService::CreateScheduleParams& params)
  {
    params.mScheduledTransaction = jsonFrom.at("scheduledTransaction");
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mAdminKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "adminKey");
    params.mPayerAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "payerAccountId");
    params.mExpirationTime = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "expirationTime");
    params.mWaitForExpiry = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "waitForExpiry");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CREATE_SCHEDULE_PARAMS_H_
