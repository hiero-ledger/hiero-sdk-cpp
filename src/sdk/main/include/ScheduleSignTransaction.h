// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SCHEDULE_SIGN_TRANSACTION_H_
#define HIERO_SDK_CPP_SCHEDULE_SIGN_TRANSACTION_H_

#include "ScheduleId.h"
#include "Transaction.h"

#include <optional>

namespace proto
{
class ScheduleSignTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that appends signatures to a scheduled transaction. You will need to know the schedule ID to reference
 * the scheduled transaction to submit signatures. A record will be generated for each ScheduleSign transaction that is
 * successful and the scheduled entity will subsequently update with the public keys that have signed the scheduled
 * transaction. To view the keys that have signed the scheduled transaction, you can query the network for the schedule
 * info. Once a scheduled transaction receives the last required signature, the scheduled transaction executes.
 *
 * Transaction Signing Requirements:
 *  - The key of the account paying for the transaction.
 */
class ScheduleSignTransaction : public Transaction<ScheduleSignTransaction>
{
public:
  ScheduleSignTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ScheduleSign.
   */
  explicit ScheduleSignTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit ScheduleSignTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the schedule to sign.
   *
   * @param scheduleId The ID of the schedule to sign.
   * @return A reference to this ScheduleSignTransaction object with the newly-set schedule ID.
   * @throws IllegalStateException If this ScheduleSignTransaction is frozen.
   */
  ScheduleSignTransaction& setScheduleId(const ScheduleId& scheduleId);

  /**
   * Clear the currently configured schedule ID.
   *
   * @return A reference to this ScheduleSignTransaction object with the newly-cleared schedule ID.
   * @throws IllegalStateException If this ScheduleSignTransaction is frozen.
   */
  ScheduleSignTransaction& clearScheduleId();

  /**
   * Get the ID of the schedule this ScheduleSignTransaction is currently configured to sign.
   *
   * @return The ID of the schedule this ScheduleSignTransaction is currently configured to sign. Uninitialized if no
   *         schedule ID has been set.
   */
  [[nodiscard]] inline std::optional<ScheduleId> getScheduleId() const { return mScheduleId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this ScheduleSignTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this ScheduleSignTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ScheduleSignTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the ScheduleSignTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this ScheduleSignTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ScheduleSignTransactionBody protobuf object from this ScheduleSignTransaction object.
   *
   * @return A pointer to a ScheduleSignTransactionBody protobuf object filled with this ScheduleSignTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ScheduleSignTransactionBody* build() const;

  /**
   * The ID of the schedule to sign.
   */
  std::optional<ScheduleId> mScheduleId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SCHEDULE_SIGN_TRANSACTION_H_
