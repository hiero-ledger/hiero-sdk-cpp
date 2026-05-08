// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_NODE_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_REGISTERED_NODE_DELETE_TRANSACTION_H_

#include "Transaction.h"

#include <cstdint>

namespace proto
{
class TransactionBody;
}

namespace com::hedera::hapi::node::addressbook
{
class RegisteredNodeDeleteTransactionBody;
}

namespace Hiero
{
/**
 * A transaction to delete a registered node from the network address book.
 *
 * This transaction SHALL remove the identified registered node from the network state. This transaction MUST be signed
 * by the existing entry admin_key or authorized by the Hiero network governance structure.
 */
class RegisteredNodeDeleteTransaction : public Transaction<RegisteredNodeDeleteTransaction>
{
public:
  RegisteredNodeDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a RegisteredNodeDelete transaction.
   */
  explicit RegisteredNodeDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit RegisteredNodeDeleteTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Get the registered node identifier.
   *
   * @return The registered node ID.
   */
  [[nodiscard]] uint64_t getRegisteredNodeId() const { return mRegisteredNodeId; }

  /**
   * Set the registered node identifier.
   *
   * @param registeredNodeId The ID of the registered node to delete.
   * @return A reference to this RegisteredNodeDeleteTransaction with the newly-set registered node ID.
   */
  RegisteredNodeDeleteTransaction& setRegisteredNodeId(uint64_t registeredNodeId);

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this
   * RegisteredNodeDeleteTransaction's data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this RegisteredNodeDeleteTransaction are valid.
   * This transaction has no entity IDs to validate.
   */
  void validateChecksums(const Client& /*client*/) const override{};

  /**
   * Derived from Transaction. Build and add the RegisteredNodeDeleteTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this RegisteredNodeDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a RegisteredNodeDeleteTransactionBody protobuf object from this RegisteredNodeDeleteTransaction object.
   *
   * @return A pointer to a RegisteredNodeDeleteTransactionBody protobuf object filled with this
   *         RegisteredNodeDeleteTransaction object's data.
   */
  [[nodiscard]] com::hedera::hapi::node::addressbook::RegisteredNodeDeleteTransactionBody* build() const;

  /**
   * The identifier of the registered node to delete.
   * The node identified MUST exist in the registered address book.
   * The node identified MUST NOT be deleted.
   * This value is REQUIRED.
   */
  uint64_t mRegisteredNodeId = 0;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_NODE_DELETE_TRANSACTION_H_
