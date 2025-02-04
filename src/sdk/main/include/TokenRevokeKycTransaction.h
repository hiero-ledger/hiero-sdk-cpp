// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_REVOKE_KYC_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_REVOKE_KYC_TRANSACTION_H_

#include "AccountId.h"
#include "TokenId.h"
#include "Transaction.h"

#include <optional>
#include <vector>

namespace proto
{
class TokenRevokeKycTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Revokes the KYC flag to the Hiero account for the given Hiero token. This transaction must be signed by the token's
 * KYC Key. If this key is not set, you can submit a TokenUpdateTransaction to provide the token with this key.
 *
 *  - If the provided account is not found, the transaction will resolve to INVALID_ACCOUNT_ID.
 *  - If the provided account has been deleted, the transaction will resolve to ACCOUNT_DELETED.
 *  - If the provided token is not found, the transaction will resolve to INVALID_TOKEN_ID.
 *  - If the provided token has been deleted, the transaction will resolve to TOKEN_WAS_DELETED.
 *  - If an Association between the provided token and account is not found, the transaction will resolve to
 *    TOKEN_NOT_ASSOCIATED_TO_ACCOUNT.
 *  - If no KYC Key is defined, the transaction will resolve to TOKEN_HAS_NO_KYC_KEY.
 *
 * Once executed, the account is marked as KYC Revoked.
 *
 * Transaction Signing Requirements:
 *  - KYC key.
 *  - Transaction fee payer account key.
 */
class TokenRevokeKycTransaction : public Transaction<TokenRevokeKycTransaction>
{
public:
  TokenRevokeKycTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenRevokeKyc transaction.
   */
  explicit TokenRevokeKycTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenRevokeKycTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the account to have its KYC revoked for this token.
   *
   * @param accountId The ID of the account to have its KYC revoked for this token.
   * @return A reference to this TokenRevokeKycTransaction object with the newly-set account ID.
   * @throws IllegalStateException If this TokenRevokeKycTransaction is frozen.
   */
  TokenRevokeKycTransaction& setAccountId(const AccountId& accountId);

  /**
   * Set the ID of the token for which the account has revoked KYC.
   *
   * @param tokenId The ID of the token for which the account has revoked KYC.
   * @return A reference to this TokenRevokeKycTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenRevokeKycTransaction is frozen.
   */
  TokenRevokeKycTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Get the ID of the account to have its KYC revoked for this token.
   *
   * @return The ID of the account to have its KYC revoked for this token.
   */
  [[nodiscard]] inline AccountId getAccountId() const { return mAccountId; }

  /**
   * Get the ID of the token for which the account has revoked KYC.
   *
   * @return The ID of the token for which the account has revoked KYC.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenRevokeKycTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this TokenRevokeKycTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenRevokeKycTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenRevokeKycTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenRevokeKycTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenRevokeKycTransactionBody protobuf object from this TokenRevokeKycTransaction object.
   *
   * @return A pointer to a TokenRevokeKycTransactionBody protobuf object filled with this TokenRevokeKycTransaction
   *         object's data.
   */
  [[nodiscard]] proto::TokenRevokeKycTransactionBody* build() const;

  /**
   * The ID of the account to have its KYC revoked for this token.
   */
  AccountId mAccountId;

  /**
   * The ID of the token for which the account has revoked KYC.
   */
  TokenId mTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_REVOKE_KYC_TRANSACTION_H_
