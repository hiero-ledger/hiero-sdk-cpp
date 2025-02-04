// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HBAR_TRANSFER_H_
#define HIERO_SDK_CPP_HBAR_TRANSFER_H_

#include "AccountId.h"
#include "Hbar.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class AccountAmount;
}

namespace Hiero
{
class HbarTransfer
{
public:
  HbarTransfer() = default;

  /**
   * Construct with an account ID, amount, and approval.
   *
   * @param accountId The ID of the account doing the transfer.
   * @param amount    The amount to transfer.
   * @param approval  \c TRUE if this is an approved allowance transfer, otherwise \c FALSE.
   */
  explicit HbarTransfer(AccountId accountId, const Hbar& amount, bool approval);

  /**
   * Construct an HbarTransfer object from an AccountAmount protobuf object.
   *
   * @param proto The AccountAmount protobuf object from which to construct an HbarTransfer object.
   * @return The constructed HbarTransfer object.
   */
  [[nodiscard]] static HbarTransfer fromProtobuf(const proto::AccountAmount& proto);

  /**
   * Construct an HbarTransfer object from a byte array.
   *
   * @param bytes The byte array from which to construct an HbarTransfer object.
   * @return The constructed HbarTransfer object.
   */
  [[nodiscard]] static HbarTransfer fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct an AccountAmount protobuf object from this HbarTransfer object.
   *
   * @return A pointer to the created HbarTransfer protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::AccountAmount> toProtobuf() const;

  /**
   * Construct a representative byte array from this HbarTransfer object.
   *
   * @return The byte array representing this HbarTransfer object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this HbarTransfer object.
   *
   * @return The string representation of this HbarTransfer object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this HbarTransfer to an output stream.
   *
   * @param os       The output stream.
   * @param transfer The HbarTransfer to print.
   * @return The output stream with this HbarTransfer written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const HbarTransfer& transfer);

  /**
   * The ID of the account associated with this HbarTransfer.
   */
  AccountId mAccountId;

  /**
   * The amount of Hbar transferred or to be transferred.
   */
  Hbar mAmount = Hbar(0ULL);

  /**
   * Is this transfer approved or not?
   */
  bool mIsApproved = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_HBAR_TRANSFER_H_
