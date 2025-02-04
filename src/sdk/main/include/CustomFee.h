// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CUSTOM_FEE_H_
#define HIERO_SDK_CPP_CUSTOM_FEE_H_

#include "AccountId.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class CustomFee;
}

namespace Hiero
{
class Client;
}

namespace Hiero
{
/**
 * Base class for custom fees. This is assessed during a CryptoTransfer that transfers units of the token to which the
 * fee is attached. A custom fee may be either fixed, fractional, or a royalty, and must specify a fee collector account
 * to receive the assessed fees. Only positive fees may be assessed.
 */
class CustomFee
{
public:
  virtual ~CustomFee() = default;

  /**
   * Construct a CustomFee object from a CustomFee protobuf object.
   *
   * @param proto The CustomFee protobuf object from which to construct a CustomFee object.
   * @return A pointer to the constructed CustomFee object.
   */
  [[nodiscard]] static std::unique_ptr<CustomFee> fromProtobuf(const proto::CustomFee& proto);

  /**
   * Construct a CustomFee object from a byte array.
   *
   * @param bytes The byte array from which to construct a CustomFee object.
   * @return A pointer to the constructed CustomFee object.
   */
  [[nodiscard]] static std::unique_ptr<CustomFee> fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Create a clone of this CustomFee object.
   *
   * @return A pointer to the created clone of this CustomFee.
   */
  [[nodiscard]] virtual std::unique_ptr<CustomFee> clone() const = 0;

  /**
   * Construct a CustomFee protobuf object from this CustomFee object.
   *
   * @return A pointer to the created CustomFee protobuf object filled with this CustomFee object's data.
   */
  [[nodiscard]] virtual std::unique_ptr<proto::CustomFee> toProtobuf() const = 0;

  /**
   * Construct a string representation of this CustomFee object.
   *
   * @return The string representation of this CustomFee object.
   */
  [[nodiscard]] virtual std::string toString() const = 0;

  /**
   * Write this CustomFee to an output stream.
   *
   * @param os  The output stream.
   * @param fee The CustomFee to print.
   * @return The output stream with this CustomFee written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const CustomFee& fee);

  /**
   * Verify the checksums of all the entities involved in this CustomFee.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This CustomFee's checksums are not valid.
   */
  virtual void validateChecksums(const Client& client) const;

  /**
   * Construct a representative byte array from this CustomFee object.
   *
   * @return The byte array representing this CustomFee object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Get the ID of the desired fee collector account.
   *
   * @return The ID of the desired fee collector account.
   */
  [[nodiscard]] inline AccountId getFeeCollectorAccountId() const { return mFeeCollectorAccountId; }

  /**
   * Get the fee collector exemption policy.
   *
   * @return \c TRUE if fee collectors are currently configured to be exempt from this CustomFee, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getAllCollectorsAreExempt() const { return mAllCollectorsAreExempt; }

protected:
  /**
   * Prevent public copying and moving to prevent slicing. Use the 'clone()' virtual method instead.
   */
  CustomFee() = default;
  CustomFee(const CustomFee&) = default;
  CustomFee& operator=(const CustomFee&) = default;
  CustomFee(CustomFee&&) noexcept = default;
  CustomFee& operator=(CustomFee&&) noexcept = default;

  /**
   * Create the initial protobuf CustomFee object, populated with this CustomFee's private members, to be used by
   * derived classes' toProtobuf() functions.
   *
   * @return A pointer to CustomFee protobuf object filled-in with this CustomFee's private members.
   */
  [[nodiscard]] std::unique_ptr<proto::CustomFee> initProtobuf() const;

  /**
   * The ID of the account that should receive the fee.
   */
  AccountId mFeeCollectorAccountId;

  /**
   * Should all token fee collection accounts be exempt from this fee?
   */
  bool mAllCollectorsAreExempt = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CUSTOM_FEE_H_
