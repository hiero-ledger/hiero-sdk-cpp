// SPDX-License-Identifier: Apache-2.0
#include "ProxyStaker.h"
#include "impl/Utilities.h"

#include <crypto_get_stakers.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
ProxyStaker::ProxyStaker(AccountId accountId, int64_t amount)
  : mAccountId(std::move(accountId))
  , mAmount(Hbar(amount, HbarUnit::TINYBAR()))
{
}

//-----
ProxyStaker ProxyStaker::fromProtobuf(const proto::ProxyStaker& proto)
{
  return { AccountId::fromProtobuf(proto.accountid()), proto.amount() };
}

//-----
ProxyStaker ProxyStaker::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ProxyStaker proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ProxyStaker> ProxyStaker::toProtobuf() const
{
  auto proto = std::make_unique<proto::ProxyStaker>();
  proto->set_allocated_accountid(mAccountId.toProtobuf().release());
  proto->set_amount(mAmount.toTinybars());
  return proto;
}

//-----
std::vector<std::byte> ProxyStaker::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ProxyStaker::toString() const
{
  nlohmann::json json;
  json["mAccountId"] = mAccountId.toString();
  json["mAmount"] = mAmount.toString();
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const ProxyStaker& staker)
{
  os << staker.toString();
  return os;
}

} // namespace Hiero
