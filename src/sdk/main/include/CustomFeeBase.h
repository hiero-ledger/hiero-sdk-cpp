// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CUSTOM_FEE_BASE_H_
#define HIERO_SDK_CPP_CUSTOM_FEE_BASE_H_

#include "AccountId.h"
#include "CustomFee.h"

namespace Hiero
{
/**
 * Helper class to sit between CustomFee and its derived implementations. This is done so that CustomFee isn't required
 * to be a templated type.
 */
template<typename FeeType>
class CustomFeeBase : public CustomFee
{
public:
  /**
   * Set the ID of the desired fee collector account.
   *
   * @param accountId The ID of the desired fee collector account.
   * @return A reference to this derived CustomFee object, with the newly-set fee collector account ID.
   */
  FeeType& setFeeCollectorAccountId(const AccountId& accountId);

  /**
   * Set the fee collector exemption policy.
   *
   * @param exempt \c TRUE if fee collectors should be exempt from this CustomFee, otherwise \c FALSE.
   * @return A reference ot this derived CustomFee object, with the newly-set fee collector exemption policy.
   */
  FeeType& setAllCollectorsAreExempt(bool exempt);

protected:
  /**
   * Prevent public copying and moving to prevent slicing. Use the 'clone()' virtual method instead.
   */
  CustomFeeBase() = default;
  CustomFeeBase(const CustomFeeBase&) = default;
  CustomFeeBase& operator=(const CustomFeeBase&) = default;
  CustomFeeBase(CustomFeeBase&&) noexcept = default;
  CustomFeeBase& operator=(CustomFeeBase&&) noexcept = default;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CUSTOM_FEE_BASE_H_
