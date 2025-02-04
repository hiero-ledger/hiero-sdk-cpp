// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SCHEDULE_CREATE_TRANSACTION_H_
#define HIERO_SDK_CPP_SCHEDULE_CREATE_TRANSACTION_H_

#include "AccountId.h"
#include "Key.h"
#include "Transaction.h"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace proto
{
class SchedulableTransactionBody;
class ScheduleCreateTransactionBody;
class TransactionBody;
class WrappedTransaction;
}

namespace Hiero
{
/**
 * A transaction that creates a schedule entity on a Hiero network. The entity ID for a schedule transaction is
 * referred to as the ScheduleId. After successfully executing a schedule create transaction, you can obtain the
 * ScheduleId by requesting the receipt of the transaction immediately after the transaction was executed. The receipt
 * also contains the scheduled transaction ID. The scheduled transaction ID is used to to request the record of the
 * scheduled transaction if it is successfully executed.
 *
 * If two users submit the same schedule create transaction, the first one to reach consensus will create the schedule
 * ID and the second one will have the schedule ID returned in the receipt of the transaction. The receipt status of the
 * second identical schedule transaction will return an "IDENTICAL_SCHEDULE_ALREADY_CREATED" response from the network.
 * The user who submits the second transaction would need to submit a ScheduleSign transaction to add their signature to
 * the schedule transaction.
 *
 * To retain the ability to delete a schedule transaction, you will need to populate the admin key field when creating a
 * schedule transaction. The admin key will be required to sign the ScheduleDelete transaction to delete the scheduled
 * transaction from the network. If you do not assign an admin key during the creation of the schedule transaction, you
 * will have an immutable schedule transaction.
 *
 * Transaction Signing Requirements:
 *  - The key of the account paying for the creation of the schedule transaction.
 *  - The key of the payer account ID paying for the execution of the scheduled transaction.
 *  - If the payer account is not specified, the operator account will be used to pay for the execution by default.
 *  - The admin key if set.
 *  - You can optionally sign with any of the required signatures for the scheduled (inner) transaction. Freeze the
 *    schedule transaction and call the sign() method to add signatures.
 */
class ScheduleCreateTransaction : public Transaction<ScheduleCreateTransaction>
{
public:
  ScheduleCreateTransaction();
  ~ScheduleCreateTransaction();
  ScheduleCreateTransaction(const ScheduleCreateTransaction&);
  ScheduleCreateTransaction& operator=(const ScheduleCreateTransaction&);
  ScheduleCreateTransaction(ScheduleCreateTransaction&&) noexcept;
  ScheduleCreateTransaction& operator=(ScheduleCreateTransaction&&) noexcept;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ScheduleCreate transaction.
   */
  explicit ScheduleCreateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit ScheduleCreateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the Transaction to schedule.
   *
   * @param transaction The Transaction to schedule.
   * @return A reference to this ScheduleCreateTransaction object with the newly-set scheduled transaction.
   * @throws IllegalStateException  If this ScheduleCreateTransaction is frozen.
   * @throws UninitializedException If the input WrappedTransaction does not contain a Transaction.
   */
  ScheduleCreateTransaction& setScheduledTransaction(const WrappedTransaction& transaction);

  /**
   * Set the memo of the schedule entity.
   *
   * @param memo The memo of the schedule entity.
   * @return A reference to this ScheduleCreateTransaction object with the newly-set memo.
   */
  ScheduleCreateTransaction& setScheduleMemo(std::string_view memo);

  /**
   * Set the admin key of the schedule entity.
   *
   * @param key The admin key of the schedule entity.
   * @return A reference to this ScheduleCreateTransaction object with the newly-set admin key.
   */
  ScheduleCreateTransaction& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set the ID of the account that will pay for the scheduled transaction when it executes. Defaults to the payer of
   * this ScheduleCreateTransaction if not provided.
   *
   * @param accountId The ID of the account that will pay for the scheduled transaction when it executes.
   * @return A reference to this ScheduleCreateTransaction object with the newly-set payer account ID.
   */
  ScheduleCreateTransaction& setPayerAccountId(const AccountId& accountId);

  /**
   * Set the expiration time of the schedule entity.
   *
   * @param time The expiration time of the schedule entity.
   * @return A reference to this ScheduleCreateTransaction object with the newly-set expiration time.
   */
  ScheduleCreateTransaction& setExpirationTime(const std::chrono::system_clock::time_point& time);

  /**
   * Set the schedule entity's wait-for-expiry policy.
   *
   * @param wait \c TRUE if the transaction should be executed at time of expiration, \c FALSE if it should execute when
   *             it receives all signatures.
   * @return A reference to this ScheduleCreateTransaction object with the newly-set expiration time.
   */
  ScheduleCreateTransaction& setWaitForExpiry(bool wait);

  /**
   * Get the Transaction (wrapped) to schedule.
   *
   * @return The Transaction (wrapped) to schedule.
   */
  [[nodiscard]] WrappedTransaction getScheduledTransaction() const;

  /**
   * Get the memo of the schedule entity.
   *
   * @return The memo of the schedule entity.
   */
  [[nodiscard]] std::string getScheduleMemo() const;

  /**
   * Get the admin key of the schedule entity.
   *
   * @return The admin key of the schedule entity.
   */
  [[nodiscard]] std::shared_ptr<Key> getAdminKey() const;

  /**
   * Get the ID of the account that will pay for the scheduled transaction when it executes.
   *
   * @return The ID of the account that will pay for the scheduled transaction when it executes. Returns uninitialized
   *         if no payer account ID has been set.
   */
  [[nodiscard]] std::optional<AccountId> getPayerAccountId() const;

  /**
   * Get the expiration time of the schedule entity.
   *
   * @return The expiration time of the schedule entity. Returns uninitialized if no expiration time has been set.
   */
  [[nodiscard]] std::optional<std::chrono::system_clock::time_point> getExpirationTime() const;

  /**
   * Get the schedule entity's wait-for-expiry policy.
   *
   * @return The schedule entity's wait-for-expiry policy. Returns \c TRUE if the transaction will be executed at time
   *         of expiration, \c FALSE if it will execute when it receives all signatures.
   */
  [[nodiscard]] bool isWaitForExpiry() const;

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this ScheduleCreateTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this ScheduleCreateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ScheduleCreateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the ScheduleCreateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this ScheduleCreateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ScheduleCreateTransactionBody protobuf object from this ScheduleCreateTransaction object.
   *
   * @return A pointer to a ScheduleCreateTransactionBody protobuf object filled with this ScheduleCreateTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ScheduleCreateTransactionBody* build() const;

  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct ScheduleCreateTransactionImpl;
  std::unique_ptr<ScheduleCreateTransactionImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SCHEDULE_CREATE_TRANSACTION_H_
