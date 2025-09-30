// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LAMBDA_S_STORE_TRANSACTION_H_
#define HIERO_SDK_CPP_LAMBDA_S_STORE_TRANSACTION_H_

#include "Transaction.h"
#include "hooks/HookId.h"
#include "hooks/LambdaStorageUpdate.h"

#include <cstdint>
#include <vector>

namespace proto
{
class TransactionBody;
}

namespace com::hedera::hapi::node::hooks
{
class LambdaSStoreTransactionBody;
}

namespace Hiero
{
/**
 * A transaction that updates the state of an existing file on a Hiero network. Once the transaction has been
 * processed, the network will be updated with the new field values of the file. If you need to access a previous state
 * of the file, you can query a mirror node.
 *
 * Transaction Signing Requirements:
 *  - The key or keys on the file are required to sign this transaction to modify the file properties.
 *  - If you are updating the keys on the file, you must sign with the old key and the new key.
 *  - If you do not sign with the key(s) on the file, you will receive an INVALID_SIGNATURE network error.
 */
class LambdaSStoreTransaction : public Transaction<LambdaSStoreTransaction>
{
public:
  LambdaSStoreTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a LambdaSStore transaction.
   */
  explicit LambdaSStoreTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit LambdaSStoreTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the lambda whose storage is to be updated.
   *
   * @param hookId The ID of the hook to update.
   * @return A reference to this LambdaSStoreTransaction object with the newly-set hook ID.
   * @throws IllegalStateException If this LambdaSStoreTransaction is frozen.
   */
  LambdaSStoreTransaction& setHookId(const HookId& hookId);

  /**
   * Add a new storage update to the hook.
   *
   * @param storageUpdate The new storage update to add.
   * @return A reference to this LambdaSStoreTransaction object with the newly-added storage update.
   * @throws IllegalStateException If this LambdaSStoreTransaction is frozen.
   */
  LambdaSStoreTransaction& addStorageUpdate(const LambdaStorageUpdate& storageUpdate);

  /**
   * Set the new storage updates for the hook.
   *
   * @param storageUpdates The new storage updates for the hook.
   * @return A reference to this LambdaSStoreTransaction object with the newly-set storage updates.
   * @throws IllegalStateException If this LambdaSStoreTransaction is frozen.
   */
  LambdaSStoreTransaction& setStorageUpdates(const std::vector<LambdaStorageUpdate>& storageUpdates);

  /**
   * Clear the storage updates for the hook.
   *
   * @return A reference to this LambdaSStoreTransaction object with now no storage updates.
   * @throws IllegalStateException If this LambdaSStoreTransaction is frozen.
   */
  LambdaSStoreTransaction& clearStorageUpdates();

  /**
   * Get the ID of the hook to update.
   *
   * @return The ID of the hook to update.
   */
  [[nodiscard]] inline HookId getHookId() const { return mHookId; }

  /**
   * Get the list of storage updates for the hook.
   *
   * @return The list of storage updates for the hook.
   */
  [[nodiscard]] inline std::vector<LambdaStorageUpdate> getStorageUpdates() const { return mStorageUpdates; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this LambdaSStoreTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this LambdaSStoreTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This LambdaSStoreTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the LambdaSStoreTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this LambdaSStoreTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a LambdaSStoreTransactionBody protobuf object from this LambdaSStoreTransaction object.
   *
   * @return A pointer to a LambdaSStoreTransactionBody protobuf object filled with this LambdaSStoreTransaction
   *         object's data.
   */
  [[nodiscard]] com::hedera::hapi::node::hooks::LambdaSStoreTransactionBody* build() const;

  /**
   * The ID of the hook to update.
   */
  HookId mHookId;

  /**
   * The storage updates to apply to the hook.
   */
  std::vector<LambdaStorageUpdate> mStorageUpdates;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LAMBDA_S_STORE_TRANSACTION_H_
