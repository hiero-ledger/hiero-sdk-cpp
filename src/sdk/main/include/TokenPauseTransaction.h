// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_PAUSE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_PAUSE_TRANSACTION_H_

#include "TokenId.h"
#include "Transaction.h"

namespace proto
{
class TokenPauseTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A token pause transaction prevents the token from being involved in any kind of operation. The token's pause key is
 * required to sign the transaction. This is a key that is specified during the creation of a token. If a token has no
 * pause key, you will not be able to pause the token. If the pause key was not set during the creation of a token, you
 * will not be able to update the token to add this key.
 *
 * The following operations cannot be performed when a token is paused and will result in a TOKEN_IS_PAUSED status:
 *  - Updating the token
 *  - Transferring the token
 *  - Transferring any other token where it has its paused key in a custom fee schedule
 *  - Deleting the token
 *  - Minting or burning a token
 *  - Freezing or unfreezing an account that holds the token
 *  - Enabling or disabling KYC
 *  - Associating or disassociating a token
 *  - Wiping a token
 *
 * Once a token is paused, token status will update to paused. To verify if the token's status has been updated to
 * paused, you can request the token info via the SDK or use the token info mirror node query. If the token is not
 * paused the token status will be unpaused. The token status for tokens that do not have an assigned pause key will
 * state PauseNotApplicable.
 *
 * Transaction Signing Requirements:
 *  - The pause key of the token.
 *  - Transaction fee payer account key.
 */
class TokenPauseTransaction : public Transaction<TokenPauseTransaction>
{
public:
  TokenPauseTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenPause transaction.
   */
  explicit TokenPauseTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenPauseTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the tokens to pause.
   *
   * @param tokenId The ID of the tokens to pause.
   * @return A reference to this TokenPauseTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenPauseTransaction is frozen.
   */
  TokenPauseTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Get the ID of the tokens to pause.
   *
   * @return The ID of the tokens to pause.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenPauseTransaction's data to a
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
   * Derived from Transaction. Verify that all the checksums in this TokenPauseTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenPauseTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenPauseTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenPauseTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenPauseTransactionBody protobuf object from this TokenPauseTransaction object.
   *
   * @return A pointer to a TokenPauseTransactionBody protobuf object filled with this TokenPauseTransaction object's
   *         data.
   */
  [[nodiscard]] proto::TokenPauseTransactionBody* build() const;

  /**
   * The ID of the token to pause.
   */
  TokenId mTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_PAUSE_TRANSACTION_H_
