// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_NETWORK_H_
#define HIERO_SDK_CPP_IMPL_NETWORK_H_

#include "BaseNetwork.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Hiero
{
namespace internal
{
class Node;
enum class TLSBehavior;
}

class AccountId;
class LedgerId;
class NodeAddress;
class NodeAddressBook;
}

namespace Hiero::internal
{
class Network : public BaseNetwork<Network, AccountId, Node>
{
public:
  /**
   * Construct a Network that is pre-configured for Hiero Mainnet access.
   *
   * @return A Network object that is set-up to communicate with the Hiero Mainnet.
   */
  [[nodiscard]] static Network forMainnet();

  /**
   * Construct a Network that is pre-configured for Hiero Testnet access.
   *
   * @return A Network object that is set-up to communicate with the Hiero Testnet.
   */
  [[nodiscard]] static Network forTestnet();

  /**
   * Construct a Network that is pre-configured for Hiero Previewnet access.
   *
   * @return A Network object that is set-up to communicate with the Hiero Previewnet.
   */
  [[nodiscard]] static Network forPreviewnet();

  /**
   * Construct a custom Network from a map of node addresses to their corresponding AccountId.
   *
   * @param network The map with string representation of node addresses with their corresponding AccountIds.
   * @return A Network object that is set-up to communicate with the custom network.
   */
  [[nodiscard]] static Network forNetwork(const std::unordered_map<std::string, AccountId>& network);

  /**
   * Construct a network map from a NodeAddressBook with a specific port for the endpoints.
   *
   * @param addressBook The NodeAddressBook from which to construct the network map.
   * @param port        The desired port.
   * @return A network map that contains the nodes in the input NodeAddressBook.
   */
  [[nodiscard]] static std::unordered_map<std::string, AccountId> getNetworkFromAddressBook(
    const NodeAddressBook& addressBook,
    unsigned int port);

  /**
   * Derived from BaseNetwork. Set the ledger ID of this Network.
   *
   * @param ledgerId The ledger ID to set.
   * @return A reference to this Network object with the newly-set ledger ID.
   */
  Network& setLedgerId(const LedgerId& ledgerId) override;

  /**
   * Set this Network's certificate verification policy.
   *
   * @param verify \c TRUE if this Network should verify remote node certificates, otherwise \c FALSE.
   * @return A reference to this Network object with the newly-set certificate verification policy.
   */
  Network& setVerifyCertificates(bool verify);

  /**
   * Set this Network's transport security policy.
   *
   * @param tls The transport security policy to set.
   * @return A reference to this Network object with the newly-set transport security policy.
   * @throws UninitializedException If TLS is required and a Node on this Network wasn't initialized with a certificate
   *                                hash.
   */
  Network& setTransportSecurity(TLSBehavior tls);

  /**
   * Set the maximum number of nodes to be returned for each request.
   *
   * @param max The maximum number of nodes to be returned for each request.
   * @return A reference to this Network object with the newly-set maximum nodes per request.
   */
  Network& setMaxNodesPerRequest(unsigned int max);

  /**
   * Are certificates being verified?
   *
   * @return A boolean value indicating if the certificates are being verified.
   */
  [[nodiscard]] bool isVerifyCertificates() const { return mVerifyCertificates; }

  /**
   * Get a list of node account IDs on which to execute. This will pick 1/3 of the available nodes sorted by health and
   * expected delay from the network.
   *
   * @return A list of AccountIds that are running nodes on which should be executed.
   */
  [[nodiscard]] unsigned int getNumberOfNodesForRequest() const;

  /**
   * Get a list of node account IDs on which to execute. This will pick 1/3 of the available nodes sorted by health and
   * expected delay from the network.
   *
   * @return A list of AccountIds that are running nodes on which should be executed.
   */
  [[nodiscard]] std::vector<AccountId> getNodeAccountIdsForExecute();

  /**
   * Get a map of this Network, mapping the Node addresses to their AccountIds.
   *
   * @return A map of this Network, mapping the Node addresses to their AccountIds.
   */
  [[nodiscard]] std::unordered_map<std::string, AccountId> getNetwork() const;

private:
  /**
   * Construct with a network map.
   *
   * @param network A map of node addresses to the AccountIds that run those nodes.
   */
  explicit Network(const std::unordered_map<std::string, AccountId>& network);

  /**
   * Construct a Network object configured to communicate with the network represented by the input LedgerId.
   *
   * @param ledgerId The LedgerId of the Network object to construct.
   * @return The constructed Network object.
   */
  [[nodiscard]] static Network getNetworkForLedgerId(const LedgerId& ledgerId);

  /**
   * Get a map of AccountIds to NodeAddresses from a local file for a network based on the input LedgerId.
   *
   * @param ledgerId The LedgerId of the network of which to get the address book.
   * @return The map of node addresses and AccountIds of the Nodes that exist on the network represented by the input
   *         LedgerId.
   */
  [[nodiscard]] static NodeAddressBook getAddressBookForLedgerId(const LedgerId& ledgerId);

  /**
   * Derived from BaseNetwork. Create a Node for this Network based on a network entry.
   *
   * @param address The address of the Node.
   * @param key     The key for the Node.
   * @return A pointer to the created Node.
   */
  [[nodiscard]] std::shared_ptr<Node> createNodeFromNetworkEntry(std::string_view address,
                                                                 const AccountId& key) const override;

  /**
   * Set the ledger ID of this Network. In addition, update the Nodes on this Network with their address book entry
   * contained in the input map.
   *
   * @param ledgerId    The new LedgerId of the Network.
   * @param addressBook The NodeAddressBook with which to update this Network's Node's address book entry.
   */
  Network& setLedgerIdInternal(const LedgerId& ledgerId, const NodeAddressBook& addressBook);

  /**
   * The maximum number of nodes to be returned for each request.
   */
  unsigned int mMaxNodesPerRequest = 0U;

  /**
   * Should the Nodes on this Network verify remote node certificates?
   */
  bool mVerifyCertificates = true;
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_NETWORK_H_
