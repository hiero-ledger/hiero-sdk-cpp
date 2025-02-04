// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_MINT_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_MINT_TRANSACTION_H_

#include "TokenId.h"
#include "Transaction.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace proto
{
class TokenMintTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Minting fungible token allows you to increase the total supply of the token. Minting a non-fungible token creates an
 * NFT with its unique metadata for the class of NFTs defined by the token ID. The Supply Key must sign the transaction.
 *
 * If no Supply Key is defined, the transaction will resolve to TOKEN_HAS_NO_SUPPLY_KEY. The maximum total supply a
 * token can have is 2^63-1.
 *
 * The amount provided must be in the lowest denomination possible.
 *  - Example: Token A has 2 decimals. In order to mint 100 tokens, one must provide an amount of 10000. In order to
 *             mint 100.55 tokens, one must provide an amount of 10055.
 *
 * The metadata field is specific to NFTs. Once an NFT is minted, the metadata cannot be changed and is immutable.
 *  - You can use the metadata field to add a URI that contains additional information about the token. The metadata
 *    field has a 100-character limit.
 *
 * The serial number for the NFT is returned in the receipt of the transaction.
 *
 * When minting NFTs, do not set the amount. The amount is used for minting fungible tokens only.
 *
 * This transaction accepts zero unit minting operations for fungible tokens (HIP-564)
 *
 * Transaction Signing Requirements:
 *  - Supply key
 *  - Transaction fee payer account key
 */
class TokenMintTransaction : public Transaction<TokenMintTransaction>
{
public:
  TokenMintTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenMint transaction.
   */
  explicit TokenMintTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenMintTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the token to mint.
   *
   * @param tokenId The ID of the token to mint.
   * @return A reference to this TokenMintTransaction with the newly-set token ID.
   */
  TokenMintTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Set the amount of the FUNGIBLE_COMMON token to mint, in the lowest denomination.
   *
   * @param amount The amount of token to mint, in the lowest denomination.
   * @return A reference to this TokenMintTransaction with the newly-set amount.
   */
  TokenMintTransaction& setAmount(const uint64_t& amount);

  /**
   * Set the metadata for the NON_FUNGIBLE_UNIQUE tokens to mint.
   *
   * @param metadata The metadata for the tokens to mint.
   * @return A reference to this TokenMintTransaction with the newly-set metadata.
   */
  TokenMintTransaction& setMetadata(const std::vector<std::vector<std::byte>>& metadata);

  /**
   * Add metadata for a NON_FUNGIBLE_UNIQUE token to this TokenMintTransaction.
   *
   * @param metadata The metadata to add.
   * @return A reference to this TokenMintTransaction with the newly-added metadata.
   */
  TokenMintTransaction& addMetadata(const std::vector<std::byte>& metadata);

  /**
   * Get the ID of the token to mint.
   *
   * @return The ID of the token to mint.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

  /**
   * Get the amount of the FUNGIBLE_COMMON token to mint, in the lowest denomination.
   *
   * @return The amount of the FUNGIBLE_COMMON token to mint, in the lowest denomination.
   */
  [[nodiscard]] inline uint64_t getAmount() const { return mAmount; }

  /**
   * Get the metadata for the NON_FUNGIBLE_UNIQUE tokens to mint.
   *
   * @return The metadata for the NON_FUNGIBLE_UNIQUE tokens to mint.
   */
  [[nodiscard]] inline std::vector<std::vector<std::byte>> getMetadata() const { return mMetadataList; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenMintTransaction's data to a
   * Node.
   *
   * @param request  The Transaction protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Transaction& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::TransactionResponse* response) const override;

  /**
   * Derived from Transaction. Verify that all the checksums in this TokenMintTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenMintTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenMintTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenMintTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenMintTransactionBody protobuf object from this TokenMintTransaction object.
   *
   * @return A pointer to a TokenMintTransactionBody protobuf object filled with this TokenMintTransaction object's
   *         data.
   */
  [[nodiscard]] proto::TokenMintTransactionBody* build() const;

  /**
   * The ID of the token for which to mint additional tokens.
   */
  TokenId mTokenId;

  /**
   * The amount of the token to mint. This is for tokens of type FUNGIBLE_COMMON. The amount provided must be in the
   * lowest denomination possible (i.e. if a token has 2 decimals, a value of 10,000 here will mint 100 tokens).
   */
  uint64_t mAmount = 0ULL;

  /**
   * The metadata of the NFTs to mint. This for tokens of type NON_FUNGIBLE_UNIQUE. Once an NFT is minted, its metadata
   * cannot be changed and is immutable. You can use the metadata field to add a URI that contains additional
   * information about the token. The metadata field has a 100 byte limit.
   */
  std::vector<std::vector<std::byte>> mMetadataList;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_MINT_TRANSACTION_H_
