// SPDX-License-Identifier: Apache-2.0
#include "impl/DurationConverter.h"

#include <duration.pb.h>

namespace Hiero::internal::DurationConverter
{
//----
std::chrono::seconds fromProtobuf(const proto::Duration& duration)
{
  return std::chrono::seconds(duration.seconds());
}

//-----
proto::Duration* toProtobuf(const std::chrono::system_clock::duration& duration)
{
  auto proto = std::make_unique<proto::Duration>();
  proto->set_seconds(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
  return proto.release();
}

} // namespace Hiero::internal::DurationConverter
