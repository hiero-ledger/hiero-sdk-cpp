// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FREEZE_TYPE_H_
#define HIERO_SDK_CPP_FREEZE_TYPE_H_

#include <string_view>
#include <unordered_map>

namespace proto
{
enum FreezeType : int;
}

namespace Hiero
{
/**
 * The type of network freeze or upgrade operation to be performed. This type dictates which fields are required.
 */
enum class FreezeType
{
  /**
   * An (invalid) default value for this enum, to ensure the client explicitly sets the intended type of freeze
   * transaction.
   */
  UNKNOWN_FREEZE_TYPE,
  /**
   * Freezes the network at the specified time. The start time field must be provided and must reference a future time.
   * Any values specified for the update file and file hash fields will be ignored. This transaction does not perform
   * any network changes or upgrades and requires manual intervention to restart the network.
   */
  FREEZE_ONLY,
  /**
   * A non-freezing operation that initiates network wide preparation in advance of a scheduled freeze upgrade. The
   * update_file and file_hash fields must be provided and valid. The start_time field may be omitted and any value
   * present will be ignored.
   */
  PREPARE_UPGRADE,
  /**
   * Freezes the network at the specified time and performs the previously prepared automatic upgrade across the entire
   * network.
   */
  FREEZE_UPGRADE,
  /**
   * Aborts a pending network freeze operation.
   */
  FREEZE_ABORT,
  /**
   * Performs an immediate upgrade on auxiliary services and containers providing telemetry/metrics. Does not impact
   * network operations.
   */
  TELEMETRY_UPGRADE
};

/**
 * Map of protobuf FreezeTypes to the corresponding FreezeType.
 */
extern const std::unordered_map<proto::FreezeType, FreezeType> gProtobufFreezeTypeToFreezeType;

/**
 * Map of FreezeTypes to the corresponding protobuf FreezeType.
 */
extern const std::unordered_map<FreezeType, proto::FreezeType> gFreezeTypeToProtobufFreezeType;

/**
 * Map of FreezeType to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<FreezeType, const char*> gFreezeTypeToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_FREEZE_TYPE_H_
