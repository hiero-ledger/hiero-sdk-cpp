// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_DATA_TYPE_H_
#define HIERO_SDK_CPP_FEE_DATA_TYPE_H_

#include <string>
#include <unordered_map>

namespace proto
{
enum SubType : int;
}

namespace Hiero
{
/**
 * Enumeration of fee data types.
 */
enum class FeeDataType
{
  /**
   * The resource prices have no special scope.
   */
  DEFAULT,
  /**
   * The resource prices are scoped to an operation on a fungible common token.
   */
  TOKEN_FUNGIBLE_COMMON,
  /**
   * The resource prices are scoped to an operation on a non-fungible unique token.
   */
  TOKEN_NON_FUNGIBLE_UNIQUE,
  /**
   * The resource prices are scoped to an operation on a fungible common token with a custom fee schedule.
   */
  TOKEN_FUNGIBLE_COMMON_WITH_CUSTOM_FEES,
  /**
   * The resource prices are scoped to an operation on a non-fungible unique token with a custom fee schedule.
   */
  TOKEN_NON_FUNGIBLE_UNIQUE_WITH_CUSTOM_FEES,
  /**
   * The resource prices are scoped to a ScheduleCreate containing a ContractCall.
   */
  SCHEDULE_CREATE_CONTRACT_CALL
};

/**
 * Map of protobuf SubTypes to their corresponding FeeDataTypes.
 */
extern const std::unordered_map<proto::SubType, FeeDataType> gProtobufSubTypeToFeeDataType;

/**
 * Map of FeeDataTypes to their corresponding protobuf SubTypes.
 */
extern const std::unordered_map<FeeDataType, proto::SubType> gFeeDataTypeToProtobufSubType;

/**
 * Map of FeeDataType to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<FeeDataType, std::string> gFeeDataTypeToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_DATA_TYPE_H_
