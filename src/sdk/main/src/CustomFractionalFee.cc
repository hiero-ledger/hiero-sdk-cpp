// SPDX-License-Identifier: Apache-2.0
#include "CustomFractionalFee.h"

#include <custom_fees.pb.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Hiero
{
//-----
CustomFractionalFee CustomFractionalFee::fromProtobuf(const proto::FractionalFee& proto)
{
  CustomFractionalFee customFractionalFee;

  if (proto.has_fractional_amount())
  {
    customFractionalFee.mNumerator = proto.fractional_amount().numerator();
    customFractionalFee.mDenominator = proto.fractional_amount().denominator();
  }

  customFractionalFee.mMinAmount = static_cast<uint64_t>(proto.minimum_amount());
  customFractionalFee.mMaxAmount = static_cast<uint64_t>(proto.maximum_amount());
  customFractionalFee.mAssessmentMethod =
    proto.net_of_transfers() ? FeeAssessmentMethod::EXCLUSIVE : FeeAssessmentMethod::INCLUSIVE;

  return customFractionalFee;
}

//-----
std::unique_ptr<CustomFee> CustomFractionalFee::clone() const
{
  return std::make_unique<CustomFractionalFee>(*this);
}

//-----
std::unique_ptr<proto::CustomFee> CustomFractionalFee::toProtobuf() const
{
  std::unique_ptr<proto::CustomFee> fee = initProtobuf();
  fee->mutable_fractional_fee()->mutable_fractional_amount()->set_numerator(mNumerator);
  fee->mutable_fractional_fee()->mutable_fractional_amount()->set_denominator(mDenominator);
  fee->mutable_fractional_fee()->set_minimum_amount(static_cast<int64_t>(mMinAmount));
  fee->mutable_fractional_fee()->set_maximum_amount(static_cast<int64_t>(mMaxAmount));
  fee->mutable_fractional_fee()->set_net_of_transfers(mAssessmentMethod == FeeAssessmentMethod::EXCLUSIVE);
  return fee;
}

//-----
std::string CustomFractionalFee::toString() const
{
  nlohmann::json json;
  json["mFeeCollectorAccountId"] = mFeeCollectorAccountId.toString();
  json["mAllCollectorsAreExempt"] = mAllCollectorsAreExempt;
  json["mNumerator"] = mNumerator;
  json["mDenominator"] = mDenominator;
  json["mMinAmount"] = mMinAmount;
  json["mMaxAmount"] = mMaxAmount;
  json["mMaxAmount"] = mMaxAmount;
  json["mAssessmentMethod"] = gFeeAssessmentMethodToString.at(mAssessmentMethod);
  return json.dump();
}

//-----
CustomFractionalFee& CustomFractionalFee::setNumerator(const int64_t& numerator)
{
  mNumerator = numerator;
  return *this;
}

//-----
CustomFractionalFee& CustomFractionalFee::setDenominator(const int64_t& denominator)
{
  mDenominator = denominator;
  return *this;
}

//-----
CustomFractionalFee& CustomFractionalFee::setMinimumAmount(const uint64_t& amount)
{
  mMinAmount = amount;
  return *this;
}

//-----
CustomFractionalFee& CustomFractionalFee::setMaximumAmount(const uint64_t& amount)
{
  mMaxAmount = amount;
  return *this;
}

//-----
CustomFractionalFee& CustomFractionalFee::setAssessmentMethod(Hiero::FeeAssessmentMethod method)
{
  mAssessmentMethod = method;
  return *this;
}

} // namespace Hiero
