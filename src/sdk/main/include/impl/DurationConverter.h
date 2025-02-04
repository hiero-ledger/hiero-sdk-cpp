// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_DURATION_CONVERTER_H_
#define HIERO_SDK_CPP_IMPL_DURATION_CONVERTER_H_

#include <chrono>
#include <memory>

namespace proto
{
class Duration;
}

namespace Hiero::internal::DurationConverter
{
/**
 * Get the duration in seconds represented by a Duration protobuf object.
 *
 * @param duration The Duration protobuf object from which to get the duration.
 * @return The duration in seconds of the input Duration protobuf object.
 */
std::chrono::seconds fromProtobuf(const proto::Duration& duration);

/**
 * Create a Duration protobuf object from a generic duration.
 *
 * @param duration The duration object from which to create a Duration protobuf object.
 * @return A pointer to the created Duration protobuf object.
 */
proto::Duration* toProtobuf(const std::chrono::system_clock::duration& duration);

} // namespace Hiero::internal::DurationConverter

#endif // HIERO_SDK_CPP_IMPL_DURATION_CONVERTER_H_
