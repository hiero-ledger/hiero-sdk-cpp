// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_NFT_ALLOWANCE_H_
#define HIERO_SDK_CPP_TOKEN_NFT_ALLOWANCE_H_

#include "AccountId.h"
#include "TokenId.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace proto
{
class NftAllowance;
class NftRemoveAllowance;
}

namespace Hiero
{
/**
 * An approved allowance of NFT transfers for a spender.
 */
class TokenNftAllowance
{
public:
  TokenNftAllowance() = default;

  /**
   * Construct with a token ID, owner, spender, list of serial numbers, and approval policy.
   *
   * @param tokenId           The ID of the token NFT type that is being approved to be spent.
   * @param owner             The ID of the account approving an allowance of its NFTs.
   * @param spender           The ID of the account being allowed to spend the NFTs.
   * @param serialNumbers     A list serial numbers of the NFT that is being allowed to be spent.
   * @param allowAll          \c TRUE to allow the spender access to all of the owner's NFTs of the designated token ID,
   *                          \c FALSE to revoke the spender's access to all of the owner's NFTs of the designated token
   *                          ID.
   * @param delegatingSpender The ID of the account who has an 'approveForAll' allowance and is granting approval to
   *                          spend an NFT to the spender.
   */
  TokenNftAllowance(const std::optional<TokenId>& tokenId,
                    const std::optional<AccountId>& owner,
                    const std::optional<AccountId>& spender,
                    const std::vector<uint64_t>& serialNumbers,
                    const std::optional<bool>& allowAll = std::optional<bool>(),
                    const std::optional<AccountId>& delegatingSpender = std::optional<AccountId>());

  /**
   * Construct a TokenNftAllowance object from a NftAllowance protobuf object.
   *
   * @param proto The NftAllowance protobuf object from which to construct an TokenNftAllowance object.
   * @return The constructed TokenNftAllowance object.
   */
  [[nodiscard]] static TokenNftAllowance fromProtobuf(const proto::NftAllowance& proto);

  /**
   * Construct a TokenNftAllowance object from a byte array.
   *
   * @param bytes The byte array from which to construct an TokenNftAllowance object.
   * @return The constructed TokenNftAllowance object.
   */
  [[nodiscard]] static TokenNftAllowance fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Validate the checksums of the entity IDs in this TokenNftAllowance.
   *
   * @param client The Client to use to validate the checksums.
   * @throws BadEntityException If the checksums are not valid.
   */
  void validateChecksums(const Client& client) const;

  /**
   * Construct an NftAllowance protobuf object from this TokenNftAllowance object.
   *
   * @return A pointer to a constructed NftAllowance protobuf object filled with this TokenNftAllowance object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::NftAllowance> toProtobuf() const;

  /**
   * Construct an NftRemoveAllowance protobuf object from this TokenNftAllowance object.
   *
   * @return A pointer to a constructed NftRemoveAllowance protobuf object filled with this TokenNftAllowance object's
   *         data.
   */
  [[nodiscard]] std::unique_ptr<proto::NftRemoveAllowance> toRemoveProtobuf() const;

  /**
   * Construct a representative byte array from this TokenNftAllowance object.
   *
   * @return A representative byte array of this TokenNftAllowance object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * The ID of the token that is being approved to be spent.
   */
  std::optional<TokenId> mTokenId;

  /**
   * The ID of the account approving an allowance of its tokens.
   */
  std::optional<AccountId> mOwnerAccountId;

  /**
   * The ID of the account being allowed to spend the tokens.
   */
  std::optional<AccountId> mSpenderAccountId;

  /**
   * The list of serial numbers that are being allowed to be spent.
   */
  std::vector<uint64_t> mSerialNumbers;

  /**
   * Is the spender allowed access to allow of the owner's NFTs?
   */
  std::optional<bool> mApprovedForAll;

  /**
   * The ID of the account who has an 'approveForAll' allowance and is granting approval to spend an NFT to the spender.
   */
  std::optional<AccountId> mDelegatingSpenderAccountId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_NFT_ALLOWANCE_H_
