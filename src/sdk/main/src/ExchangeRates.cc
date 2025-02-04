// SPDX-License-Identifier: Apache-2.0
#include "ExchangeRates.h"
#include "impl/Utilities.h"

#include <exchange_rate.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
ExchangeRates::ExchangeRates(const ExchangeRate& current, const ExchangeRate& next)
  : mCurrentRate(current)
  , mNextRate(next)
{
}

//-----
ExchangeRates ExchangeRates::fromProtobuf(const proto::ExchangeRateSet& proto)
{
  return { ExchangeRate::fromProtobuf(proto.currentrate()), ExchangeRate::fromProtobuf(proto.nextrate()) };
}

//-----
ExchangeRates ExchangeRates::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ExchangeRateSet proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ExchangeRateSet> ExchangeRates::toProtobuf() const
{
  auto proto = std::make_unique<proto::ExchangeRateSet>();
  proto->set_allocated_currentrate(mCurrentRate.toProtobuf().release());
  proto->set_allocated_nextrate(mNextRate.toProtobuf().release());
  return proto;
}

//-----
std::vector<std::byte> ExchangeRates::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ExchangeRates::toString() const
{
  nlohmann::json json;
  json["mCurrentRate"] = mCurrentRate.toString();
  json["mNextRate"] = mNextRate.toString();
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const ExchangeRates& rates)
{
  os << rates.toString();
  return os;
}

} // namespace Hiero
