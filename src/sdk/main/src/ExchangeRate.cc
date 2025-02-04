// SPDX-License-Identifier: Apache-2.0
#include "ExchangeRate.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <exchange_rate.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
ExchangeRate::ExchangeRate(int hbar, int cents, const std::chrono::system_clock::time_point& expirationTime)
  : mHbars(hbar)
  , mCents(cents)
  , mExpirationTime(expirationTime)
  , mExchangeRateInCents(static_cast<double>(cents) / static_cast<double>(hbar))
{
}

//-----
ExchangeRate ExchangeRate::fromProtobuf(const proto::ExchangeRate& proto)
{
  return { proto.hbarequiv(), proto.centequiv(), internal::TimestampConverter::fromProtobuf(proto.expirationtime()) };
}

//-----
ExchangeRate ExchangeRate::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ExchangeRate proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ExchangeRate> ExchangeRate::toProtobuf() const
{
  auto proto = std::make_unique<proto::ExchangeRate>();
  proto->set_hbarequiv(mHbars);
  proto->set_centequiv(mCents);
  proto->set_allocated_expirationtime(internal::TimestampConverter::toSecondsProtobuf(mExpirationTime));
  return proto;
}

//-----
std::vector<std::byte> ExchangeRate::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ExchangeRate::toString() const
{
  nlohmann::json json;
  json["mHbars"] = mHbars;
  json["mCents"] = mCents;
  json["mExpirationTime"] = internal::TimestampConverter::toString(mExpirationTime);
  json["mExchangeRateInCents"] = mExchangeRateInCents;
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const ExchangeRate& rate)
{
  os << rate.toString();
  return os;
}

} // namespace Hiero
