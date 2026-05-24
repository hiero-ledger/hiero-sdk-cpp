// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_NODE_CREATE_TRANSACTION_H_
#define HIERO_SDK_CPP_REGISTERED_NODE_CREATE_TRANSACTION_H_

#include "RegisteredServiceEndpoint.h"
#include "Transaction.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class TransactionBody;
}

namespace com::hedera::hapi::node::addressbook
{
class RegisteredNodeCreateTransactionBody;
}

namespace Hiero
{
class Key;

/**
 * A transaction to create a new registered node in the network address book.
 *
 * This transaction SHALL create a new registered node entry in the network state. The new registered node SHALL be
 * visible and discoverable upon completion of this transaction.
 *
 * The admin key MUST sign this transaction. Upon success, the network assigns a unique registeredNodeId which is
 * returned in the TransactionReceipt.
 */
class RegisteredNodeCreateTransaction : public Transaction<RegisteredNodeCreateTransaction>
{
public:
  RegisteredNodeCreateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a RegisteredNodeCreate transaction.
   */
  explicit RegisteredNodeCreateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit RegisteredNodeCreateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Get the administrative key controlled by the node operator.
   *
   * @return A shared pointer to the administrative Key.
   */
  [[nodiscard]] const std::shared_ptr<Key>& getAdminKey() const { return mAdminKey; }

  /**
   * Get the description of the node.
   *
   * @return The description string.
   */
  [[nodiscard]] const std::string& getDescription() const { return mDescription; }

  /**
   * Get the list of service endpoints for this registered node.
   *
   * @return A vector of shared pointers to RegisteredServiceEndpoint objects.
   */
  [[nodiscard]] const std::vector<std::shared_ptr<RegisteredServiceEndpoint>>& getServiceEndpoints() const
  {
    return mServiceEndpoints;
  }

  /**
   * Set the administrative key controlled by the node operator. This key MUST sign this transaction.
   *
   * @param key A shared pointer to the Key to set as the admin key.
   * @return A reference to this RegisteredNodeCreateTransaction with the newly-set admin key.
   */
  RegisteredNodeCreateTransaction& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set the description of the node.
   *
   * @param description The description to set. Must not exceed 100 bytes when encoded as UTF-8.
   * @return A reference to this RegisteredNodeCreateTransaction with the newly-set description.
   */
  RegisteredNodeCreateTransaction& setDescription(std::string_view description);

  /**
   * Add a service endpoint to this registered node.
   *
   * @param endpoint A shared pointer to a RegisteredServiceEndpoint to add.
   * @return A reference to this RegisteredNodeCreateTransaction with the newly-added service endpoint.
   */
  RegisteredNodeCreateTransaction& addServiceEndpoint(std::shared_ptr<RegisteredServiceEndpoint> endpoint);

  /**
   * Clear all service endpoints from this registered node.
   *
   * @return A reference to this RegisteredNodeCreateTransaction with all service endpoints cleared.
   */
  RegisteredNodeCreateTransaction& clearServiceEndpoints();

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this
   * RegisteredNodeCreateTransaction's data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this RegisteredNodeCreateTransaction are valid.
   * This transaction has no entity IDs to validate.
   */
  void validateChecksums(const Client& /*client*/) const override{};

  /**
   * Derived from Transaction. Build and add the RegisteredNodeCreateTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this RegisteredNodeCreateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a RegisteredNodeCreateTransactionBody protobuf object from this RegisteredNodeCreateTransaction object.
   *
   * @return A pointer to a RegisteredNodeCreateTransactionBody protobuf object filled with this
   *         RegisteredNodeCreateTransaction object's data.
   */
  [[nodiscard]] com::hedera::hapi::node::addressbook::RegisteredNodeCreateTransactionBody* build() const;

  /**
   * The administrative key controlled by the node operator.
   * This key MUST sign this transaction.
   * This key MUST sign each transaction to update this node.
   * This field is REQUIRED and MUST NOT be set to an empty KeyList.
   */
  std::shared_ptr<Key> mAdminKey;

  /**
   * A short description of the node.
   * This value, if set, MUST NOT exceed 100 bytes when encoded as UTF-8.
   * This field is OPTIONAL.
   */
  std::string mDescription;

  /**
   * A list of service endpoints for client calls.
   * This list MUST NOT be empty.
   * This list MUST NOT contain more than 50 entries.
   */
  std::vector<std::shared_ptr<RegisteredServiceEndpoint>> mServiceEndpoints;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_NODE_CREATE_TRANSACTION_H_
