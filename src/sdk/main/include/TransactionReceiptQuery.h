// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSACTION_RECEIPT_QUERY_H_
#define HIERO_SDK_CPP_TRANSACTION_RECEIPT_QUERY_H_

#include "Query.h"
#include "TransactionId.h"

#include <optional>

namespace Hiero
{
class TransactionReceipt;
}

namespace Hiero
{
/**
 * Get the receipt of a transaction, given its transaction ID. Once a transaction reaches consensus, then information
 * about whether it succeeded or failed will be available until the end of the receipt period. Before and after the
 * receipt period, and for a transaction that was never submitted, the receipt is unknown. This query is free (the
 * payment field is left empty). No State proof is available for this response
 */
class TransactionReceiptQuery : public Query<TransactionReceiptQuery, TransactionReceipt>
{
public:
  /**
   * Set the ID of the transaction of which to request the receipt.
   *
   * @param transactionId The ID of the desired transaction of which to request the receipt.
   * @return A reference to this TransactionReceiptQuery object with the newly-set transaction ID.
   */
  TransactionReceiptQuery& setTransactionId(const TransactionId& transactionId);

  /**
   * Set the child transaction retrieval policy for this TransactionReceiptQuery.
   *
   * @param children \c TRUE if this TransactionReceiptQuery should get the receipts of any child transactions,
   *                 otherwise \c FALSE.
   * @return A reference to this TransactionReceiptQuery object with the newly-set child transaction retrieval policy.
   */
  TransactionReceiptQuery& setIncludeChildren(bool children);

  /**
   * Set the duplicate transaction retrieval policy for this TransactionReceiptQuery.
   *
   * @param duplicates \c TRUE if this TransactionReceiptQuery should get the receipts of any duplicate transactions,
   *                   otherwise \c FALSE.
   * @return A reference to this TransactionReceiptQuery object with the newly-set duplicate transaction retrieval
   *         policy.
   */
  TransactionReceiptQuery& setIncludeDuplicates(bool duplicates);

  /**
   * Get the ID of the transaction of which this query is currently configured to get the receipt.
   *
   * @return The ID of the transaction for which this query is meant. Returns uninitialized if a value has not yet been
   *         set.
   */
  [[nodiscard]] inline std::optional<TransactionId> getTransactionId() const { return mTransactionId; }

  /**
   * Get the child transaction retrieval policy for this TransactionReceiptQuery.
   *
   * @return \c TRUE if this TransactionReceiptQuery is currently configured to get the receipts of any child
   *         transactions, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getIncludeChildren() const { return mIncludeChildren; }

  /**
   * Get the duplicate transaction retrieval policy for this TransactionReceiptQuery.
   *
   * @return \c TRUE if this TransactionReceiptQuery is currently configured to get the receipts of any duplicate
   *         transactions, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getIncludeDuplicates() const { return mIncludeDuplicates; }

private:
  /**
   * Derived from Executable. Construct a TransactionReceipt object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a TransactionReceipt object.
   * @return A TransactionReceipt object filled with the Response protobuf object's data
   */
  [[nodiscard]] TransactionReceipt mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this TransactionReceiptQuery's data to a
   * Node.
   *
   * @param request  The Query protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Query& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::Response* response) const override;

  /**
   * Derived from Query. Verify that all the checksums in this TopicUpdateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TopicUpdateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Executable. Determine the ExecutionStatus of this TransactionReceiptQuery after being submitted.
   *
   * @param status   The response status from the network.
   * @param client   The Client that submitted this TransactionReceiptQuery.
   * @param response The Response protobuf object received from the network in response to submitting this
   *                 TransactionReceiptQuery.
   * @return The status of the submitted TransactionReceiptQuery.
   */
  [[nodiscard]]
  typename Executable<TransactionReceiptQuery, proto::Query, proto::Response, TransactionReceipt>::ExecutionStatus
  determineStatus(Status status,
                  [[maybe_unused]] const Client& client,
                  [[maybe_unused]] const proto::Response& response) override;
  /**
   * Derived from Query. Build a Query protobuf object with this TransactionReceiptQuery's data, with the input
   * QueryHeader protobuf object.
   *
   * @param header A pointer to the QueryHeader protobuf object to add to the Query protobuf object.
   * @return The constructed Query protobuf object.
   */
  [[nodiscard]] proto::Query buildRequest(proto::QueryHeader* header) const override;

  /**
   * Derived from Query. Get the ResponseHeader protobuf object from the input Response protobuf object.
   *
   * @param response The Response protobuf object from which to get the ResponseHeader protobuf object.
   * @return The ResponseHeader protobuf object of the input Response protobuf object for this derived Query.
   */
  [[nodiscard]] proto::ResponseHeader mapResponseHeader(const proto::Response& response) const override;

  /**
   * Derived from Query. Does this TransactionReceiptQuery require payment?
   *
   * @return \c FALSE, TransactionReceiptQuery is free.
   */
  [[nodiscard]] inline bool isPaymentRequired() const override { return false; }

  /**
   * The ID of the transaction of which this query should get the receipt.
   */
  std::optional<TransactionId> mTransactionId;

  /**
   * Should the receipts of any children transactions be retrieved as well?
   */
  bool mIncludeChildren = false;

  /**
   * Should the receipts of any duplicates transactions be retrieved as well?
   */
  bool mIncludeDuplicates = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TRANSACTION_RECEIPT_QUERY_H_
