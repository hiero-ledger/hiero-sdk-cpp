// SPDX-License-Identifier: Apache-2.0
#include "CustomFixedFee.h"

#include <custom_fees.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
CustomFixedFee CustomFixedFee::fromProtobuf(const proto::FixedFee& proto)
{
  CustomFixedFee customFixedFee;
  customFixedFee.mAmount = static_cast<uint64_t>(proto.amount());

  if (proto.has_denominating_token_id())
  {
    customFixedFee.mDenominatingTokenId = TokenId::fromProtobuf(proto.denominating_token_id());
  }

  return customFixedFee;
}

//-----
std::unique_ptr<CustomFee> CustomFixedFee::clone() const
{
  return std::make_unique<CustomFixedFee>(*this);
}

//-----
std::unique_ptr<proto::CustomFee> CustomFixedFee::toProtobuf() const
{
  std::unique_ptr<proto::CustomFee> fee = initProtobuf();
  fee->mutable_fixed_fee()->set_amount(static_cast<int64_t>(mAmount));

  if (mDenominatingTokenId.has_value())
  {
    fee->mutable_fixed_fee()->set_allocated_denominating_token_id(mDenominatingTokenId->toProtobuf().release());
  }

  return fee;
}

//-----
std::string CustomFixedFee::toString() const
{
  nlohmann::json json;
  json["mFeeCollectorAccountId"] = mFeeCollectorAccountId.toString();
  json["mAllCollectorsAreExempt"] = mAllCollectorsAreExempt;
  json["mAmount"] = mAmount;

  if (mDenominatingTokenId.has_value())
  {
    json["mDenominatingTokenId"] = mDenominatingTokenId->toString();
  }

  return json.dump();
}

//-----
void CustomFixedFee::validateChecksums(const Client& client) const
{
  CustomFee::validateChecksums(client);

  if (mDenominatingTokenId.has_value())
  {
    mDenominatingTokenId->validateChecksum(client);
  }
}

//-----
std::unique_ptr<proto::FixedFee> CustomFixedFee::toFixedFeeProtobuf() const
{
  auto fee = std::make_unique<proto::FixedFee>();
  fee->set_amount(static_cast<int64_t>(mAmount));

  if (mDenominatingTokenId.has_value())
  {
    fee->set_allocated_denominating_token_id(mDenominatingTokenId->toProtobuf().release());
  }

  return fee;
}

//-----
CustomFixedFee& CustomFixedFee::setAmount(const uint64_t& amount)
{
  mAmount = amount;
  return *this;
}

//-----
CustomFixedFee& CustomFixedFee::setHbarAmount(const Hbar& amount)
{
  mDenominatingTokenId.reset();
  mAmount = amount.toTinybars();
  return *this;
}

//-----
CustomFixedFee& CustomFixedFee::setDenominatingTokenId(const TokenId& tokenId)
{
  mDenominatingTokenId = tokenId;
  return *this;
}

//-----
CustomFixedFee& CustomFixedFee::setDenominatingTokenToSameToken()
{
  mDenominatingTokenId = TokenId(0ULL, 0ULL, 0ULL);
  return *this;
}

} // namespace Hiero
