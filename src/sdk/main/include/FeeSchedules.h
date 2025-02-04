// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_SCHEDULES_H_
#define HIERO_SDK_CPP_FEE_SCHEDULES_H_

#include "FeeSchedule.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace proto
{
class CurrentAndNextFeeSchedule;
}

namespace Hiero
{
/**
 * Two fee schedules, one being the current fee schedule and its expiration, and the other being the fee schedule to be
 * used after the current expires.
 */
class FeeSchedules
{
public:
  /**
   * Construct a FeeSchedules object from a CurrentAndNextFeeSchedule protobuf object.
   *
   * @param proto The CurrentAndNextFeeSchedule protobuf object from which to construct an FeeSchedules object.
   * @return The constructed FeeSchedules object.
   */
  [[nodiscard]] static FeeSchedules fromProtobuf(const proto::CurrentAndNextFeeSchedule& proto);

  /**
   * Construct a FeeSchedules object from a byte array.
   *
   * @param bytes The byte array from which to construct an FeeSchedules object.
   * @return The constructed FeeSchedules object.
   */
  [[nodiscard]] static FeeSchedules fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a CurrentAndNextFeeSchedule protobuf object from this FeeSchedules object.
   *
   * @return A pointer to the created CurrentAndNextFeeSchedule protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::CurrentAndNextFeeSchedule> toProtobuf() const;

  /**
   * Construct a representative byte array from this FeeSchedules object.
   *
   * @return The byte array representing this FeeSchedules object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this FeeSchedules object.
   *
   * @return The string representation of this FeeSchedules object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Set the current fee schedule.
   *
   * @param schedule The current fee schedule to set.
   * @return A reference to this FeeSchedules with the newly-added current fee schedule.
   */
  inline FeeSchedules& setCurrent(const FeeSchedule& schedule)
  {
    mCurrentFeeSchedule = schedule;
    return *this;
  }

  /**
   * Set the next fee schedule.
   *
   * @param schedule The next fee schedule to set.
   * @return A reference to this FeeSchedules with the newly-added next fee schedule.
   */
  inline FeeSchedules& setNext(const FeeSchedule& schedule)
  {
    mNextFeeSchedule = schedule;
    return *this;
  }

  /**
   * Get the current fee schedule.
   *
   * @return The current fee schedule.
   */
  [[nodiscard]] inline FeeSchedule getCurrent() const { return mCurrentFeeSchedule; }

  /**
   * Get the next fee schedule.
   *
   * @return The next fee schedule.
   */
  [[nodiscard]] inline FeeSchedule getNext() const { return mNextFeeSchedule; }

private:
  /**
   * The current FeeSchedule.
   */
  FeeSchedule mCurrentFeeSchedule;

  /**
   * The next FeeSchedule.
   */
  FeeSchedule mNextFeeSchedule;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_SCHEDULES_H_
