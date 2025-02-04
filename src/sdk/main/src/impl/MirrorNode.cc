// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNode.h"
#include "impl/BaseNodeAddress.h"

namespace Hiero::internal
{
//-----
MirrorNode::MirrorNode(const BaseNodeAddress& address)
  : BaseNode<MirrorNode, BaseNodeAddress>(address)
{
}

//-----
MirrorNode::MirrorNode(std::string_view address)
  : BaseNode<MirrorNode, BaseNodeAddress>(BaseNodeAddress::fromString(address))
{
}

//-----
void MirrorNode::initializeStubs()
{
  if (!mConsensusStub)
  {
    mConsensusStub = com::hedera::mirror::api::proto::ConsensusService::NewStub(getChannel());
  }

  if (!mNetworkStub)
  {
    mNetworkStub = com::hedera::mirror::api::proto::NetworkService::NewStub(getChannel());
  }
}

//-----
void MirrorNode::closeStubs()
{
  mConsensusStub = nullptr;
  mNetworkStub = nullptr;
}

} // namespace Hiero::internal
