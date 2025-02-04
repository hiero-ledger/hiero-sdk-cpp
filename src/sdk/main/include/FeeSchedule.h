// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_SCHEDULE_H_
#define HIERO_SDK_CPP_FEE_SCHEDULE_H_

#include "TransactionFeeSchedule.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace proto
{
class FeeSchedule;
}

namespace Hiero
{
/**
 * A list of resource prices fee for different transactions and queries and the time period at which this fee schedule
 * will expire. Nodes use the prices to determine the fees for all transactions based on how much of those resources
 * each transaction uses.
 */
class FeeSchedule
{
public:
  /**
   * Construct a FeeSchedule object from a FeeSchedule protobuf object.
   *
   * @param proto The FeeSchedule protobuf object from which to construct an FeeSchedule object.
   * @return The constructed FeeSchedule object.
   */
  [[nodiscard]] static FeeSchedule fromProtobuf(const proto::FeeSchedule& proto);

  /**
   * Construct a FeeSchedule object from a byte array.
   *
   * @param bytes The byte array from which to construct an FeeSchedule object.
   * @return The constructed FeeSchedule object.
   */
  [[nodiscard]] static FeeSchedule fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a FeeSchedule protobuf object from this FeeSchedule object.
   *
   * @return A pointer to the created FeeSchedule protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::FeeSchedule> toProtobuf() const;

  /**
   * Construct a representative byte array from this FeeSchedule object.
   *
   * @return The byte array representing this FeeSchedule object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this FeeSchedule object.
   *
   * @return The string representation of this FeeSchedule object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Add a TransactionFeeSchedule.
   *
   * @param schedule The schedule to add.
   * @return A reference to this FeeSchedule with the newly-added schedule.
   */
  inline FeeSchedule& addTransactionFeeSchedule(const TransactionFeeSchedule& schedule)
  {
    mTransactionFeeSchedules.push_back(schedule);
    return *this;
  }

  /**
   * Set the expiration time.
   *
   * @param time The expiration time to set.
   * @return A reference to this FeeSchedule with the newly-set expiration time.
   */
  inline FeeSchedule& setExpirationTime(const std::chrono::system_clock::time_point& time)
  {
    mExpirationTime = time;
    return *this;
  }

  /**
   * Get the list of TransactionFeeSchedules.
   *
   * @return The list of TransactionFeeSchedules.
   */
  [[nodiscard]] inline std::vector<TransactionFeeSchedule> getTransactionFeeSchedules() const
  {
    return mTransactionFeeSchedules;
  }

  /**
   * Get the expiration time.
   *
   * @return The expiration time.
   */
  [[nodiscard]] inline std::chrono::system_clock::time_point getExpirationTime() const { return mExpirationTime; }

private:
  /**
   * The list of price coefficients for network resources.
   */
  std::vector<TransactionFeeSchedule> mTransactionFeeSchedules;

  /**
   * The expiration time of this FeeSchedule.
   */
  std::chrono::system_clock::time_point mExpirationTime = std::chrono::system_clock::now();
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_SCHEDULE_H_
