// SPDX-License-Identifier: Apache-2.0
#include "CustomFeeBase.h"
#include "CustomFixedFee.h"
#include "CustomFractionalFee.h"
#include "CustomRoyaltyFee.h"

#include <custom_fees.pb.h>

namespace Hiero
{
//-----
template<typename FeeType>
FeeType& CustomFeeBase<FeeType>::setFeeCollectorAccountId(const AccountId& accountId)
{
  mFeeCollectorAccountId = accountId;
  return static_cast<FeeType&>(*this);
}

//-----
template<typename FeeType>
FeeType& CustomFeeBase<FeeType>::setAllCollectorsAreExempt(bool exempt)
{
  mAllCollectorsAreExempt = exempt;
  return static_cast<FeeType&>(*this);
}

/**
 * Explicit template instantiations
 */
template class CustomFeeBase<CustomFixedFee>;
template class CustomFeeBase<CustomFractionalFee>;
template class CustomFeeBase<CustomRoyaltyFee>;

} // namespace Hiero
