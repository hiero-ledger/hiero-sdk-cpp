// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_FREEZE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_FREEZE_TRANSACTION_H_

#include "AccountId.h"
#include "TokenId.h"
#include "Transaction.h"

namespace proto
{
class TokenFreezeAccountTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Freezes transfers of the specified token for the account. The transaction must be signed by the token's freeze key.
 *
 *  - If the provided account is not found, the transaction will resolve to INVALID_ACCOUNT_ID.
 *  - If the provided account has been deleted, the transaction will resolve to ACCOUNT_DELETED.
 *  - If the provided token is not found, the transaction will resolve to INVALID_TOKEN_ID.
 *  - If the provided token has been deleted, the transaction will resolve to TOKEN_WAS_DELETED.
 *  - If an Association between the provided token and account is not found, the transaction will resolve to
 *    TOKEN_NOT_ASSOCIATED_TO_ACCOUNT.
 *  - If no Freeze Key is defined, the transaction will resolve to TOKEN_HAS_NO_FREEZE_KEY.
 *
 * Once executed the Account is marked as Frozen and will not be able to receive or send tokens unless unfrozen. The
 * operation is idempotent.
 *
 * Transaction Signing Requirements:
 *  - Freeze key.
 *  - Transaction fee payer account key.
 */
class TokenFreezeTransaction : public Transaction<TokenFreezeTransaction>
{
public:
  TokenFreezeTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenFreeze transaction.
   */
  explicit TokenFreezeTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenFreezeTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the account to be frozen for the specified token.
   *
   * @param accountId The ID of the account to be frozen for the specified token.
   * @return A reference to this TokenFreezeTransaction object with the newly-set account ID.
   * @throws IllegalStateException If this TokenFreezeTransaction is frozen.
   */
  TokenFreezeTransaction& setAccountId(const AccountId& accountId);

  /**
   * Set the ID of the token to be frozen for the specified account.
   *
   * @param tokenId The ID of the token to be frozen for the specified account.
   * @return A reference to this TokenFreezeTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenFreezeTransaction is frozen.
   */
  TokenFreezeTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Get the ID of the account to be frozen for the specified token.
   *
   * @return The ID of the account to be frozen for the specified token.
   */
  [[nodiscard]] inline AccountId getAccountId() const { return mAccountId; }

  /**
   * Get the ID of the token to be frozen for the specified account.
   *
   * @return The ID of the token to be frozen for the specified account.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenFreezeTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this TokenFreezeTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenFreezeTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenFreezeTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenFreezeTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenFreezeAccountTransactionBody protobuf object from this TokenFreezeTransaction object.
   *
   * @return A pointer to a TokenFreezeAccountTransactionBody protobuf object filled with this TokenFreezeTransaction
   *         object's data.
   */
  [[nodiscard]] proto::TokenFreezeAccountTransactionBody* build() const;

  /**
   * The ID of the account to be frozen for the specified token.
   */
  AccountId mAccountId;

  /**
   * The ID of the token to be frozen for the specified account.
   */
  TokenId mTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_FREEZE_TRANSACTION_H_
