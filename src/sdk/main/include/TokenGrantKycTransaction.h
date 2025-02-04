// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_GRANT_KYC_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_GRANT_KYC_TRANSACTION_H_

#include "AccountId.h"
#include "TokenId.h"
#include "Transaction.h"

#include <optional>
#include <vector>

namespace proto
{
class TokenGrantKycTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Grants KYC to the Hiero accounts for the given Hiero token. This transaction must be signed by the token's KYC Key.
 *
 *  - If the provided account is not found, the transaction will resolve to INVALID_ACCOUNT_ID.
 *  - If the provided account has been deleted, the transaction will resolve to ACCOUNT_DELETED.
 *  - If the provided token is not found, the transaction will resolve to INVALID_TOKEN_ID.
 *  - If the provided token has been deleted, the transaction will resolve to TOKEN_WAS_DELETED.
 *  - If an association between the provided token and account is not found, the transaction will resolve to
 *    TOKEN_NOT_ASSOCIATED_TO_ACCOUNT.
 *  - If no KYC Key is defined, the transaction will resolve to TOKEN_HAS_NO_KYC_KEY.
 *
 * Once executed the Account is marked as KYC Granted.
 *
 * Transaction Signing Requirements:
 *  - KYC key.
 *  - Transaction fee payer account key.
 */
class TokenGrantKycTransaction : public Transaction<TokenGrantKycTransaction>
{
public:
  TokenGrantKycTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenGrantKyc transaction.
   */
  explicit TokenGrantKycTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenGrantKycTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the account to have passed KYC for this token.
   *
   * @param accountId The ID of the account to have passed KYC for this token.
   * @return A reference to this TokenGrantKycTransaction object with the newly-set account ID.
   * @throws IllegalStateException If this TokenGrantKycTransaction is frozen.
   */
  TokenGrantKycTransaction& setAccountId(const AccountId& accountId);

  /**
   * Set the ID of the token for which the account has passed KYC.
   *
   * @param tokenId The ID of the token for which the account has passed KYC.
   * @return A reference to this TokenGrantKycTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenGrantKycTransaction is frozen.
   */
  TokenGrantKycTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Get the ID of the account to have passed KYC for this token.
   *
   * @return The ID of the account to have passed KYC for this token.
   */
  [[nodiscard]] inline AccountId getAccountId() const { return mAccountId; }

  /**
   * Get the ID of the token for which the account has passed KYC.
   *
   * @return The ID of the token for which the account has passed KYC.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenGrantKycTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this TokenGrantKycTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenGrantKycTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenGrantKycTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenGrantKycTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenGrantKycTransactionBody protobuf object from this TokenGrantKycTransaction object.
   *
   * @return A pointer to a TokenGrantKycTransactionBody protobuf object filled with this TokenGrantKycTransaction
   *         object's data.
   */
  [[nodiscard]] proto::TokenGrantKycTransactionBody* build() const;

  /**
   * The ID of the account to have passed KYC for this token.
   */
  AccountId mAccountId;

  /**
   * The ID of the token for which the account has passed KYC.
   */
  TokenId mTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_GRANT_KYC_TRANSACTION_H_
