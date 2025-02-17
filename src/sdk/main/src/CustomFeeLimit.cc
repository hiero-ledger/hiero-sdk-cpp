// SPDX-License-Identifier: Apache-2.0
#include "CustomFeeLimit.h"

#include "custom_fees.pb.h"

namespace Hiero
{
//-----
CustomFeeLimit CustomFeeLimit::fromProtobuf(const proto::CustomFeeLimit& protoFeeLimit)
{
  std::optional<AccountId> payerId;
  if (protoFeeLimit.has_account_id())
  {
    payerId = AccountId::fromProtobuf(protoFeeLimit.account_id());
  }

  std::vector<CustomFixedFee> customFees;
  for (const auto& protoFee : protoFeeLimit.fees())
  {
    customFees.emplace_back(CustomFixedFee::fromProtobuf(protoFee));
  }

  return CustomFeeLimit().setPayerId(payerId.value_or(AccountId())).setCustomFees(customFees);
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
  std::string result = "CustomFeeLimit{PayerId: ";

  if (mPayerId)
  {
    result += mPayerId->toString();
  }
  else
  {
    result += "None";
  }

  result += ", CustomFees: [";

  for (size_t i = 0; i < mCustomFees.size(); ++i)
  {
    if (i > 0)
    {
      result += ", ";
    }
    result += mCustomFees[i].toString();
  }

  result += "]}";

  return result;
}
}