// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_MIRROR_NETWORK_H_
#define HIERO_SDK_CPP_IMPL_MIRROR_NETWORK_H_

#include "BaseNetwork.h"
#include "BaseNodeAddress.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Hiero::internal
{
class MirrorNode;
}

namespace Hiero::internal
{
class MirrorNetwork : public BaseNetwork<MirrorNetwork, BaseNodeAddress, MirrorNode>
{
public:
  /**
   * Construct a MirrorNetwork that is pre-configured for Hiero Mainnet access.
   *
   * @return A MirrorNetwork object that is set-up to communicate with the Hiero Mainnet.
   */
  [[nodiscard]] static MirrorNetwork forMainnet();

  /**
   * Construct a MirrorNetwork that is pre-configured for Hiero Testnet access.
   *
   * @return A MirrorNetwork object that is set-up to communicate with the Hiero Testnet.
   */
  [[nodiscard]] static MirrorNetwork forTestnet();

  /**
   * Construct a MirrorNetwork that is pre-configured for Hiero Previewnet access.
   *
   * @return A MirrorNetwork object that is set-up to communicate with the Hiero Previewnet.
   */
  [[nodiscard]] static MirrorNetwork forPreviewnet();

  /**
   * Construct a MirrorNetwork that is pre-configured for a custom network.
   *
   * @param nodeList The list of MirrorNode addresses for the custom network.
   * @return A MirrorNetwork object that is set-up to communicate with a specific node addresses.
   */
  [[nodiscard]] static MirrorNetwork forNetwork(const std::vector<std::string>& networkMap);

  /**
   * Set the MirrorNodes in the MirrorNetwork. This will put the input addresses into a map of addresses to
   * BaseNodeAddresses, and then feed it to BaseNetwork::setNetwork.
   *
   * @param network The network to set for this MirrorNetwork.
   * @return A reference to this MirrorNetwork object with the newly-set network.
   */
  MirrorNetwork& setNetwork(const std::vector<std::string>& network);

  /**
   * Get the addresses of the MirrorNodes in this MirrorNetwork.
   *
   * @return The addresses of the MirrorNodes in this MirrorNetwork.
   */
  [[nodiscard]] std::vector<std::string> getNetwork() const;

  /**
   * Get a pointer to the next MirrorNode.
   *
   * @return A pointer to the next MirrorNode.
   */
  [[nodiscard]] std::shared_ptr<MirrorNode> getNextMirrorNode() const;

private:
  /**
   * Derived from BaseNetwork. Create a MirrorNode for this MirrorNetwork based on a network entry.
   *
   * @param address The address of the MirrorNode.
   * @param key     The key for the MirrorNode. This is unused.
   * @return A pointer to the created MirrorNode.
   */
  [[nodiscard]] std::shared_ptr<MirrorNode> createNodeFromNetworkEntry(std::string_view address,
                                                                       const BaseNodeAddress& /*key*/) const override;
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_MIRROR_NETWORK_H_
