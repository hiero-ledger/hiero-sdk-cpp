// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNetwork.h"
#include "impl/MirrorNode.h"
#include "impl/Utilities.h"

namespace Hiero::internal
{
//-----
MirrorNetwork MirrorNetwork::forMainnet()
{
  MirrorNetwork network;
  network.setNetwork({ "mainnet-public.mirrornode.hedera.com:443" });
  return network;
}

//-----
MirrorNetwork MirrorNetwork::forTestnet()
{
  MirrorNetwork network;
  network.setNetwork({ "testnet.mirrornode.hedera.com:443" });
  return network;
}

//-----
MirrorNetwork MirrorNetwork::forPreviewnet()
{
  MirrorNetwork network;
  network.setNetwork({ "previewnet.mirrornode.hedera.com:443" });
  return network;
}

//-----
MirrorNetwork MirrorNetwork::forNetwork(const std::vector<std::string>& networkMap)
{
  MirrorNetwork network;
  network.setNetwork(networkMap);
  return network;
}

//-----
MirrorNetwork& MirrorNetwork::setNetwork(const std::vector<std::string>& network)
{
  std::unordered_map<std::string, BaseNodeAddress> networkMap;
  for (const auto& address : network)
  {
    networkMap.try_emplace(address, BaseNodeAddress::fromString(address));
  }

  BaseNetwork<MirrorNetwork, BaseNodeAddress, MirrorNode>::setNetwork(networkMap);
  return *this;
}

//-----
std::vector<std::string> MirrorNetwork::getNetwork() const
{
  std::unique_lock lock(*getLock());

  std::vector<std::string> network;
  for (const auto& [address, nodes] : BaseNetwork<MirrorNetwork, BaseNodeAddress, MirrorNode>::getNetworkInternal())
  {
    network.push_back(address.toString());
  }

  return network;
}

//-----
std::shared_ptr<MirrorNode> MirrorNetwork::getNextMirrorNode() const
{
  std::unique_lock lock(*getLock());

  if (getNodes().empty())
  {
    return nullptr;
  }

  auto nodeIter = getNodes().cbegin();
  std::advance(nodeIter, Utilities::getRandomNumber(0U, static_cast<unsigned int>(getNodes().size()) - 1U));
  return *nodeIter;
}

//-----
std::shared_ptr<MirrorNode> MirrorNetwork::createNodeFromNetworkEntry(std::string_view address,
                                                                      const BaseNodeAddress&) const
{
  return std::make_shared<MirrorNode>(address);
}

} // namespace Hiero::internal
