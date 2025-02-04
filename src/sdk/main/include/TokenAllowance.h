// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_ALLOWANCE_H_
#define HIERO_SDK_CPP_TOKEN_ALLOWANCE_H_

#include "AccountId.h"
#include "TokenId.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace proto
{
class TokenAllowance;
}

namespace Hiero
{
class Client;
}

namespace Hiero
{
/**
 * An approved allowance of fungible token transfers for a spender.
 */
class TokenAllowance
{
public:
  TokenAllowance() = default;

  /**
   * Construct with a token ID, owner, spender, and amount.
   *
   * @param tokenId The ID of the token that is being approved to be spent.
   * @param owner   The ID of the account approving an allowance of its tokens.
   * @param spender The ID of the account being allowed to spend the tokens.
   * @param amount  The amount of tokens that are being allowed to be spent.
   */
  TokenAllowance(const TokenId& tokenId, AccountId owner, AccountId spender, int64_t amount);

  /**
   * Construct a TokenAllowance object from a TokenAllowance protobuf object.
   *
   * @param proto The TokenAllowance protobuf object from which to construct an TokenAllowance object.
   * @return The constructed TokenAllowance object.
   */
  [[nodiscard]] static TokenAllowance fromProtobuf(const proto::TokenAllowance& proto);

  /**
   * Construct a TokenAllowance object from a byte array.
   *
   * @param bytes The byte array from which to construct an TokenAllowance object.
   * @return The constructed TokenAllowance object.
   */
  [[nodiscard]] static TokenAllowance fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Validate the checksums of the entity IDs in this TokenAllowance.
   *
   * @param client The Client to use to validate the checksums.
   * @throws BadEntityException If the checksums are not valid.
   */
  void validateChecksums(const Client& client) const;

  /**
   * Construct a TokenAllowance protobuf object from this TokenAllowance object.
   *
   * @return A pointer to a constructed TokenAllowance protobuf object filled with this TokenAllowance object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::TokenAllowance> toProtobuf() const;

  /**
   * Construct a byte array from this TokenAllowance object.
   *
   * @return A byte array representing this TokenAllowance object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * The ID of the token that is being approved to be spent.
   */
  TokenId mTokenId;

  /**
   * The ID of the account approving an allowance of its tokens.
   */
  AccountId mOwnerAccountId;

  /**
   * The ID of the account being allowed to spend the tokens.
   */
  AccountId mSpenderAccountId;

  /**
   * The amount of tokens that are being allowed to be spent.
   */
  int64_t mAmount = 0LL;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_ALLOWANCE_H_
