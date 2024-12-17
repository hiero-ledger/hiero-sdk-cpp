// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_NFT_TRANSFER_H_
#define HIERO_SDK_CPP_TOKEN_NFT_TRANSFER_H_

#include "AccountId.h"
#include "NftId.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class NftTransfer;
}

namespace Hiero
{
/**
 * A sender account, a receiver account, and the serial number of an NFT of a Token with NON_FUNGIBLE_UNIQUE type. When
 * minting NFTs the sender will be the default AccountID instance (0.0.0) and when burning NFTs, the receiver will be
 * the default AccountID instance.
 */
class TokenNftTransfer
{
public:
  TokenNftTransfer() = default;

  /**
   * Construct with an nft ID, sender account ID, receiver account ID, and approval.
   *
   * @param nftId The ID of the NFT.
   * @param sender The ID of the account sending the NFT.
   * @param receiver The ID of the account receiving the NFT.
   * @param approved \c TRUE if this is an approved allowance NFT transfer, otherwise \c FALSE.
   */
  TokenNftTransfer(NftId nftId, AccountId sender, AccountId receiver, bool approved);

  /**
   * Construct a TokenNftTransfer object from an NftTransfer protobuf object and a TokenId object.
   *
   * @param proto   The NftTransfer protobuf object from which to construct an TokenNftTransfer object.
   * @param tokenId The ID of the token.
   * @return The constructed TokenNftTransfer object.
   */
  [[nodiscard]] static TokenNftTransfer fromProtobuf(const proto::NftTransfer& proto, const TokenId& tokenId);

  /**
   * Construct a TokenNftTransfer object from a byte array.
   *
   * @param bytes The byte array from which to construct a TokenNftTransfer object.
   * @return The constructed TokenNftTransfer object.
   */
  [[nodiscard]] static TokenNftTransfer fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Validate the checksums of the entities associated with this TokenNftTransfer.
   *
   * @param client The Client to use to validate the checksums.
   * @throws BadEntityException If any of the checksums are not valid.
   */
  void validateChecksums(const Client& client) const;

  /**
   * Construct a NftTransfer protobuf object from this TokenNftTransfer object.
   *
   * @return A pointer to the created NftTransfer protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::NftTransfer> toProtobuf() const;

  /**
   * Construct a representative byte array from this TokenNftTransfer object.
   *
   * @return The byte array representing this TokenNftTransfer object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this TokenNftTransfer object.
   *
   * @return The string representation of this TokenNftTransfer object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this TokenNftTransfer to an output stream.
   *
   * @param os       The output stream.
   * @param transfer The TokenNftTransfer to print.
   * @return The output stream with this TokenNftTransfer written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const TokenNftTransfer& transfer);

  /**
   * The ID of the NFT.
   */
  NftId mNftId;

  /**
   * The account ID of the sender.
   */
  AccountId mSenderAccountId;

  /**
   * The account ID of the receiver.
   */
  AccountId mReceiverAccountId;

  /**
   * If \c TRUE then the transfer is expected to be an approved allowance and the senderAccountID is expected to be the
   * owner.
   */
  bool mIsApproval = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_NFT_TRANSFER_H_
