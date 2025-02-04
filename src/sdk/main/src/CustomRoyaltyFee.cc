// SPDX-License-Identifier: Apache-2.0
#include "CustomRoyaltyFee.h"

#include <custom_fees.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
CustomRoyaltyFee CustomRoyaltyFee::fromProtobuf(const proto::RoyaltyFee& proto)
{
  CustomRoyaltyFee customRoyaltyFee;

  if (proto.has_exchange_value_fraction())
  {
    customRoyaltyFee.mNumerator = proto.exchange_value_fraction().numerator();
    customRoyaltyFee.mDenominator = proto.exchange_value_fraction().denominator();
  }

  if (proto.has_fallback_fee())
  {
    customRoyaltyFee.mFallbackFee = CustomFixedFee::fromProtobuf(proto.fallback_fee());
  }

  return customRoyaltyFee;
}

//-----
std::unique_ptr<CustomFee> CustomRoyaltyFee::clone() const
{
  return std::make_unique<CustomRoyaltyFee>(*this);
}

//-----
std::unique_ptr<proto::CustomFee> CustomRoyaltyFee::toProtobuf() const
{
  std::unique_ptr<proto::CustomFee> fee = initProtobuf();
  fee->mutable_royalty_fee()->mutable_exchange_value_fraction()->set_numerator(mNumerator);
  fee->mutable_royalty_fee()->mutable_exchange_value_fraction()->set_denominator(mDenominator);

  if (mFallbackFee.has_value())
  {
    fee->mutable_royalty_fee()->set_allocated_fallback_fee(mFallbackFee->toFixedFeeProtobuf().release());
  }

  return fee;
}

//-----
std::string CustomRoyaltyFee::toString() const
{
  nlohmann::json json;
  json["mFeeCollectorAccountId"] = mFeeCollectorAccountId.toString();
  json["mAllCollectorsAreExempt"] = mAllCollectorsAreExempt;
  json["mNumerator"] = mNumerator;
  json["mDenominator"] = mDenominator;

  if (mFallbackFee.has_value())
  {
    json["mFallbackFee"] = mFallbackFee->toString();
  }

  return json.dump();
}

//-----
CustomRoyaltyFee& CustomRoyaltyFee::setNumerator(const int64_t& numerator)
{
  mNumerator = numerator;
  return *this;
}

//-----
CustomRoyaltyFee& CustomRoyaltyFee::setDenominator(const int64_t& denominator)
{
  mDenominator = denominator;
  return *this;
}

//-----
CustomRoyaltyFee& CustomRoyaltyFee::setFallbackFee(const CustomFixedFee& fee)
{
  mFallbackFee = fee;
  return *this;
}

} // namespace Hiero
