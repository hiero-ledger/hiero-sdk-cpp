// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_TRANSFER_H_
#define HIERO_SDK_CPP_TOKEN_TRANSFER_H_

#include "AccountId.h"
#include "TokenId.h"

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
class Client;
}

namespace Hiero
{
/**
 * A token ID and list of amounts representing the transferred out (negative) or into (positive) amounts, represented
 * in the lowest denomination of the token.
 */
class TokenTransfer
{
public:
  TokenTransfer() = default;

  /**
   * Construct with a token ID, account ID, amount, and approval.
   *
   * @param tokenId    The ID of the token involved with this TokenTransfer.
   * @param accountId  The ID of the account to/from which the token is being transferred.
   * @param amount     The amount of the token being transferred.
   * @param isApproved \c TRUE if this transfer is approved, otherwise \c FALSE.
   */
  TokenTransfer(TokenId tokenId, AccountId accountId, int64_t amount, bool isApproved);

  /**
   * Construct with a token ID, account ID, amount, expected decimals of the token, and approval.
   *
   * @param tokenId    The ID of the token involved with this TokenTransfer.
   * @param accountId  The ID of the account to/from which the token is being transferred.
   * @param amount     The amount of the token being transferred.
   * @param isApproved \c TRUE if this transfer is approved, otherwise \c FALSE.
   */
  TokenTransfer(TokenId tokenId, AccountId accountId, int64_t amount, uint32_t decimals, bool isApproved);

  /**
   * Construct a TokenTransfer object from an AccountAmount protobuf object, a TokenId object, and the number of
   * expected decimals.
   *
   * @param proto    The AccountAmount protobuf object from which to construct the TokenTransfer object.
   * @param tokenId  The ID of the token.
   * @param decimals The number of expected decimals.
   * @return The constructed TokenTransfer object.
   */
  [[nodiscard]] static TokenTransfer fromProtobuf(const proto::AccountAmount& proto,
                                                  const TokenId& tokenId,
                                                  uint32_t expectedDecimals);

  /**
   * Construct a TokenTransfer object from a byte array.
   *
   * @param bytes The byte array from which to construct a TokenTransfer object.
   * @return The constructed TokenTransfer object.
   */
  [[nodiscard]] static TokenTransfer fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Validate the checksums of the entities in this TokenTransfer.
   *
   * @param client The Client to use to validate the checksums.
   * @throws BadEntityException If a checksum of one of the entities is not valid.
   */
  void validateChecksums(const Client& client) const;

  /**
   * Construct an AccountAmount protobuf object from this TokenTransfer object.
   *
   * @return A pointer to the constructed AccountAmount protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::AccountAmount> toProtobuf() const;

  /**
   * Construct a representative byte array from this TokenTransfer object.
   *
   * @return The byte array representing this TokenTransfer object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this TokenTransfer object.
   *
   * @return The string representation of this TokenTransfer object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this TokenTransfer to an output stream.
   *
   * @param os       The output stream.
   * @param transfer The TokenTransfer to print.
   * @return The output stream with this TokenTransfer written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const TokenTransfer& transfer);

  /**
   * The ID of the token being transferred.
   */
  TokenId mTokenId;

  /**
   * The ID of the account to/from which the token is being transferred.
   */
  AccountId mAccountId;

  /**
   * The amount of the token to transfer.
   */
  int64_t mAmount = 0LL;

  /**
   * The expected decimals of the transfer amount.
   */
  uint32_t mExpectedDecimals = 0U;

  /**
   * If \c TRUE then the transfer is expected to be an approved allowance.
   */
  bool mIsApproval = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_TRANSFER_H_
