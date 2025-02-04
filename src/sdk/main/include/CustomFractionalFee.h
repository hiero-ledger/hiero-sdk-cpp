// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CUSTOM_FRACTIONAL_FEE_H_
#define HIERO_SDK_CPP_CUSTOM_FRACTIONAL_FEE_H_

#include "CustomFeeBase.h"
#include "FeeAssessmentMethod.h"

#include <memory>
#include <optional>

namespace proto
{
class FractionalFee;
}

namespace Hiero
{
/**
 * A fraction of the transferred units of a token to assess as a fee. The amount assessed will never be less than the
 * given mMinimumAmount, and never greater than the given mMaximumAmount. The denomination is always units of the token
 * to which this fractional fee is attached.
 */
class CustomFractionalFee : public CustomFeeBase<CustomFractionalFee>
{
public:
  /**
   * Create a CustomFractionalFee object from a FractionalFee protobuf object.
   *
   * @param proto The FractionalFee protobuf object from which to create an CustomFractionalFee object.
   * @return A pointer to the constructed CustomFractionalFee object.
   */
  [[nodiscard]] static CustomFractionalFee fromProtobuf(const proto::FractionalFee& proto);

  /**
   * Derived from CustomFee. Create a clone of this CustomFractionalFee object.
   *
   * @return A pointer to the created clone of this CustomFractionalFee.
   */
  [[nodiscard]] std::unique_ptr<CustomFee> clone() const override;

  /**
   * Derived from CustomFee. Construct a CustomFee protobuf object from this CustomFractionalFee object.
   *
   * @return A pointer to the created CustomFee protobuf object filled with this CustomFractionalFee object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::CustomFee> toProtobuf() const override;

  /**
   * Derived from CustomFee. Construct a string representation of this CustomFractionalFee object.
   *
   * @return The string representation of this CustomFractionalFee object.
   */
  [[nodiscard]] std::string toString() const override;

  /**
   * Set the desired numerator of the fractional amount of the transferred units to assess as a part of this
   * CustomFractionalFee.
   *
   * @param numerator The desired numerator of the fractional amount of the transferred units to assess as a part of
   *                  this CustomFractionalFee.
   * @return A reference to this CustomFractionalFee object, with the newly-set numerator.
   */
  CustomFractionalFee& setNumerator(const int64_t& numerator);

  /**
   * Set the desired denominator of the fractional amount of the transferred units to assess as a part of this
   * CustomFractionalFee.
   *
   * @param denominator The desired denominator of the fractional amount of the transferred units to assess as a part of
   *                    this CustomFractionalFee.
   * @return A reference to this CustomFractionalFee object, with the newly-set denominator.
   */
  CustomFractionalFee& setDenominator(const int64_t& denominator);

  /**
   * Set the desired minimum amount to assess as a part of this CustomFractionalFee.
   *
   * @param amount The desired minimum amount to assess as a part of this CustomFractionalFee.
   * @return A reference ot this CustomFractionalFee object, with the newly-set minimum amount.
   */
  CustomFractionalFee& setMinimumAmount(const uint64_t& amount);

  /**
   * Set the desired maximum amount to assess as a part of this CustomFractionalFee.
   *
   * @param amount The desired maximum amount to assess as a part of this CustomFractionalFee.
   * @return A reference ot this CustomFractionalFee object, with the newly-set maximum amount.
   */
  CustomFractionalFee& setMaximumAmount(const uint64_t& amount);

  /**
   * Set the desired fee assessment method.
   *
   * @param method The desired fee assessment method.
   * @return A reference ot this CustomFractionalFee object, with the newly-set fee assessment method.
   */
  CustomFractionalFee& setAssessmentMethod(FeeAssessmentMethod method);

  /**
   * Get the desired numerator of the fractional amount of the transferred units to assess as a part of this
   * CustomFractionalFee.
   *
   * @return The desired numerator of the fractional amount of the transferred units to assess as a part of this
   *         CustomFractionalFee.
   */
  [[nodiscard]] inline int64_t getNumerator() const { return mNumerator; }

  /**
   * Get the desired denominator of the fractional amount of the transferred units to assess as a part of this
   * CustomFractionalFee.
   *
   * @return The desired denominator of the fractional amount of the transferred units to assess as a part of this
   *         CustomFractionalFee.
   */
  [[nodiscard]] inline int64_t getDenominator() const { return mDenominator; }

  /**
   * Get the desired minimum amount to assess as a part of this CustomFractionalFee.
   *
   * @return The desired minimum amount to assess as a part of this CustomFractionalFee.
   */
  [[nodiscard]] inline uint64_t getMinimumAmount() const { return mMinAmount; }

  /**
   * Get the desired maximum amount to assess as a part of this CustomFractionalFee.
   *
   * @return The desired maximum amount to assess as a part of this CustomFractionalFee.
   */
  [[nodiscard]] inline uint64_t getMaximumAmount() const { return mMaxAmount; }

  /**
   * Get the desired fee assessment method.
   *
   * @return The desired fee assessment method.
   */
  [[nodiscard]] inline FeeAssessmentMethod getAssessmentMethod() const { return mAssessmentMethod; }

private:
  /**
   * The numerator of the fractional amount of the transferred units to assess as a part of this CustomFractionalFee.
   */
  int64_t mNumerator = 0LL;

  /**
   * The denominator of the fractional amount of the transferred units to assess as a part of this CustomFractionalFee.
   */
  int64_t mDenominator = 1LL;

  /**
   * The desired minimum amount to assess as a part of this CustomFractionalFee
   */
  uint64_t mMinAmount = 0ULL;

  /**
   * The desired maximum amount to assess as a part of this CustomFractionalFee
   */
  uint64_t mMaxAmount = 0ULL;

  /**
   * How is this CustomFractionalFee assessed?
   */
  FeeAssessmentMethod mAssessmentMethod = FeeAssessmentMethod::INCLUSIVE;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CUSTOM_FRACTIONAL_FEE_H_
