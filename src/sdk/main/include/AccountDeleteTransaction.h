// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_ACCOUNT_DELETE_TRANSACTION_H_

#include "AccountId.h"
#include "Transaction.h"

#include <optional>

namespace proto
{
class CryptoDeleteTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Mark an account as deleted, moving all its current hbars to another account. It will remain in the ledger, marked as
 * deleted, until it expires. Transfers into it a deleted account fail. But a deleted account can still have its
 * expiration extended in the normal way.
 */
class AccountDeleteTransaction : public Transaction<AccountDeleteTransaction>
{
public:
  AccountDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a CryptoDeleteAccount transaction.
   */
  explicit AccountDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit AccountDeleteTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Sets the ID of the account to delete.
   *
   * @param accountId The ID of the account to delete.
   * @return A reference to this AccountDeleteTransaction object with the newly-set account ID.
   * @throws IllegalStateException If this AccountDeleteTransaction is frozen.
   */
  AccountDeleteTransaction& setDeleteAccountId(const AccountId& accountId);

  /**
   * Set the ID of the account to which to transfer the deleted account's remaining Hbars.
   *
   * @param accountId The ID of the account to which to transfer the deleted account's remaining Hbars.
   * @return A reference to this AccountDeleteTransaction object with the newly-set transfer account ID.
   * @throws IllegalStateException If this AccountDeleteTransaction is frozen.
   */
  AccountDeleteTransaction& setTransferAccountId(const AccountId& accountId);

  /**
   * Get the ID of the account this AccountDeleteTransaction is currently configured to delete.
   *
   * @return The ID of the account this AccountDeleteTransaction is currently configured to delete. Uninitialized if no
   *         account ID has been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getDeleteAccountId() const { return mDeleteAccountId; }

  /**
   * Get the ID of the account to which this AccountDeleteTransaction is currently configured to transfer the deleted
   * account's remaining Hbars.
   *
   * @return The ID of the account to which this AccountDeleteTransaction is currently configured to transfer the
   *         deleted account's remaining Hbars. Uninitialized if no account ID has been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getTransferAccountId() const { return mTransferAccountId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this AccountDeleteTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this AccountDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the AccountDeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this AccountDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a CryptoDeleteTransactionBody protobuf object from this AccountDeleteTransaction object.
   *
   * @return A pointer to a CryptoDeleteTransactionBody protobuf object filled with this AccountDeleteTransaction
   *         object's data.
   */
  [[nodiscard]] proto::CryptoDeleteTransactionBody* build() const;

  /**
   * The ID of the account to delete.
   */
  std::optional<AccountId> mDeleteAccountId;

  /**
   * The ID of the account which will receive all the remaining Hbars from the deleted account.
   */
  std::optional<AccountId> mTransferAccountId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_DELETE_TRANSACTION_H_
