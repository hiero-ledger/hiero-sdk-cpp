// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SCHEDULE_INFO_H_
#define HIERO_SDK_CPP_SCHEDULE_INFO_H_

#include "AccountId.h"
#include "Key.h"
#include "KeyList.h"
#include "LedgerId.h"
#include "ScheduleId.h"
#include "TransactionId.h"
#include "WrappedTransaction.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class ScheduleInfo;
}

namespace Hiero
{
/**
 * Response from a Hiero network when the client sends a ScheduleInfoQuery.
 */
class ScheduleInfo
{
public:
  /**
   * Construct a ScheduleInfo object from a ScheduleInfo protobuf object.
   *
   * @param proto The ScheduleInfo protobuf object from which to construct a ScheduleInfo object.
   * @return The constructed ScheduleInfo object.
   */
  [[nodiscard]] static ScheduleInfo fromProtobuf(const proto::ScheduleInfo& proto);

  /**
   * Construct a ScheduleInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct a ScheduleInfo object.
   * @return The constructed ScheduleInfo object.
   */
  [[nodiscard]] static ScheduleInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a ScheduleInfo protobuf object from this ScheduleInfo object.
   *
   * @return A pointer to the created ScheduleInfo protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::ScheduleInfo> toProtobuf() const;

  /**
   * Construct a representative byte array from this ScheduleInfo object.
   *
   * @return The byte array representing this ScheduleInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this ScheduleInfo object.
   *
   * @return The string representation of this ScheduleInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this ScheduleInfo to an output stream.
   *
   * @param os   The output stream.
   * @param info The ScheduleInfo to print.
   * @return The output stream with this ScheduleInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const ScheduleInfo& info);

  /**
   * The ID of the schedule.
   */
  ScheduleId mScheduleId;

  /**
   * The consensus timestamp the scheduled transaction was executed. If the scheduled transaction has not yet executed,
   * this will be uninitialized.
   */
  std::optional<std::chrono::system_clock::time_point> mExecutionTime;

  /**
   * The consensus timestamp that the scheduled transaction was deleted. If the scheduled transaction has not been
   * deleted, this will be uninitialized.
   */
  std::optional<std::chrono::system_clock::time_point> mDeletionTime;

  /**
   * The consensus timestamp at which the scheduled transaction will expire.
   */
  std::chrono::system_clock::time_point mExpirationTime;

  /**
   * The scheduled transaction.
   */
  WrappedTransaction mScheduledTransaction;

  /**
   * The publicly visible memo of the schedule entity.
   */
  std::string mMemo;

  /**
   * The admin key of the schedule entity.
   */
  std::shared_ptr<Key> mAdminKey = nullptr;

  /**
   * The signatories who have provided their signatures thus far for the scheduled transaction.
   */
  KeyList mSignatories;

  /**
   * The ID of the account that created the scheduled transaction.
   */
  AccountId mCreatorAccountId;

  /**
   * The ID of the account paying for the execution of the scheduled transaction.
   */
  AccountId mPayerAccountId;

  /**
   * The ID of the scheduled transaction, if it executes.
   */
  TransactionId mScheduledTransactionId;

  /**
   * The ID of the ledger from which this response was returned.
   */
  LedgerId mLedgerId;

  /**
   * If \c TRUE, the scheduled transaction will wait to execute until its expiration time. If \c FALSE, the scheduled
   * transaction will execute when it receives a sufficient signature amount.
   */
  bool mWaitForExpiry = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SCHEDULE_INFO_H_
