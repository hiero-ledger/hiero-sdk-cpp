// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SCHEDULE_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_SCHEDULE_DELETE_TRANSACTION_H_

#include "ScheduleId.h"
#include "Transaction.h"

#include <optional>

namespace proto
{
class ScheduleDeleteTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that deletes a scheduled transaction from the network. You can delete a scheduled transaction if only
 * the admin key was set during the creation of the scheduled transaction. If an admin key was not set, the attempted
 * deletion will result in "SCHEDULE_IS_IMMUTABLE" response from the network. Once the scheduled transaction is deleted,
 * the scheduled transaction will be marked as deleted with the consensus timestamp the scheduled transaction was
 * deleted at.
 *
 * Transaction Signing Requirements:
 *  - The admin key of the scheduled transaction.
 */
class ScheduleDeleteTransaction : public Transaction<ScheduleDeleteTransaction>
{
public:
  ScheduleDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ScheduleDelete.
   */
  explicit ScheduleDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit ScheduleDeleteTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the schedule to delete.
   *
   * @param scheduleId The ID of the schedule to delete.
   * @return A reference to this ScheduleDeleteTransaction object with the newly-set schedule ID.
   * @throws IllegalStateException If this ScheduleDeleteTransaction is frozen.
   */
  ScheduleDeleteTransaction& setScheduleId(const ScheduleId& scheduleId);

  /**
   * Get the ID of the schedule this ScheduleDeleteTransaction is currently configured to delete.
   *
   * @return The ID of the schedule this ScheduleDeleteTransaction is currently configured to delete. Uninitialized if
   *         no schedule ID has been set.
   */
  [[nodiscard]] inline std::optional<ScheduleId> getScheduleId() const { return mScheduleId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this ScheduleDeleteTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this ScheduleDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ScheduleDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the ScheduleDeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this ScheduleDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ScheduleDeleteTransactionBody protobuf object from this ScheduleDeleteTransaction object.
   *
   * @return A pointer to a ScheduleDeleteTransactionBody protobuf object filled with this ScheduleDeleteTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ScheduleDeleteTransactionBody* build() const;

  /**
   * The ID of the schedule to delete.
   */
  std::optional<ScheduleId> mScheduleId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SCHEDULE_DELETE_TRANSACTION_H_
