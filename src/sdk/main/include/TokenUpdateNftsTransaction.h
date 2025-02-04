// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_UPDATE_NFTS_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_UPDATE_NFTS_TRANSACTION_H_

#include "AccountId.h"
#include "TokenId.h"
#include "Transaction.h"

#include <chrono>
#include <memory>

namespace proto
{
class TokenUpdateNftsTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * At consensus, updates an already created Non Fungible Token to the given values.
 *
 * If no value is given for a field, that field is left unchanged.
 * Only certain fields such as metadata can be updated.
 *
 * Updating the metadata of an NFT does not affect its ownership or transferability.
 * This operation is intended for updating attributes of individual NFTs in a collection.
 *
 * Transaction Signing Requirements
 *  - To update metadata of an NFT, the metadata_key of the token should sign the transaction.
 */
class TokenUpdateNftsTransaction : public Transaction<TokenUpdateNftsTransaction>
{
public:
  TokenUpdateNftsTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenUpdate transaction.
   */
  explicit TokenUpdateNftsTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenUpdateNftsTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the token to update.
   *
   * @param tokenId The ID of the token to update.
   * @return A reference to this TokenUpdateNftsTransaction with the newly-set token ID.
   */
  TokenUpdateNftsTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Set the token serials.
   *
   * @param serials The vector of serials.
   * @return A reference to this TokenUpdateNftsTransaction with the newly-set token serials.
   */
  TokenUpdateNftsTransaction& setSerials(const std::vector<uint64_t>& serials);

  /**
   * Set the new metadata for the tokens.
   *
   * @param metadata The new metadata for the tokens.
   * @return A reference to this TokenUpdateNftsTransaction with the newly-set token metadata.
   */
  TokenUpdateNftsTransaction& setMetadata(const std::vector<std::byte>& metadata);

  /**
   * Get the ID of the token to update.
   *
   * @return The ID of the token to update.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

  /**
   * Get the serials for the token.
   *
   * @return The serials for the token. Returns empty vector if serials are not set.
   */
  [[nodiscard]] inline std::vector<uint64_t> getSerials() const { return mSerials; }

  /**
   * Get the new metadata for the tokens.
   *
   * @return The new metadata for the tokens. Returns uninitialized if no new symbol has been set.
   */
  [[nodiscard]] inline std::vector<std::byte> getMetadata() const { return mMetadata; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenUpdateNftsTransaction's data
   * to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this TokenUpdateNftsTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenUpdateNftsTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenUpdateNftsTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenUpdateNftsTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenUpdateNftsTransactionBody protobuf object from this TokenUpdateNftsTransaction object.
   *
   * @return A pointer to a TokenUpdateNftsTransactionBody protobuf object filled with this TokenUpdateNftsTransaction
   * object's data.
   */
  [[nodiscard]] proto::TokenUpdateNftsTransactionBody* build() const;

  /**
   * The token for which to update NFTs.
   */
  TokenId mTokenId;

  /**
   * The list of serial numbers to be updated.
   */
  std::vector<uint64_t> mSerials;

  /**
   * The new metadata of the NFT(s)
   */
  std::vector<std::byte> mMetadata;
};

}
// namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_UPDATE_NFTS_TRANSACTION_H_
