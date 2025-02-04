// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSACTION_RECORD_QUERY_H_
#define HIERO_SDK_CPP_TRANSACTION_RECORD_QUERY_H_

#include "Query.h"
#include "TransactionId.h"

#include <optional>

namespace Hiero
{
class TransactionRecord;
}

namespace Hiero
{
/**
 * Get the record for a transaction. If the transaction requested a record, then the record lasts for one hour, and a
 * state proof is available for it. If the transaction created an account, file, or smart contract instance, then the
 * record will contain the ID for what it created. If the transaction called a smart contract function, then the record
 * contains the result of that call. If the transaction was a cryptocurrency transfer, then the record includes the
 * TransferList which gives the details of that transfer. If the transaction didn't return anything that should be in
 * the record, then the results field will be set to nothing.
 */
class TransactionRecordQuery : public Query<TransactionRecordQuery, TransactionRecord>
{
public:
  /**
   * Set the ID of the transaction of which to request the record.
   *
   * @param transactionId The ID of the desired transaction of which to request the record.
   * @return A reference to this TransactionRecordQuery object with the newly-set transaction ID.
   */
  TransactionRecordQuery& setTransactionId(const TransactionId& transactionId);

  /**
   * Set the child transaction retrieval policy for this TransactionRecordQuery.
   *
   * @param children \c TRUE if this TransactionRecordQuery should get the receipts of any child transactions,
   *                 otherwise \c FALSE.
   * @return A reference to this TransactionRecordQuery object with the newly-set child transaction retrieval policy.
   */
  TransactionRecordQuery& setIncludeChildren(bool children);

  /**
   * Set the duplicate transaction retrieval policy for this TransactionRecordQuery.
   *
   * @param duplicates \c TRUE if this TransactionRecordQuery should get the receipts of any duplicate transactions,
   *                   otherwise \c FALSE.
   * @return A reference to this TransactionRecordQuery object with the newly-set duplicate transaction retrieval
   *         policy.
   */
  TransactionRecordQuery& setIncludeDuplicates(bool duplicates);

  /**
   * Get the ID of the transaction of which this query is currently configured to get the record.
   *
   * @return The ID of the transaction for which this query is meant. Returns uninitialized if a value has not yet been
   *         set.
   */
  [[nodiscard]] inline std::optional<TransactionId> getTransactionId() const { return mTransactionId; }

  /**
   * Get the child transaction retrieval policy for this TransactionRecordQuery.
   *
   * @return \c TRUE if this TransactionRecordQuery is currently configured to get the records of any child
   *         transactions, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getIncludeChildren() const { return mIncludeChildren; }

  /**
   * Get the duplicate transaction retrieval policy for this TransactionRecordQuery.
   *
   * @return \c TRUE if this TransactionRecordQuery is currently configured to get the records of any duplicate
   *         transactions, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getIncludeDuplicates() const { return mIncludeDuplicates; }

private:
  /**
   * Derived from Executable. Construct a TransactionRecord object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a TransactionRecord object.
   * @return A TransactionRecord object filled with the Response protobuf object's data
   */
  [[nodiscard]] TransactionRecord mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this TransactionRecordQuery's data to a
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
   * Derived from Query. Verify that all the checksums in this TransactionRecordQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TransactionRecordQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Executable. Determine the ExecutionStatus of this TransactionRecordQuery after being submitted.
   *
   * @param status   The response status of the previous attempt.
   * @param client   The Client that attempted to submit this TransactionRecordQuery.
   * @param response The Response protobuf object received from the network in response to submitting this request.
   * @return The status of the submitted request.
   */
  [[nodiscard]]
  typename Executable<TransactionRecordQuery, proto::Query, proto::Response, TransactionRecord>::ExecutionStatus
  determineStatus(Status status,
                  [[maybe_unused]] const Client& client,
                  [[maybe_unused]] const proto::Response& response) override;
  /**
   * Derived from Query. Build a Query protobuf object with this TransactionRecordQuery's data, with the input
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
   * The ID of the transaction of which this query should get the record.
   */
  std::optional<TransactionId> mTransactionId;

  /**
   * Should the records of any children transactions be retrieved as well?
   */
  bool mIncludeChildren = false;

  /**
   * Should the records of any duplicates transactions be retrieved as well?
   */
  bool mIncludeDuplicates = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TRANSACTION_RECORD_QUERY_H_
