// SPDX-License-Identifier: Apache-2.0
#include "FeeSchedules.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
FeeSchedules FeeSchedules::fromProtobuf(const proto::CurrentAndNextFeeSchedule& proto)
{
  FeeSchedules feeSchedules;

  if (proto.has_currentfeeschedule())
  {
    feeSchedules.mCurrentFeeSchedule = FeeSchedule::fromProtobuf(proto.currentfeeschedule());
  }

  if (proto.has_nextfeeschedule())
  {
    feeSchedules.mNextFeeSchedule = FeeSchedule::fromProtobuf(proto.nextfeeschedule());
  }

  return feeSchedules;
}

//-----
FeeSchedules FeeSchedules::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::CurrentAndNextFeeSchedule proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::CurrentAndNextFeeSchedule> FeeSchedules::toProtobuf() const
{
  auto proto = std::make_unique<proto::CurrentAndNextFeeSchedule>();
  proto->set_allocated_currentfeeschedule(mCurrentFeeSchedule.toProtobuf().release());
  proto->set_allocated_nextfeeschedule(mNextFeeSchedule.toProtobuf().release());
  return proto;
}

//-----
std::vector<std::byte> FeeSchedules::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string FeeSchedules::toString() const
{
  nlohmann::json json;
  json["mCurrentFeeSchedule"] = mCurrentFeeSchedule.toString();
  json["mNextFeeSchedule"] = mNextFeeSchedule.toString();
  return json.dump();
}

} // namespace Hiero
