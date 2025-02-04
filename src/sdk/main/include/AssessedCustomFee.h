// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ASSESSED_CUSTOM_FEE_H_
#define HIERO_SDK_CPP_ASSESSED_CUSTOM_FEE_H_

#include "AccountId.h"
#include "TokenId.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class AssessedCustomFee;
}

namespace Hiero
{
/**
 * A custom transfer fee that was assessed during the handling of a CryptoTransfer.
 */
class AssessedCustomFee
{
public:
  /**
   * Construct an AssessedCustomFee object from an AssessedCustomFee protobuf object.
   *
   * @param proto The AssessedCustomFee protobuf object from which to construct an AssessedCustomFee object.
   * @return The constructed AssessedCustomFee object.
   */
  [[nodiscard]] static AssessedCustomFee fromProtobuf(const proto::AssessedCustomFee& proto);

  /**
   * Construct an AssessedCustomFee object from a byte array.
   *
   * @param bytes The byte array from which to construct an AssessedCustomFee object.
   * @return The constructed AssessedCustomFee object.
   */
  [[nodiscard]] static AssessedCustomFee fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct an AssessedCustomFee protobuf object from this AssessedCustomFee object.
   *
   * @return A pointer to the created AssessedCustomFee protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::AssessedCustomFee> toProtobuf() const;

  /**
   * Construct a representative byte array from this AssessedCustomFee object.
   *
   * @return The byte array representing this AssessedCustomFee object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this AssessedCustomFee object.
   *
   * @return The string representation of this AssessedCustomFee object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this AssessedCustomFee to an output stream.
   *
   * @param os  The output stream.
   * @param fee The AssessedCustomFee to print.
   * @return The output stream with this AssessedCustomFee written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const AssessedCustomFee& fee);

  /**
   * The number of units assessed for the fee.
   */
  int64_t mAmount = 0LL;

  /**
   * The denomination of the fee. The denomination is Hbar if left unset.
   */
  std::optional<TokenId> mTokenId;

  /**
   * The ID of the account to receive the assessed fee.
   */
  AccountId mFeeCollectorAccountId;

  /**
   * The IDs of the accounts who paid this fee (i.e. their balances would be higher if not for this fee).
   */
  std::vector<AccountId> mPayerAccountIdList;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ASSESSED_CUSTOM_FEE_H_
