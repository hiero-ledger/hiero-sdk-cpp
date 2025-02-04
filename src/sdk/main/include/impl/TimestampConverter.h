// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_TIMESTAMP_CONVERTER_H_
#define HIERO_SDK_CPP_IMPL_TIMESTAMP_CONVERTER_H_

#include <chrono>
#include <memory>
#include <string>

namespace proto
{
class Timestamp;
class TimestampSeconds;
}

namespace Hiero::internal::TimestampConverter
{
/**
 * Get the time point represented by a Timestamp protobuf object.
 *
 * @param timestamp The Timestamp protobuf from which to get the time point.
 * @return The time point represented by the input Timestamp protobuf.
 */
std::chrono::system_clock::time_point fromProtobuf(const proto::Timestamp& timestamp);

/**
 * Get the time point represented by a TimestampSeconds protobuf object.
 *
 * @param timestamp The TimestampSeconds protobuf from which to get the time point.
 * @return The time point represented by the input TimestampSeconds protobuf.
 */
std::chrono::system_clock::time_point fromProtobuf(const proto::TimestampSeconds& timestamp);

/**
 * Create a Timestamp protobuf object from a time point.
 *
 * @param duration The duration object from which to create a Timestamp protobuf object.
 * @return A pointer to the created Timestamp protobuf object.
 */
proto::Timestamp* toProtobuf(const std::chrono::system_clock::time_point& time);

/**
 * Create a TimestampSeconds protobuf object from a time point.
 *
 * @param duration The duration object from which to create a TimestampSeconds protobuf object.
 * @return A pointer to the created TimestampSeconds protobuf object.
 */
proto::TimestampSeconds* toSecondsProtobuf(const std::chrono::system_clock::time_point& time);

/**
 * Convert an epoch timestamp to a string representation of the form "<seconds>.<nanoseconds>".
 *
 * @param timestamp The timestamp of which to get the the string representation.
 * @return The string representation of the input timestamp.
 */
[[nodiscard]] std::string toString(const std::chrono::system_clock::time_point& timestamp);

} // namespace Hiero::internal::TimestampConverter

#endif // HIERO_SDK_CPP_IMPL_TIMESTAMP_CONVERTER_H_
