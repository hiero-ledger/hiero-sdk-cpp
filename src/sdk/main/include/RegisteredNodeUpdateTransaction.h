// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_NODE_UPDATE_TRANSACTION_H_
#define HIERO_SDK_CPP_REGISTERED_NODE_UPDATE_TRANSACTION_H_

#include "RegisteredServiceEndpoint.h"
#include "Transaction.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class TransactionBody;
}

namespace com::hedera::hapi::node::addressbook
{
class RegisteredNodeUpdateTransactionBody;
}

namespace aproto = com::hedera::hapi::node::addressbook;

namespace Hiero
{
class Key;

/**
 * A transaction to update an existing registered node in the network address book.
 *
 * This transaction SHALL modify the identified registered node state as requested. It MUST be signed by the registered
 * node's current admin key. If the admin key is being changed, both the old and new key must sign.
 *
 * When serviceEndpoints is set to a non-empty list, it replaces the existing endpoint list entirely. When left empty
 * (the default), the existing endpoints are unchanged.
 *
 * When description is set, it replaces the existing description. When left null, the existing description is unchanged.
 */
class RegisteredNodeUpdateTransaction : public Transaction<RegisteredNodeUpdateTransaction>
{
public:
  RegisteredNodeUpdateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a RegisteredNodeUpdate transaction.
   */
  explicit RegisteredNodeUpdateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit RegisteredNodeUpdateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Get the registered node identifier to update.
   *
   * @return The registered node ID.
   */
  [[nodiscard]] uint64_t getRegisteredNodeId() const { return mRegisteredNodeId; }

  /**
   * Get the new administrative key.
   *
   * @return A shared pointer to the new admin Key, or nullptr if not set.
   */
  [[nodiscard]] const std::shared_ptr<Key>& getAdminKey() const { return mAdminKey; }

  /**
   * Get the new description. nullopt means the description will not be changed.
   *
   * @return An optional string containing the new description.
   */
  [[nodiscard]] const std::optional<std::string>& getDescription() const { return mDescription; }

  /**
   * Get the new service endpoints list. An empty vector means the endpoints will not be changed.
   *
   * @return A vector of shared pointers to RegisteredServiceEndpoint objects.
   */
  [[nodiscard]] const std::vector<std::shared_ptr<RegisteredServiceEndpoint>>& getServiceEndpoints() const
  {
    return mServiceEndpoints;
  }

  /**
   * Set the registered node identifier to update.
   *
   * @param registeredNodeId The ID of the registered node to update.
   * @return A reference to this RegisteredNodeUpdateTransaction with the newly-set registered node ID.
   */
  RegisteredNodeUpdateTransaction& setRegisteredNodeId(uint64_t registeredNodeId);

  /**
   * Set the new administrative key. Both the current and new admin key must sign this transaction if the key is
   * changed.
   *
   * @param key A shared pointer to the new admin Key.
   * @return A reference to this RegisteredNodeUpdateTransaction with the newly-set admin key.
   */
  RegisteredNodeUpdateTransaction& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set the new description. If set, this replaces the current description.
   *
   * @param description The new description. Must not exceed 100 bytes when encoded as UTF-8.
   * @return A reference to this RegisteredNodeUpdateTransaction with the newly-set description.
   */
  RegisteredNodeUpdateTransaction& setDescription(std::string_view description);

  /**
   * Clear the description field so it will not be updated.
   *
   * @return A reference to this RegisteredNodeUpdateTransaction with the description cleared.
   */
  RegisteredNodeUpdateTransaction& clearDescription();

  /**
   * Add a service endpoint to the new endpoints list. If any endpoints are added, the entire list will replace the
   * existing endpoints.
   *
   * @param endpoint A shared pointer to a RegisteredServiceEndpoint to add.
   * @return A reference to this RegisteredNodeUpdateTransaction with the newly-added service endpoint.
   */
  RegisteredNodeUpdateTransaction& addServiceEndpoint(std::shared_ptr<RegisteredServiceEndpoint> endpoint);

  /**
   * Clear all service endpoints from the update. The existing endpoints will remain unchanged.
   *
   * @return A reference to this RegisteredNodeUpdateTransaction with all service endpoints cleared.
   */
  RegisteredNodeUpdateTransaction& clearServiceEndpoints();

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this
   * RegisteredNodeUpdateTransaction's data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this RegisteredNodeUpdateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This RegisteredNodeUpdateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the RegisteredNodeUpdateTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this RegisteredNodeUpdateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a RegisteredNodeUpdateTransactionBody protobuf object from this RegisteredNodeUpdateTransaction object.
   *
   * @return A pointer to a RegisteredNodeUpdateTransactionBody protobuf object filled with this
   *         RegisteredNodeUpdateTransaction object's data.
   */
  [[nodiscard]] aproto::RegisteredNodeUpdateTransactionBody* build() const;

  /**
   * The identifier of the registered node to update.
   */
  uint64_t mRegisteredNodeId = 0;

  /**
   * The new administrative key. If nullptr, the admin key will not be changed.
   */
  std::shared_ptr<Key> mAdminKey;

  /**
   * The new description. nullopt means the description will not be changed.
   */
  std::optional<std::string> mDescription;

  /**
   * The new service endpoints list. An empty vector means the endpoints will not be changed. A non-empty vector
   * replaces the existing endpoints entirely.
   */
  std::vector<std::shared_ptr<RegisteredServiceEndpoint>> mServiceEndpoints;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_NODE_UPDATE_TRANSACTION_H_
