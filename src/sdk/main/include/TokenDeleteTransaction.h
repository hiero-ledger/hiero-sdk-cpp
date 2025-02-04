// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_DELETE_TRANSACTION_H_

#include "TokenId.h"
#include "Transaction.h"

#include <optional>

namespace proto
{
class TokenDeleteTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Deleting a token marks a token as deleted, though it will remain in the ledger. The operation must be signed by the
 * specified admin key of the token. If the admin key is not set, the transaction will result in TOKEN_IS_IMMUTABlE.
 * Once deleted, update, mint, burn, wipe, freeze, unfreeze, grant KYC, revoke KYC and token transfer transactions will
 * resolve to TOKEN_WAS_DELETED.
 *
 * For NFTs, you cannot delete a specific NFT. You can delete the class of the NFT specified by the token ID after you
 * have burned all associated NFTs associated with the token class
 *
 * Transaction Signing Requirements:
 *  - Admin key
 *  - Transaction fee payer account key
 */
class TokenDeleteTransaction : public Transaction<TokenDeleteTransaction>
{
public:
  TokenDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenDelete transaction.
   */
  explicit TokenDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenDeleteTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the token to delete.
   *
   * @param tokenId The ID of the token to delete.
   * @return A reference to this TokenDeleteTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenDeleteTransaction is frozen.
   */
  TokenDeleteTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Get the ID of the token this TokenDeleteTransaction is currently configured to delete.
   *
   * @return The ID of the account this TokenDeleteTransaction is currently configured to delete. Uninitialized if no
   *         token ID has been set.
   */
  [[nodiscard]] inline std::optional<TokenId> getTokenId() const { return mTokenId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenDeleteTransaction's data to
   * a Node.
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
   * Derived from Transaction. Verify that all the checksums in this TokenDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenDeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenDeleteTransactionBody protobuf object from this TokenDeleteTransaction object.
   *
   * @return A pointer to a TokenDeleteTransactionBody protobuf object filled with this TokenDeleteTransaction
   *         object's data.
   */
  [[nodiscard]] proto::TokenDeleteTransactionBody* build() const;

  /**
   * The ID of the token to delete.
   */
  std::optional<TokenId> mTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_DELETE_TRANSACTION_H_
