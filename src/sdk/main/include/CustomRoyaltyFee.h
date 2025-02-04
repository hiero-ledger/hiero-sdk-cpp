// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CUSTOM_ROYALTY_FEE_H_
#define HIERO_SDK_CPP_CUSTOM_ROYALTY_FEE_H_

#include "CustomFeeBase.h"
#include "CustomFixedFee.h"

#include <optional>

namespace proto
{
class RoyaltyFee;
}

namespace Hiero
{
/**
 * A fee to assess during a CryptoTransfer that changes ownership of an NFT. Defines the fraction of the fungible value
 * exchanged for an NFT that the ledger should collect as a royalty. ("Fungible value" includes both ℏ and units of
 * fungible HTS tokens.) When the NFT sender does not receive any fungible value, the ledger will assess the fallback
 * fee, if present, to the new NFT owner. Royalty fees can only be added to tokens of type NON_FUNGIBLE_UNIQUE.
 */
class CustomRoyaltyFee : public CustomFeeBase<CustomRoyaltyFee>
{
public:
  /**
   * Create a CustomRoyaltyFee object from a RoyaltyFee protobuf object.
   *
   * @param proto The RoyaltyFee protobuf object from which to create an CustomRoyaltyFee object.
   * @return A pointer to the constructed CustomRoyaltyFee object.
   */
  [[nodiscard]] static CustomRoyaltyFee fromProtobuf(const proto::RoyaltyFee& proto);

  /**
   * Derived from CustomFee. Create a clone of this CustomRoyaltyFee object.
   *
   * @return A pointer to the created clone of this CustomRoyaltyFee.
   */
  [[nodiscard]] std::unique_ptr<CustomFee> clone() const override;

  /**
   * Derived from CustomFee. Construct a CustomFee protobuf object from this CustomRoyaltyFee object.
   *
   * @return A pointer to the created CustomFee protobuf object filled with this CustomRoyaltyFee object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::CustomFee> toProtobuf() const override;

  /**
   * Derived from CustomFee. Construct a string representation of this CustomRoyaltyFee object.
   *
   * @return The string representation of this CustomRoyaltyFee object.
   */
  [[nodiscard]] std::string toString() const override;

  /**
   * Set the desired numerator of the fractional amount of the transferred units to assess as a part of this
   * CustomRoyaltyFee.
   *
   * @param numerator The desired numerator of the fractional amount of the transferred units to assess as a part of
   *                  this CustomRoyaltyFee.
   * @return A reference to this CustomRoyaltyFee object, with the newly-set numerator.
   */
  CustomRoyaltyFee& setNumerator(const int64_t& numerator);

  /**
   * Set the desired denominator of the fractional amount of the transferred units to assess as a part of this
   * CustomRoyaltyFee.
   *
   * @param denominator The desired denominator of the fractional amount of the transferred units to assess as a part of
   *                    this CustomRoyaltyFee.
   * @return A reference to this CustomRoyaltyFee object, with the newly-set denominator.
   */
  CustomRoyaltyFee& setDenominator(const int64_t& denominator);

  /**
   * Set the desired CustomFixedFee to be assessed to the NFT receiver when there is no fungible value exchanged with
   * the sender of the NFT.
   *
   * @param fee The desired CustomFixedFee to be assessed to the NFT receiver when there is no fungible value exchanged
   *            with the sender of the NFT.
   * @return A reference to this CustomRoyaltyFee object, with the newly-set fallback fee.
   */
  CustomRoyaltyFee& setFallbackFee(const CustomFixedFee& fee);

  /**
   * Set the desired numerator of the fractional amount of the transferred units to assess as a part of this
   * CustomRoyaltyFee.
   *
   * @return The desired numerator of the fractional amount of the transferred units to assess as a part of this
   *         CustomRoyaltyFee.
   */
  [[nodiscard]] inline int64_t getNumerator() const { return mNumerator; }

  /**
   * Set the desired denominator of the fractional amount of the transferred units to assess as a part of this
   * CustomRoyaltyFee.
   *
   * @return The desired denominator of the fractional amount of the transferred units to assess as a part of this
   *         CustomRoyaltyFee.
   */
  [[nodiscard]] inline int64_t getDenominator() const { return mDenominator; }

  /**
   * Get the desired CustomFixedFee to be assessed to the NFT receiver when there is no fungible value exchanged with
   * the sender of the NFT.
   *
   * @return The desired CustomFixedFee to be assessed to the NFT receiver when there is no fungible value exchanged
   *         with the sender of the NFT. Returns uninitialized if no fallback fee has yet been set.
   */
  [[nodiscard]] inline std::optional<CustomFixedFee> getFallbackFee() const { return mFallbackFee; }

private:
  /**
   * The numerator of the fractional amount of the transferred units to assess as a part of this CustomRoyaltyFee.
   */
  int64_t mNumerator = 0LL;

  /**
   * The denominator of the fractional amount of the transferred units to assess as a part of this CustomRoyaltyFee.
   */
  int64_t mDenominator = 1LL;

  /**
   * The CustomFixedFee to be assessed to the NFT receiver when there is no fungible value exchanged with the sender of
   * the NFT.
   */
  std::optional<CustomFixedFee> mFallbackFee;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CUSTOM_ROYALTY_FEE_H_
