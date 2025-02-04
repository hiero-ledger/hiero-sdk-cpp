// SPDX-License-Identifier: Apache-2.0
#include "HbarTransfer.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
HbarTransfer::HbarTransfer(AccountId accountId, const Hbar& amount, bool approval)
  : mAccountId(std::move(accountId))
  , mAmount(amount)
  , mIsApproved(approval)
{
}

//-----
HbarTransfer HbarTransfer::fromProtobuf(const proto::AccountAmount& proto)
{
  HbarTransfer transfer;

  if (proto.has_accountid())
  {
    transfer.mAccountId = AccountId::fromProtobuf(proto.accountid());
  }

  transfer.mAmount = Hbar(proto.amount(), HbarUnit::TINYBAR());
  transfer.mIsApproved = proto.is_approval();

  return transfer;
}

//-----
HbarTransfer HbarTransfer::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::AccountAmount proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::AccountAmount> HbarTransfer::toProtobuf() const
{
  auto proto = std::make_unique<proto::AccountAmount>();
  proto->set_allocated_accountid(mAccountId.toProtobuf().release());
  proto->set_amount(mAmount.toTinybars());
  proto->set_is_approval(mIsApproved);
  return proto;
}

//-----
std::vector<std::byte> HbarTransfer::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string HbarTransfer::toString() const
{
  nlohmann::json json;
  json["mAccountId"] = mAccountId.toString();
  json["mAmount"] = mAmount.toString();
  json["mIsApproved"] = mIsApproved;
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const HbarTransfer& transfer)
{
  os << transfer.toString();
  return os;
}

} // namespace Hiero
