// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_BATCH_TRANSACTION_H_
#define HIERO_SDK_CPP_BATCH_TRANSACTION_H_

#include "Transaction.h"
#include "WrappedTransaction.h"

namespace proto
{
class AtomicBatchTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction body for handling a set of transactions atomically.
 */
class BatchTransaction : public Transaction<BatchTransaction>
{
public:
  BatchTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a Batch transaction.
   */
  explicit BatchTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit BatchTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Append a transaction to the list of transactions this BatchTransaction will execute.
   *
   * @param transaction The transaction to be added
   * @throws IllegalStateException if this transaction is frozen
   * @throws IllegalStateException if the inner transaction is not frozen or missing a batch key
   * @return A reference to this BatchTransaction with the newly-set transaction
   */
  BatchTransaction& addInnerTransaction(const WrappedTransaction& transaction);

  /**
   * Set the list of transactions that this BatchTransaction will execute.
   *
   * @param transactions The list of transactions that this BatchTransaction will execute.
   * @return A reference to this BatchTransaction with the newly-set transactions.
   */
  BatchTransaction& setInnerTransactions(const std::vector<WrappedTransaction>& transactions);

  /**
   * Get the list of transactions that this BatchTransaction will execute.
   *
   * @return The list of transactions that this BatchTransaction will execute.
   */
  [[nodiscard]] inline std::vector<WrappedTransaction> getInnerTransactions() const { return mInnerTransactions; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this BatchTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this BatchTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This BatchTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the BatchTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this BatchTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a AtomicBatchTransactionBody protobuf object from this BatchTransaction object.
   *
   * @return A pointer to a BatchTransactionBody protobuf object filled with this BatchTransaction
   * object's data.
   */
  [[nodiscard]] proto::AtomicBatchTransactionBody* build() const;

  /**
   * A list of transactions that represent the atomic batch transactions.
   */
  std::vector<WrappedTransaction> mInnerTransactions;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_BATCH_TRANSACTION_H_