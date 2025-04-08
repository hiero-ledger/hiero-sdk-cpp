// SPDX-License-Identifier: Apache-2.0
#include "CustomFeeLimit.h"

#include "services/custom_fees.pb.h"
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
CustomFeeLimit CustomFeeLimit::fromProtobuf(const proto::CustomFeeLimit& protoFeeLimit)
{
  CustomFeeLimit customFeeLimit;

  if (protoFeeLimit.has_account_id())
  {
    customFeeLimit.setPayerId(AccountId::fromProtobuf(protoFeeLimit.account_id()));
  }

  for (const auto& protoFee : protoFeeLimit.fees())
  {
    customFeeLimit.addCustomFee(CustomFixedFee::fromProtobuf(protoFee));
  }

  return customFeeLimit;
}

//-----
std::unique_ptr<proto::CustomFeeLimit> CustomFeeLimit::toProtobuf() const
{
  auto protoFeeLimit = std::make_unique<proto::CustomFeeLimit>();

  if (mPayerId)
  {
    protoFeeLimit->set_allocated_account_id(mPayerId->toProtobuf().release());
  }

  for (const auto& fee : mCustomFees)
  {
    protoFeeLimit->mutable_fees()->AddAllocated(fee.toFixedFeeProtobuf().release());
  }

  return protoFeeLimit;
}

//-----
CustomFeeLimit& CustomFeeLimit::setPayerId(const AccountId& payerId)
{
  mPayerId = payerId;
  return *this;
}

//-----
std::optional<AccountId> CustomFeeLimit::getPayerId() const
{
  return mPayerId;
}

//-----
CustomFeeLimit& CustomFeeLimit::setCustomFees(const std::vector<CustomFixedFee>& customFees)
{
  mCustomFees = customFees;
  return *this;
}

//-----
CustomFeeLimit& CustomFeeLimit::addCustomFee(const CustomFixedFee& customFee)
{
  mCustomFees.push_back(customFee);
  return *this;
}

//-----
std::vector<CustomFixedFee> CustomFeeLimit::getCustomFees() const
{
  return mCustomFees;
}

//-----
std::string CustomFeeLimit::toString() const
{
  nlohmann::json json;

  json["mPayerId"] = mPayerId ? mPayerId->toString() : "None";

  json["mCustomFees"] = nlohmann::json::array();
  for (const auto& fee : mCustomFees)
  {
    json["mCustomFees"].push_back(fee.toString());
  }

  return json.dump();
}
}