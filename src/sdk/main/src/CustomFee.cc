// SPDX-License-Identifier: Apache-2.0
#include "CustomFee.h"
#include "Client.h"
#include "CustomFixedFee.h"
#include "CustomFractionalFee.h"
#include "CustomRoyaltyFee.h"
#include "impl/Utilities.h"

#include <custom_fees.pb.h>

namespace Hiero
{
//-----
std::unique_ptr<CustomFee> CustomFee::fromProtobuf(const proto::CustomFee& proto)
{
  std::unique_ptr<CustomFee> fee;
  switch (proto.fee_case())
  {
    case proto::CustomFee::FeeCase::kFixedFee:
    {
      fee = std::make_unique<CustomFixedFee>(CustomFixedFee::fromProtobuf(proto.fixed_fee()));
      break;
    }
    case proto::CustomFee::FeeCase::kFractionalFee:
    {
      fee = std::make_unique<CustomFractionalFee>(CustomFractionalFee::fromProtobuf(proto.fractional_fee()));
      break;
    }
    case proto::CustomFee::FeeCase::kRoyaltyFee:
    {
      fee = std::make_unique<CustomRoyaltyFee>(CustomRoyaltyFee::fromProtobuf(proto.royalty_fee()));
      break;
    }
    default:
      throw std::invalid_argument("Fee protobuf case not recognized");
  }

  fee->mFeeCollectorAccountId = AccountId::fromProtobuf(proto.fee_collector_account_id());
  fee->mAllCollectorsAreExempt = proto.all_collectors_are_exempt();
  return fee;
}

//-----
std::unique_ptr<CustomFee> CustomFee::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::CustomFee fee;
  fee.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(fee);
}

//-----
std::ostream& operator<<(std::ostream& os, const CustomFee& fee)
{
  os << fee.toString();
  return os;
}

//-----
void CustomFee::validateChecksums(const Client& client) const
{
  mFeeCollectorAccountId.validateChecksum(client);
}

//-----
std::vector<std::byte> CustomFee::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::unique_ptr<proto::CustomFee> CustomFee::initProtobuf() const
{
  auto fee = std::make_unique<proto::CustomFee>();
  fee->set_allocated_fee_collector_account_id(mFeeCollectorAccountId.toProtobuf().release());
  fee->set_all_collectors_are_exempt(mAllCollectorsAreExempt);
  return fee;
}

} // namespace Hiero
