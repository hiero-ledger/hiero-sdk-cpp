// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NODE_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_NODE_DELETE_TRANSACTION_H_

#include "Transaction.h"

namespace proto
{
class NodeDeleteTransactionBody;
class TransactionBody;
}

namespace com::hedera::hapi::node::addressbook
{
class NodeDeleteTransactionBody;
}

namespace aproto = com::hedera::hapi::node::addressbook;

namespace Hiero
{
/**
 * A transaction to delete a node from the network address book.
 *
 * This transaction body SHALL be considered a "privileged transaction".
 *
 * - A `NodeDeleteTransactionBody` MUST be signed by the governing council.
 * - Upon success, the address book entry SHALL enter a "pending delete"
 *   state.
 * - All address book entries pending deletion SHALL be removed from the
 *   active network configuration during the next `freeze` transaction with
 *   the field `freeze_type` set to `PREPARE_UPGRADE`.<br/>
 * - A deleted address book node SHALL be removed entirely from network state.
 * - A deleted address book node identifier SHALL NOT be reused.
 *
 * ### Record Stream Effects
 * Upon completion the "deleted" `node_id` SHALL be in the transaction
 * receipt.
 */
class NodeDeleteTransaction : public Transaction<NodeDeleteTransaction>
{
public:
  NodeDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a NodeDelete transaction.
   */
  explicit NodeDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit NodeDeleteTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Get the consensus node identifier in the network state.
   *
   * @return A uint64_t representing the node identifier.
   */
  [[nodiscard]] uint64_t getNodeId() const { return mNodeId; };

  /**
   * Set the consensus node identifier in the network state.
   *
   * @param nodeId A uint64_t representing the node identifier.
   * @return A reference to this NodeDeleteTransaction with the newly-set node identifier.
   */
  NodeDeleteTransaction& setNodeId(uint64_t nodeId);

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this NodeDeleteTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this NodeDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This NodeDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the NodeDeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this NodeDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a NodeDeleteTransactionBody protobuf object from this NodeDeleteTransaction object.
   *
   * @return A pointer to a NodeDeleteTransactionBody protobuf object filled with this NodeDeleteTransaction object's
   *         data.
   */
  [[nodiscard]] aproto::NodeDeleteTransactionBody* build() const;

  /**
   * A consensus node identifier in the network state.
   * The node identified MUST exist in the network address book.
   * The node identified MUST NOT be deleted.
   * This value is REQUIRED.
   */
  uint64_t mNodeId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_NODE_DELETE_TRANSACTION_H_
