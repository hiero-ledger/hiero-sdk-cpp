// SPDX-License-Identifier: Apache-2.0
#include "FeeSchedule.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
FeeSchedule FeeSchedule::fromProtobuf(const proto::FeeSchedule& proto)
{
  FeeSchedule feeSchedule;

  for (int i = 0; i < proto.transactionfeeschedule_size(); ++i)
  {
    feeSchedule.mTransactionFeeSchedules.push_back(
      TransactionFeeSchedule::fromProtobuf(proto.transactionfeeschedule(i)));
  }

  if (proto.has_expirytime())
  {
    feeSchedule.mExpirationTime = internal::TimestampConverter::fromProtobuf(proto.expirytime());
  }

  return feeSchedule;
}

//-----
FeeSchedule FeeSchedule::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::FeeSchedule feeSchedule;
  feeSchedule.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(feeSchedule);
}

//-----
std::unique_ptr<proto::FeeSchedule> FeeSchedule::toProtobuf() const
{
  auto proto = std::make_unique<proto::FeeSchedule>();
  std::for_each(mTransactionFeeSchedules.cbegin(),
                mTransactionFeeSchedules.cend(),
                [&proto](const TransactionFeeSchedule& schedule)
                { *proto->add_transactionfeeschedule() = *schedule.toProtobuf(); });
  proto->set_allocated_expirytime(internal::TimestampConverter::toSecondsProtobuf(mExpirationTime));
  return proto;
}

//-----
std::vector<std::byte> FeeSchedule::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string FeeSchedule::toString() const
{
  nlohmann::json json;
  std::for_each(mTransactionFeeSchedules.cbegin(),
                mTransactionFeeSchedules.cend(),
                [&json](const TransactionFeeSchedule& schedule)
                { json["mTransactionFeeSchedules"].push_back(schedule.toString()); });
  json["mExpirationTime"] = internal::TimestampConverter::toString(mExpirationTime);
  return json.dump();
}

} // namespace Hiero
