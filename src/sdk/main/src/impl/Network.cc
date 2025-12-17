// SPDX-License-Identifier: Apache-2.0
#include "impl/Network.h"
#include "AccountId.h"
#include "Endpoint.h"
#include "NodeAddress.h"
#include "NodeAddressBook.h"
#include "impl/Node.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>

namespace Hiero::internal
{
namespace
{
/**
 * Map Kubernetes service DNS names to correct ports for local development port-forwarding.
 * This handles scenarios where network-node2 is port-forwarded to 51211 while network-node1 stays at 50211.
 * The DNS names resolve to 127.0.0.1 via /etc/hosts entries.
 *
 * @param endpoint The endpoint address to potentially map
 * @return The mapped endpoint with corrected port if local development, otherwise the original endpoint unchanged
 *
 * @note This function ONLY maps specific local development Kubernetes service DNS names.
 *       All other addresses (testnet, mainnet, previewnet, custom networks) pass through unchanged.
 */
std::string mapEndpointForLocalDevelopment(const std::string& endpoint)
{
  // Map network-node2 from port 50211 to 51211 for local port-forwarding
  if (endpoint.find("network-node2-svc.solo.svc.cluster.local:50211") != std::string::npos)
  {
    return "network-node2-svc.solo.svc.cluster.local:51211";
  }

  // All other addresses (including network-node1) pass through unchanged
  return endpoint;
}
} // anonymous namespace

//-----
Network Network::forMainnet()
{
  return getNetworkForLedgerId(LedgerId::MAINNET);
}

//-----
Network Network::forTestnet()
{
  return getNetworkForLedgerId(LedgerId::TESTNET);
}

//-----
Network Network::forPreviewnet()
{
  return getNetworkForLedgerId(LedgerId::PREVIEWNET);
}

//-----
Network Network::forNetwork(const std::unordered_map<std::string, AccountId>& network)
{
  return Network(network);
}

//-----
std::unordered_map<std::string, AccountId> Network::getNetworkFromAddressBook(const NodeAddressBook& addressBook,
                                                                              unsigned int port)
{
  std::unordered_map<std::string, AccountId> network;
  for (const auto& nodeAddress : addressBook.getNodeAddresses())
  {
    for (const auto& endpoint : nodeAddress.getEndpoints())
    {
      if (endpoint.getPort() == port)
      {
        // Map the endpoint for local development (e.g., Kubernetes service names to localhost)
        const std::string mappedEndpoint = mapEndpointForLocalDevelopment(endpoint.toString());
        network[mappedEndpoint] = nodeAddress.getAccountId();
      }
    }
  }

  return network;
}

//-----
Network& Network::updateNodeAccountIds(const NodeAddressBook& addressBook, unsigned int port)
{
  std::unique_lock lock(*getLock());

  try
  {
    // Build a map of full addresses (with port) to new AccountIds from the address book
    std::unordered_map<std::string, AccountId> addressToAccountId;

    for (const auto& nodeAddress : addressBook.getNodeAddresses())
    {
      for (const auto& endpoint : nodeAddress.getEndpoints())
      {
        if (endpoint.getPort() == port)
        {
          // Map the endpoint for local development (e.g., Kubernetes service names to localhost)
          const std::string mappedEndpoint = mapEndpointForLocalDevelopment(endpoint.toString());
          addressToAccountId[mappedEndpoint] = nodeAddress.getAccountId();
        }
      }
    }

    // Build a new network map with updated AccountIds
    std::unordered_map<AccountId, std::unordered_set<std::shared_ptr<Node>>> newNetworkMap;

    // Update each node's AccountId and add it to the new network map
    for (const auto& node : getNodes())
    {
      const std::string nodeAddress = node->getAddress().toString();
      const AccountId currentAccountId = node->getAccountId();

      // Check if this node's full address is in the address book
      auto it = addressToAccountId.find(nodeAddress);
      if (it != addressToAccountId.end())
      {
        const AccountId& newAccountId = it->second;

        if (!(currentAccountId == newAccountId))
        {
          // Update the node's AccountId in place
          node->setAccountId(newAccountId);
        }

        // Add to new network map with updated AccountId
        newNetworkMap[newAccountId].insert(node);
      }
      else
      {
        // Node not in address book, keep it with current AccountId
        newNetworkMap[currentAccountId].insert(node);
      }
    }

    // Replace the internal network map
    setNetworkInternal(newNetworkMap);
  }
  catch (const std::exception& e)
  {
    // Log and rethrow with a clearer message
    throw std::runtime_error(std::string("Error updating node account IDs: ") + e.what());
  }
  catch (...)
  {
    throw std::runtime_error("Unknown error updating node account IDs");
  }

  return *this;
}

//-----
Network& Network::setLedgerId(const LedgerId& ledgerId)
{
  return setLedgerIdInternal(ledgerId, getAddressBookForLedgerId(ledgerId));
}

//-----
Network& Network::setVerifyCertificates(bool verify)
{
  std::unique_lock lock(*getLock());
  mVerifyCertificates = verify;

  // Set the new certificate verification policy for all Nodes on this Network.
  std::for_each(getNodes().cbegin(),
                getNodes().cend(),
                [&verify](const std::shared_ptr<Node>& node) { node->setVerifyCertificates(verify); });

  return *this;
}

//-----
Network& Network::setMaxNodesPerRequest(unsigned int max)
{
  std::unique_lock lock(*getLock());
  mMaxNodesPerRequest = max;
  return *this;
}

//-----
unsigned int Network::getNumberOfNodesForRequest() const
{
  if (mMaxNodesPerRequest > 0)
  {
    return mMaxNodesPerRequest;
  }

  return (getNetworkInternal().size() + 3 - 1) / 3;
}

//-----
Network& Network::setTransportSecurity(TLSBehavior tls)
{
  std::unique_lock lock(*getLock());
  if (isTransportSecurity() != tls)
  {
    for (const std::shared_ptr<Node>& node : getNodes())
    {
      switch (tls)
      {
        case TLSBehavior::REQUIRE:
        {
          node->toSecure();
          break;
        }
        case TLSBehavior::DISABLE:
        {
          node->toInsecure();
          break;
        }
        default:
        {
          // Unimplemented other TLSBehaviors. Do nothing for now.
        }
      }
    }

    BaseNetwork<Network, AccountId, Node>::setTransportSecurityInternal(tls);
  }

  return *this;
}

//-----
std::vector<AccountId> Network::getNodeAccountIdsForExecute(unsigned int maxAttempts)
{
  std::unique_lock lock(*getLock());

  // Get up to maxAttempts number of most healthy nodes
  const std::vector<std::shared_ptr<Node>> nodes =
    getNumberOfMostHealthyNodes(std::min(maxAttempts, static_cast<unsigned int>(getNodes().size())));

  std::vector<AccountId> accountIds;
  accountIds.reserve(nodes.size());
  std::for_each(nodes.cbegin(),
                nodes.cend(),
                [&accountIds](const std::shared_ptr<Node>& node) { accountIds.push_back(node->getAccountId()); });

  return accountIds;
}

//-----
std::unordered_map<std::string, AccountId> Network::getNetwork() const
{
  std::unique_lock lock(*getLock());
  std::unordered_map<std::string, AccountId> network;
  std::for_each(getNodes().cbegin(),
                getNodes().cend(),
                [&network](const std::shared_ptr<Node>& node)
                { network[node->getAddress().toString()] = node->getAccountId(); });

  return network;
}

//-----
Network::Network(const std::unordered_map<std::string, AccountId>& network)
{
  // Map the network addresses for local development before setting the network
  std::unordered_map<std::string, AccountId> mappedNetwork;
  for (const auto& [address, accountId] : network)
  {
    const std::string mappedAddress = mapEndpointForLocalDevelopment(address);
    mappedNetwork[mappedAddress] = accountId;
  }

  setNetwork(mappedNetwork);
}

//-----
Network Network::getNetworkForLedgerId(const LedgerId& ledgerId)
{
  const NodeAddressBook addressBook = getAddressBookForLedgerId(ledgerId);
  return Network(Network::getNetworkFromAddressBook(addressBook, BaseNodeAddress::PORT_NODE_PLAIN))
    .setLedgerIdInternal(ledgerId, addressBook);
}

//-----
NodeAddressBook Network::getAddressBookForLedgerId(const LedgerId& ledgerId)
{
  // The address book can only be fetched for known Hiero networks.
  if (!ledgerId.isKnownNetwork())
  {
    return {};
  }

  std::string buildPath = std::filesystem::current_path().string() + "/addressbook/" + ledgerId.toString() + ".pb";
  std::ifstream infile(buildPath, std::ios_base::binary);
  return NodeAddressBook::fromBytes({ std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>() });
}

//-----
std::shared_ptr<Node> Network::createNodeFromNetworkEntry(std::string_view address, const AccountId& key) const
{
  auto node = std::make_shared<Node>(key, address);
  node->setVerifyCertificates(mVerifyCertificates);
  return node;
}

//-----
Network& Network::setLedgerIdInternal(const LedgerId& ledgerId, const NodeAddressBook& addressBook)
{
  // Set the ledger ID. Don't lock here because setLedgerId locks.
  BaseNetwork<Network, AccountId, Node>::setLedgerId(ledgerId);

  // Reset the node certificate hash of each node if the address book is empty.
  std::unique_lock lock(*getLock());
  if (addressBook.getNodeAddresses().empty())
  {
    std::for_each(getNodes().cbegin(),
                  getNodes().cend(),
                  [](const std::shared_ptr<Node>& node) { node->setNodeCertificateHash({}); });
  }
  else
  {
    std::for_each(getNodes().cbegin(),
                  getNodes().cend(),
                  [&addressBook](const std::shared_ptr<Node>& node)
                  {
                    for (const NodeAddress& address : addressBook.getNodeAddresses())
                    {
                      if (node->getAccountId() == address.getAccountId())
                      {
                        node->setNodeCertificateHash(address.getCertHash());
                      }
                    }
                  });
  }

  return *this;
}

} // namespace Hiero::internal