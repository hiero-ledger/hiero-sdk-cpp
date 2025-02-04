// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_MIRROR_NODE_H_
#define HIERO_SDK_CPP_IMPL_MIRROR_NODE_H_

#include <mirror/consensus_service.grpc.pb.h>
#include <mirror/mirror_network_service.grpc.pb.h>

#include "BaseNode.h"

#include <memory>
#include <string_view>

namespace Hiero::internal
{
class BaseNodeAddress;
}

namespace Hiero::internal
{
/**
 * Internal utility class used to represent a mirror node on a Hiero network.
 */
class MirrorNode : public BaseNode<MirrorNode, BaseNodeAddress>
{
public:
  /**
   * Construct with the address of the MirrorNode.
   *
   * @param address The address of the MirrorNode.
   */
  explicit MirrorNode(const BaseNodeAddress& address);

  /**
   * Construct with the address of the MirrorNode as a string.
   *
   * @param address The address of the MirrorNode as a string.
   */
  explicit MirrorNode(std::string_view address);

  /**
   * Derived from BaseNode. Get this MirrorNode's key, which is its BaseNodeAddress.
   *
   * @return This MirrorNode's key, which is its BaseNodeAddress.
   */
  [[nodiscard]] inline BaseNodeAddress getKey() const override { return getAddress(); }

  /**
   * Get a pointer to the ConsensusService stub used by this MirrorNode.
   *
   * @return A pointer ot the ConsensusService stub used by this MirrorNode.
   */
  [[nodiscard]] std::shared_ptr<com::hedera::mirror::api::proto::ConsensusService::Stub> getConsensusServiceStub() const
  {
    return mConsensusStub;
  }

  /**
   * Get a pointer to the NetworkService stub used by this MirrorNode.
   *
   * @return A pointer ot the NetworkService stub used by this MirrorNode.
   */
  [[nodiscard]] std::shared_ptr<com::hedera::mirror::api::proto::NetworkService::Stub> getNetworkServiceStub() const
  {
    return mNetworkStub;
  }

private:
  /**
   * Derived from BaseNode. Get the authority of this MirrorNode.
   *
   * @return There is no MirrorNode authority, so returns an empty string;
   */
  [[nodiscard]] inline std::string getAuthority() const override { return {}; }

  /**
   * Derived from BaseNode. Initialize the stubs in this MirrorNode with this MirrorNode's gRPC channel.
   */
  void initializeStubs() override;

  /**
   * Derived from BaseNode. Close the stubs in this MirrorNode.
   */
  void closeStubs() override;

  /**
   * Pointer to the gRPC stub used to communicate with the consensus service living on the remote mirror node.
   */
  std::shared_ptr<com::hedera::mirror::api::proto::ConsensusService::Stub> mConsensusStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the network service living on the remote mirror node.
   */
  std::shared_ptr<com::hedera::mirror::api::proto::NetworkService::Stub> mNetworkStub = nullptr;
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_MIRROR_NODE_H_
