// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_UNPAUSE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_UNPAUSE_TRANSACTION_H_

#include "TokenId.h"
#include "Transaction.h"

namespace proto
{
class TokenUnpauseTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A token unpause transaction is a transaction that unpauses the token that was previously disabled from participating
 * in transactions. The token's pause key is required to sign the transaction. Once the unpause transaction is submitted
 * the token pause status is updated to unpause.
 *
 * Transaction Signing Requirements:
 *  - The pause key of the token.
 *  - Transaction fee payer account key.
 */
class TokenUnpauseTransaction : public Transaction<TokenUnpauseTransaction>
{
public:
  TokenUnpauseTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenUnpause transaction.
   */
  explicit TokenUnpauseTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenUnpauseTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the token to unpause.
   *
   * @param tokenId The ID of the token to unpause.
   * @return A reference to this TokenUnpauseTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenUnpauseTransaction is frozen.
   */
  TokenUnpauseTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Get the ID of the token to unpause.
   *
   * @return The ID of the token to unpause.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenUnpauseTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this TokenUnpauseTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenUnpauseTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenUnpauseTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenUnpauseTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenUnpauseTransactionBody protobuf object from this TokenUnpauseTransaction object.
   *
   * @return A pointer to a TokenUnpauseTransactionBody protobuf object filled with this TokenUnpauseTransaction
   *         object's data.
   */
  [[nodiscard]] proto::TokenUnpauseTransactionBody* build() const;

  /**
   * The ID of the token to unpause.
   */
  TokenId mTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_UNPAUSE_TRANSACTION_H_
