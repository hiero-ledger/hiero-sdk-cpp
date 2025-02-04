// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HBAR_ALLOWANCE_H_
#define HIERO_SDK_CPP_HBAR_ALLOWANCE_H_

#include "AccountId.h"
#include "Hbar.h"

#include <memory>

namespace proto
{
class CryptoAllowance;
}

namespace Hiero
{
class Client;
}

namespace Hiero
{
/**
 * An approved allowance of Hbar transfers for a spender.
 */
class HbarAllowance
{
public:
  HbarAllowance() = default;

  /**
   * Construct with an owner, spender, and an amount.
   *
   * @param owner   The ID of the account approving an allowance of its Hbars.
   * @param spender The ID of the account being allowed to spend the Hbars.
   * @param amount  The amount of Hbars that are being allowed to be spent.
   */
  HbarAllowance(AccountId owner, AccountId spender, const Hbar& amount);

  /**
   * Construct an HbarAllowance object from a CryptoAllowance protobuf object.
   *
   * @param proto The CryptoAllowance protobuf object from which to construct an HbarAllowance object.
   * @return The constructed HbarAllowance object.
   */
  [[nodiscard]] static HbarAllowance fromProtobuf(const proto::CryptoAllowance& proto);

  /**
   * Validate the checksums of the entity IDs in this HbarAllowance.
   *
   * @param client The Client to use to validate the checksums.
   * @throws BadEntityException If the checksums are not valid.
   */
  void validateChecksums(const Client& client) const;

  /**
   * Construct a CryptoAllowance protobuf object from this HbarAllowance object.
   *
   * @return A pointer to a constructed CryptoAllowance protobuf object filled with this HbarAllowance object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::CryptoAllowance> toProtobuf() const;

  /**
   * The ID of the account approving an allowance of its Hbars.
   */
  AccountId mOwnerAccountId;

  /**
   * The ID of the account being allowed to spend the Hbars.
   */
  AccountId mSpenderAccountId;

  /**
   * The amount of Hbars that are being allowed to be spent.
   */
  Hbar mAmount;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_HBAR_ALLOWANCE_H_
