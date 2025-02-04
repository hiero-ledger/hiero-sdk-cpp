// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CUSTOM_FIXED_FEE_H_
#define HIERO_SDK_CPP_CUSTOM_FIXED_FEE_H_

#include "CustomFeeBase.h"
#include "Hbar.h"
#include "TokenId.h"

#include <memory>
#include <optional>

namespace proto
{
class FixedFee;
}

namespace Hiero
{
/**
 * A fixed number of units (Hbar or token) to assess as a fee during a CryptoTransfer that transfers units of the token
 * to which this fixed fee is attached.
 */
class CustomFixedFee : public CustomFeeBase<CustomFixedFee>
{
public:
  /**
   * Create a CustomFixedFee object from a FixedFee protobuf object.
   *
   * @param proto The FixedFee protobuf object from which to create an CustomFixedFee object.
   * @return A pointer to the constructed CustomFixedFee object.
   */
  [[nodiscard]] static CustomFixedFee fromProtobuf(const proto::FixedFee& proto);

  /**
   * Derived from CustomFee. Create a clone of this CustomFixedFee object.
   *
   * @return A pointer to the created clone of this CustomFixedFee.
   */
  [[nodiscard]] std::unique_ptr<CustomFee> clone() const override;

  /**
   * Derived from CustomFee. Construct a CustomFee protobuf object from this CustomFixedFee object.
   *
   * @return A pointer to the created CustomFee protobuf object filled with this CustomFixedFee object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::CustomFee> toProtobuf() const override;

  /**
   * Derived from CustomFee. Construct a string representation of this CustomFee object.
   *
   * @return The string representation of this CustomFee object.
   */
  [[nodiscard]] std::string toString() const override;

  /**
   * Derived from CustomFee. Verify the checksums of all the entities involved in this CustomFixedFee.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This CustomFixedFee's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Construct a FixedFee protobuf object from this CustomFixedFee object.
   *
   * @return A pointer to the created FixedFee protobuf object filled with this CustomFixedFee object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::FixedFee> toFixedFeeProtobuf() const;

  /**
   * Set the desired fee amount.
   *
   * @param amount The desired fee amount.
   * @return A reference to this CustomFixedFee object, with the newly-set fee amount.
   */
  CustomFixedFee& setAmount(const uint64_t& amount);
  CustomFixedFee& setHbarAmount(const Hbar& amount);

  /**
   * Set the ID of the denominating token.
   *
   * @param tokenId The ID of the denominating token.
   * @return A reference ot this CustomFixedFee object, with the newly-set denominating token ID.
   */
  CustomFixedFee& setDenominatingTokenId(const TokenId& tokenId);

  /**
   * Set the ID of the denominating token to the token being created (as part of a TokenCreateTransaction).
   *
   * @return A reference ot this CustomFixedFee object, with the newly-set denominating token ID.
   */
  CustomFixedFee& setDenominatingTokenToSameToken();

  /**
   * Get the desired fee amount.
   *
   * @return The desired fee amount.
   */
  [[nodiscard]] inline uint64_t getAmount() const { return mAmount; }
  [[nodiscard]] inline Hbar getHbarAmount() const { return Hbar(static_cast<int64_t>(mAmount), HbarUnit::TINYBAR()); }

  /**
   * Get the ID of the denominating token.
   *
   * @return The ID of the denominating token. Returns uninitialized if a token ID has not yet been set.
   */
  [[nodiscard]] inline std::optional<TokenId> getDenominatingTokenId() const { return mDenominatingTokenId; }

private:
  /**
   * The number of units to assess as a fee.
   */
  uint64_t mAmount = 0ULL;

  /**
   * The denomination of the fee. If left unset, Hbar will be the denomination. In a TokenCreateTransaction, if set to
   * the sentinel value of 0.0.0, the created token will be the denomination.
   */
  std::optional<TokenId> mDenominatingTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CUSTOM_FIXED_FEE_H_
