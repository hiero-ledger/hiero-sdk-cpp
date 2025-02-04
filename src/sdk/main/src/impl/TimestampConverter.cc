// SPDX-License-Identifier: Apache-2.0
#include "impl/TimestampConverter.h"

#include <timestamp.pb.h>

namespace Hiero::internal::TimestampConverter
{
//-----
std::chrono::system_clock::time_point fromProtobuf(const proto::Timestamp& timestamp)
{
  // Convert the nanoseconds to a point in time using the precision of the system's clock
  return std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(
    std::chrono::nanoseconds(timestamp.seconds() * 1000000000 + timestamp.nanos())));
}

//-----
std::chrono::system_clock::time_point fromProtobuf(const proto::TimestampSeconds& timestamp)
{
  return std::chrono::system_clock::time_point(std::chrono::seconds(timestamp.seconds()));
}

//-----
proto::Timestamp* toProtobuf(const std::chrono::system_clock::time_point& time)
{
  auto timestamp = std::make_unique<proto::Timestamp>();
  const std::chrono::nanoseconds nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(time.time_since_epoch());
  timestamp->set_seconds(nanos.count() / 1000000000);
  timestamp->set_nanos(nanos.count() % 1000000000);
  return timestamp.release();
}

//-----
proto::TimestampSeconds* toSecondsProtobuf(const std::chrono::system_clock::time_point& time)
{
  auto timestamp = std::make_unique<proto::TimestampSeconds>();
  timestamp->set_seconds(std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count());
  return timestamp.release();
}

//-----
std::string toString(const std::chrono::system_clock::time_point& timestamp)
{
  const std::chrono::system_clock::duration durationSinceEpoch = timestamp.time_since_epoch();
  const std::chrono::seconds secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);

  return std::to_string(secondsSinceEpoch.count()) + '.' +
         std::to_string(
           std::chrono::duration_cast<std::chrono::nanoseconds>(durationSinceEpoch - secondsSinceEpoch).count());
}

} // namespace Hiero::internal::TimestampConverter
