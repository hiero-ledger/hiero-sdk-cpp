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
        network[endpoint.toString()] = nodeAddress.getAccountId();
      }
    }
  }

  return network;
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
std::vector<AccountId> Network::getNodeAccountIdsForExecute()
{
  std::unique_lock lock(*getLock());

  // Get either the 1/3 most healthy nodes, or the number of most healthy nodes specified by mMaxNodesPerRequest.
  const std::vector<std::shared_ptr<Node>> nodes = getNumberOfMostHealthyNodes(
    mMaxNodesPerRequest > 0U ? std::min(mMaxNodesPerRequest, static_cast<unsigned int>(getNodes().size()))
                             : static_cast<unsigned int>(std::ceil(static_cast<double>(getNodes().size()) / 3.0)));

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
  setNetwork(network);
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