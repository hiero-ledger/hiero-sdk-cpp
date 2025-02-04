// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_DISSOCIATE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_DISSOCIATE_TRANSACTION_H_

#include "AccountId.h"
#include "TokenId.h"
#include "Transaction.h"

#include <optional>
#include <vector>

namespace proto
{
class TokenDissociateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Disassociates the provided Hiero account from the provided Hiero tokens. This transaction must be signed by the
 * provided account's key. Once the association is removed, no token related operation can be performed to that account.
 * AccountBalanceQuery and AccountInfoQuery will not return anything related to the token that was disassociated.
 *
 *  - If the provided account is not found, the transaction will resolve to INVALID_ACCOUNT_ID.
 *  - If the provided account has been deleted, the transaction will resolve to ACCOUNT_DELETED.
 *  - If any of the provided tokens is not found, the transaction will resolve to INVALID_TOKEN_REF.
 *  - If an association between the provided account and any of the tokens does not exist, the transaction will resolve
 *    to TOKEN_NOT_ASSOCIATED_TO_ACCOUNT.
 *  - If the provided account has a nonzero balance with any of the provided tokens, the transaction will resolve to
 *    TRANSACTION_REQUIRES_ZERO_TOKEN_BALANCES.
 *
 * On success, associations between the provided account and tokens are removed. The account is required to have a zero
 * balance of the token you wish to disassociate. If a token balance is present, you will receive a
 * TRANSACTION_REQUIRES_ZERO_TOKEN_BALANCES error.
 *
 * Transaction Signing Requirements:
 *  - The key of the account from which the token is being dissociated.
 *  - Transaction fee payer account key.
 */
class TokenDissociateTransaction : public Transaction<TokenDissociateTransaction>
{
public:
  TokenDissociateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenDissociate transaction.
   */
  explicit TokenDissociateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenDissociateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the account to be dissociated from the provided tokens.
   *
   * @param accountId The ID of the account to be dissociated from the provided tokens.
   * @return A reference to this TokenDissociateTransaction object with the newly-set account ID.
   * @throws IllegalStateException If this TokenDissociateTransaction is frozen.
   */
  TokenDissociateTransaction& setAccountId(const AccountId& accountId);

  /**
   * Set the IDs of the tokens to be dissociated from the provided account.
   *
   * @param tokenIds The IDs of the tokens to be dissociated from the provided account.
   * @return A reference to this TokenDissociateTransaction object with the newly-set token IDs.
   * @throws IllegalStateException If this TokenDissociateTransaction is frozen.
   */
  TokenDissociateTransaction& setTokenIds(const std::vector<TokenId>& tokenIds);

  /**
   * Get the ID of the account to be dissociated from the provided tokens.
   *
   * @return The ID of the account to be dissociated from the provided tokens. Returns uninitialized if no account ID
   *         has been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getAccountId() const { return mAccountId; }

  /**
   * Get the IDs of the tokens to be dissociated from the provided account.
   *
   * @return The IDs of the tokens to be dissociated from the provided account.
   */
  [[nodiscard]] inline std::vector<TokenId> getTokenIds() const { return mTokenIds; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenDissociateTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this TokenDissociateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenDissociateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenDissociateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenDissociateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenDissociateTransactionBody protobuf object from this TokenDissociateTransaction object.
   *
   * @return A pointer to a TokenDissociateTransactionBody protobuf object filled with this TokenDissociateTransaction
   *         object's data.
   */
  [[nodiscard]] proto::TokenDissociateTransactionBody* build() const;

  /**
   * The ID of the account to be dissociated from the provided tokens.
   */
  std::optional<AccountId> mAccountId;

  /**
   * The IDs of the tokens to be dissociated from the provided account.
   */
  std::vector<TokenId> mTokenIds;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_DISSOCIATE_TRANSACTION_H_
