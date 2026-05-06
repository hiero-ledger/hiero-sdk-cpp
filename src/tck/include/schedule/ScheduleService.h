// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_SCHEDULE_SERVICE_H_
#define HIERO_TCK_CPP_SCHEDULE_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::ScheduleService
{
/**
 * Forward declarations.
 */
struct CreateScheduleParams;
struct DeleteScheduleParams;

/**
 * Create a schedule.
 *
 * @param params The parameters to use to create a schedule.
 * @return A JSON response containing the status of the schedule creation and the new schedule's ID.
 */
nlohmann::json createSchedule(const CreateScheduleParams& params);

/**
 * Delete a schedule.
 *
 * @param params The parameters to use to delete a schedule.
 * @return A JSON response containing the status of the deletion of the schedule.
 */
nlohmann::json deleteSchedule(const DeleteScheduleParams& params);

} // namespace Hiero::TCK::ScheduleService

#endif // HIERO_TCK_CPP_SCHEDULE_SERVICE_H_
