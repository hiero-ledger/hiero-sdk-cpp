// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NODE_ADDRESS_H_
#define HIERO_SDK_CPP_NODE_ADDRESS_H_

#include "AccountId.h"
#include "Endpoint.h"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class NodeAddress;
}

namespace Hiero
{
/**
 * Class containing all information related to the address(es) of a node.
 */
class NodeAddress
{
public:
  /**
   * Construct a NodeAddress object from a NodeAddress protobuf object.
   *
   * @param proto The NodeAddress protobuf object from which to construct a NodeAddress object.
   * @return The constructed NodeAddress object.
   */
  [[nodiscard]] static NodeAddress fromProtobuf(const proto::NodeAddress& protoNodeAddress);

  /**
   * Construct a NodeAddress protobuf object from this NodeAddress object.
   *
   * @return A pointer to the created NodeAddress protobuf object filled with this NodeAddress object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::NodeAddress> toProtobuf() const;

  /**
   * Get a string representation of this NodeAddress.
   *
   * @return A string representing this NodeAddress.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Set the public key of this NodeAddress. This should be a hex-encoded string of the public key's DER encoding.
   *
   * @param publicKey The hex-encoded and DER-encoded public key to set.
   * @return A reference to this NodeAddress with the newly-set public key.
   */
  NodeAddress& setPublicKey(std::string_view publicKey);

  /**
   * Set the node ID of this NodeAddress.
   *
   * @param nodeId The node ID to set.
   * @return A reference to this NodeAddress with the newly-set node ID.
   */
  NodeAddress& setNodeId(const int64_t& nodeId);

  /**
   * Set the account ID of this NodeAddress.
   *
   * @param accountId The account ID to set.
   * @return A reference to this NodeAddress with the newly-set account ID.
   */
  NodeAddress& setAccountId(const AccountId& accountId);

  /**
   * Set the certificate hash of this NodeAddress. This should be hex-encoded SHA384 hash of the UTF-8 NFKD encoding of
   * the remote node's TLS cert in PEM format.
   *
   * @param certHash The certificate hash to set.
   * @return A reference to this NodeAddress with the newly-set certificate hash.
   */
  NodeAddress& setCertHash(std::string_view certHash);
  NodeAddress& setCertHash(std::vector<std::byte> certHash);

  /**
   * Set the list of endpoints of this NodeAddress.
   *
   * @param endpoints The list of endpoints to set.
   * @return A reference to this NodeAddress with the newly-set endpoints.
   */
  NodeAddress& setEndpoints(const std::vector<Endpoint>& endpoints);

  /**
   * Set the description text of this NodeAddress.
   *
   * @param description The description text to set.
   * @return A reference to this NodeAddress with the newly-set description.
   */
  NodeAddress& setDescription(std::string_view description);

  /**
   * Get the node ID of this NodeAddress.
   *
   * @return The node ID of this NodeAddress.
   */
  [[nodiscard]] inline int64_t getNodeId() const { return mNodeId; }

  /**
   * Get the public key of this NodeAddress.
   *
   * @return The public key of this NodeAddress.
   */
  [[nodiscard]] inline std::string getPublicKey() const { return mRSAPublicKey; }

  /**
   * Get the account ID of this NodeAddress.
   *
   * @return The account ID of this NodeAddress.
   */
  [[nodiscard]] inline AccountId getAccountId() const { return mNodeAccountId; }

  /**
   * Get the node certificate hash of this NodeAddress.
   *
   * @return The node certificate hash of this NodeAddress.
   */
  [[nodiscard]] inline std::vector<std::byte> getCertHash() const { return mNodeCertHash; }

  /**
   * Get the list of endpoints of this NodeAddress.
   *
   * @return The list of endpoints of this NodeAddress.
   */
  [[nodiscard]] inline const std::vector<Endpoint>& getEndpoints() const { return mEndpoints; }

  /**
   * Get the description text of this NodeAddress.
   *
   * @return The description text of this NodeAddress.
   */
  [[nodiscard]] inline std::string getDescription() const { return mDescription; }

private:
  /**
   * The node's public key.
   */
  std::string mRSAPublicKey;

  /**
   * The ID of the node.
   */
  int64_t mNodeId = -1;

  /**
   * The account ID associated with the node.
   */
  AccountId mNodeAccountId;

  /**
   * The SHA-384 hash of the node's certificate chain.
   */
  std::vector<std::byte> mNodeCertHash;

  /**
   * The Endpoints associated with the node.
   */
  std::vector<Endpoint> mEndpoints;

  /**
   * A string description of the node.
   */
  std::string mDescription;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_NODE_ADDRESS_H_
