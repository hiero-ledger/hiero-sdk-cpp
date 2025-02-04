// SPDX-License-Identifier: Apache-2.0
#include "impl/BaseNetwork.h"
#include "AccountId.h"
#include "impl/BaseNodeAddress.h"
#include "impl/MirrorNetwork.h"
#include "impl/MirrorNode.h"
#include "impl/Network.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <algorithm>
#include <thread>

namespace Hiero::internal
{
//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setNetwork(
  const std::unordered_map<std::string, KeyType>& network)
{
  std::unique_lock lock(*mMutex);

  // New containers to hold new network.
  std::unordered_map<KeyType, std::unordered_set<std::shared_ptr<NodeType>>> newNetwork;
  std::unordered_set<std::shared_ptr<NodeType>> newNodes;

  // Go through each entry in the new network list and either move the NodeType that already exists for that entry to
  // the newNodes list, or create a new NodeType for that entry.
  for (const auto& [address, key] : network)
  {
    // Addresses can be added with the same IP addresses, but different ports. Since the different ports just represent
    // a TLS connection or not, they shouldn't be treated as different NodeTypes. Grab just the IP address and use that
    // to compare to the current NodeTypes.
    const std::string ipAddress = BaseNodeAddress::fromString(address).getAddress();

    // Determine if this entry already has a NodeType.
    bool entryAlreadyExists = false;
    for (const auto& node : mNodes)
    {
      if (node->getAddress().getAddress() == ipAddress && node->getKey() == key)
      {
        // Move this node to the newNodes set and go to the next entry.
        const std::shared_ptr<NodeType> extractedNode = mNodes.extract(node).value();
        newNodes.insert(extractedNode);
        newNetwork[key].insert(extractedNode);
        entryAlreadyExists = true;
        break;
      }
    }

    // If the entry wasn't found, verify that the address doesn't match a node that has already been added to newNodes.
    // Addresses can be repeated with different ports, so this makes sure duplicates don't get added.
    if (!entryAlreadyExists)
    {
      for (const std::shared_ptr<NodeType>& node : newNodes)
      {
        if (node->getAddress().getAddress() == ipAddress && node->getKey() == key)
        {
          entryAlreadyExists = true;
          break;
        }
      }
    }

    // If the entry was found, go to the next entry.
    if (entryAlreadyExists)
    {
      continue;
    }

    // If an entry doesn't exist, create one and add it to the nodes lists and network.
    const std::shared_ptr<NodeType> newNode = createNodeFromNetworkEntry(address, key);
    newNodes.insert(newNode);
    newNetwork[key].insert(newNode);
  }

  // The remaining nodes in mNodes need to be closed and removed.
  for (const std::shared_ptr<NodeType>& node : mNodes)
  {
    node->close();
    removeNodeFromNetwork(node);
  }

  // Set the new nodes list and network.
  mNodes = newNodes;
  mNetwork = newNetwork;
  mHealthyNodes.clear();

  // Try to readmit all nodes.
  mEarliestReadmitTime = std::chrono::system_clock::now();
  readmitNodes();

  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
void BaseNetwork<NetworkType, KeyType, NodeType>::increaseBackoff(const std::shared_ptr<NodeType>& node)
{
  std::unique_lock lock(*mMutex);
  node->increaseBackoff();
  mHealthyNodes.erase(node);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
void BaseNetwork<NetworkType, KeyType, NodeType>::decreaseBackoff(const std::shared_ptr<NodeType>& node) const
{
  std::unique_lock lock(*mMutex);
  node->decreaseBackoff();
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
std::vector<std::shared_ptr<NodeType>> BaseNetwork<NetworkType, KeyType, NodeType>::getNodeProxies(const KeyType& key)
{
  std::unique_lock lock(*mMutex);
  readmitNodes();
  return { mNetwork[key].cbegin(), mNetwork[key].cend() };
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
void BaseNetwork<NetworkType, KeyType, NodeType>::close() const
{
  std::unique_lock lock(*mMutex);
  for (const std::shared_ptr<NodeType>& node : mNodes)
  {
    node->close();
  }
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setMaxNodeAttempts(unsigned int attempts)
{
  std::unique_lock lock(*mMutex);
  mMaxNodeAttempts = attempts;
  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setMinNodeBackoff(
  const std::chrono::system_clock::duration& backoff)
{
  std::unique_lock lock(*mMutex);
  mMinNodeBackoff = backoff;
  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setMaxNodeBackoff(
  const std::chrono::system_clock::duration& backoff)
{
  std::unique_lock lock(*mMutex);
  mMaxNodeBackoff = backoff;
  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setMinNodeReadmitTime(
  const std::chrono::system_clock::duration& time)
{
  std::unique_lock lock(*mMutex);
  mMinNodeReadmitTime = time;
  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setMaxNodeReadmitTime(
  const std::chrono::system_clock::duration& time)
{
  std::unique_lock lock(*mMutex);
  mMaxNodeReadmitTime = time;
  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setCloseTimeout(
  const std::chrono::system_clock::duration& timeout)
{
  std::unique_lock lock(*mMutex);
  mCloseTimeout = timeout;
  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
NetworkType& BaseNetwork<NetworkType, KeyType, NodeType>::setLedgerId(const LedgerId& ledgerId)
{
  std::unique_lock lock(*mMutex);
  mLedgerId = ledgerId;
  return static_cast<NetworkType&>(*this);
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
std::vector<std::shared_ptr<NodeType>> BaseNetwork<NetworkType, KeyType, NodeType>::getNumberOfMostHealthyNodes(
  unsigned int count)
{
  // First, remove any nodes from the network that have exceeded the maximum number of node attempts.
  if (mMaxNodeAttempts > 0U)
  {
    auto nodeIter = mNodes.begin();
    while (nodeIter != mNodes.end())
    {
      if ((*nodeIter)->getBadGrpcStatusCount() >= mMaxNodeAttempts)
      {
        (*nodeIter)->close();
        removeNodeFromNetwork(*nodeIter);
      }
      else
      {
        ++nodeIter;
      }
    }
  }

  std::unordered_set<std::shared_ptr<NodeType>> nodes;
  while (count > nodes.size())
  {
    // Readmit nodes each time one is fetched. Since readmitting only happens periodically, calling this each time
    // should not have a significant impact on performance.
    readmitNodes();

    // If there are no healthy nodes, wait until one can be readmitted.
    if (mHealthyNodes.empty())
    {
      std::this_thread::sleep_for(mEarliestReadmitTime - std::chrono::system_clock::now());
      continue;
    }

    // Add a random node (it won't be added if its repeated).
    auto iter = mHealthyNodes.cbegin();
    std::advance(iter, internal::Utilities::getRandomNumber(0U, static_cast<unsigned int>(mHealthyNodes.size()) - 1U));
    nodes.insert(*iter);
  }

  return { nodes.cbegin(), nodes.cend() };
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
void BaseNetwork<NetworkType, KeyType, NodeType>::setTransportSecurityInternal(TLSBehavior tls)
{
  mTransportSecurity = tls;
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
void BaseNetwork<NetworkType, KeyType, NodeType>::readmitNodes()
{
  const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

  // Do nothing if the time for node re-admittance hasn't been reached yet.
  if (now <= mEarliestReadmitTime)
  {
    return;
  }

  // Determine the next earliest readmit time.
  std::chrono::system_clock::time_point nextEarliestReadmitTime = now + mMaxNodeReadmitTime;

  for (const auto& node : mNodes)
  {
    if (node->getReadmitTime() > now && node->getReadmitTime() < nextEarliestReadmitTime)
    {
      nextEarliestReadmitTime = node->getReadmitTime();
    }
  }

  // Set the next earliest readmit time.
  mEarliestReadmitTime = nextEarliestReadmitTime;

  // Readmit nodes as healthy if they're passed their readmit time.
  for (const auto& node : mNodes)
  {
    if (mHealthyNodes.find(node) == mHealthyNodes.end() && node->getReadmitTime() <= now)
    {
      mHealthyNodes.insert(node);
    }
  }
}

//-----
template<typename NetworkType, typename KeyType, typename NodeType>
void BaseNetwork<NetworkType, KeyType, NodeType>::removeNodeFromNetwork(const std::shared_ptr<NodeType>& node)
{
  mNetwork[node->getKey()].erase(node);
  mNodes.erase(node);
  mHealthyNodes.erase(node);
}

/**
 * Explicit template instantiations.
 */
template class BaseNetwork<Network, AccountId, Node>;
template class BaseNetwork<MirrorNetwork, BaseNodeAddress, MirrorNode>;

} // namespace Hiero::internal
