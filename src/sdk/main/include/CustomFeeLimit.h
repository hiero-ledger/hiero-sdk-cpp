// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CUSTOM_FEE_LIMIT_H_
#define HIERO_SDK_CPP_CUSTOM_FEE_LIMIT_H_

#include "AccountId.h"
#include "CustomFixedFee.h"
#include <optional>
#include <string>
#include <vector>

namespace proto
{
class CustomFeeLimit;
}

namespace Hiero
{
/**
 * Represents a maximum custom fee a user is willing to pay.
 */
class CustomFeeLimit
{
public:
  /**
   * Creates a CustomFeeLimit object from a protobuf representation.
   *
   * @param protoFeeLimit The protobuf object to convert from.
   * @return A CustomFeeLimit object constructed from the protobuf data.
   */
  [[nodiscard]] static CustomFeeLimit fromProtobuf(const proto::CustomFeeLimit& protoFeeLimit);

  /**
   * Converts this CustomFeeLimit object to a protobuf representation.
   *
   * @return A pointer to proto::CustomFeeLimit object.
   */
  [[nodiscard]] std::unique_ptr<proto::CustomFeeLimit> toProtobuf() const;

  /**
   * Sets the payer ID for the fee limit.
   *
   * @param payerId The AccountId to set.
   * @return A reference to this object for method chaining.
   */
  CustomFeeLimit& setPayerId(const AccountId& payerId);

  /**
   * Gets the payer ID.
   *
   * @return The optional AccountId of the payer.
   */
  [[nodiscard]] std::optional<AccountId> getPayerId() const;

  /**
   * Sets the custom fees.
   *
   * @param customFees The list of custom fees.
   * @return A reference to this object for method chaining.
   */
  CustomFeeLimit& setCustomFees(const std::vector<CustomFixedFee>& customFees);

  /**
   * Adds a custom fee to the list.
   *
   * @param customFee The CustomFixedFee to add.
   * @return A reference to this object for method chaining.
   */
  CustomFeeLimit& addCustomFee(const CustomFixedFee& customFee);

  /**
   * Gets the custom fees.
   *
   * @return A vector of CustomFixedFee objects.
   */
  [[nodiscard]] std::vector<CustomFixedFee> getCustomFees() const;

  /**
   * Converts the CustomFeeLimit object to a human-readable string.
   *
   * @return A string representation of the CustomFeeLimit.
   */
  [[nodiscard]] std::string toString() const;

private:
  /**
   * Optional payer ID for the fee.
   */
  std::optional<AccountId> mPayerId;

  /**
   * List of custom fees.
   */
  std::vector<CustomFixedFee> mCustomFees;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CUSTOM_FEE_LIMIT_H_
