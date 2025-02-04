// SPDX-License-Identifier: Apache-2.0
#include "impl/BaseNode.h"
#include "impl/BaseNodeAddress.h"
#include "impl/HieroCertificateVerifier.h"
#include "impl/MirrorNode.h"
#include "impl/Node.h"

#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <thread>
#include <utility>

namespace Hiero::internal
{
//-----
template<typename NodeType, typename KeyType>
void BaseNode<NodeType, KeyType>::close()
{
  std::unique_lock lock(*mMutex);
  closeChannel();
}

//-----
template<typename NodeType, typename KeyType>
void BaseNode<NodeType, KeyType>::increaseBackoff()
{
  std::unique_lock lock(*mMutex);
  ++mBadGrpcStatusCount;
  mReadmitTime = std::chrono::system_clock::now() + mCurrentBackoff;
  mCurrentBackoff *= 2.0;

  // Make sure the current backoff doesn't go over the max backoff.
  if (mCurrentBackoff > mMaxNodeBackoff)
  {
    mCurrentBackoff = mMaxNodeBackoff;
  }
}

//-----
template<typename NodeType, typename KeyType>
void BaseNode<NodeType, KeyType>::decreaseBackoff()
{
  std::unique_lock lock(*mMutex);
  mCurrentBackoff /= 2.0;

  // Make sure the current backoff doesn't go below the min backoff.
  if (mCurrentBackoff < mMinNodeBackoff)
  {
    mCurrentBackoff = mMinNodeBackoff;
  }
}

//-----
template<typename NodeType, typename KeyType>
bool BaseNode<NodeType, KeyType>::isHealthy() const
{
  std::unique_lock lock(*mMutex);
  return mReadmitTime < std::chrono::system_clock::now();
}

//-----
template<typename NodeType, typename KeyType>
bool BaseNode<NodeType, KeyType>::channelFailedToConnect()
{
  std::unique_lock lock(*mMutex);
  if (mIsConnected)
  {
    return false;
  }

  const std::chrono::system_clock::time_point timeoutTime = std::chrono::system_clock::now() + GET_STATE_TIMEOUT;
  mIsConnected = getChannel()->WaitForConnected(timeoutTime);

  return !mIsConnected;
}

//-----
template<typename NodeType, typename KeyType>
std::chrono::system_clock::duration BaseNode<NodeType, KeyType>::getRemainingTimeForBackoff() const
{
  std::unique_lock lock(*mMutex);
  return mReadmitTime - std::chrono::system_clock::now();
}

//-----
template<typename NodeType, typename KeyType>
NodeType& BaseNode<NodeType, KeyType>::setMinNodeBackoff(const std::chrono::system_clock::duration& backoff)
{
  std::unique_lock lock(*mMutex);
  if (mCurrentBackoff == mMinNodeBackoff)
  {
    mCurrentBackoff = backoff;
  }

  mMinNodeBackoff = backoff;
  return static_cast<NodeType&>(*this);
}

//-----
template<typename NodeType, typename KeyType>
NodeType& BaseNode<NodeType, KeyType>::setMaxNodeBackoff(const std::chrono::system_clock::duration& backoff)
{
  std::unique_lock lock(*mMutex);
  mMaxNodeBackoff = backoff;
  return static_cast<NodeType&>(*this);
}

//-----
template<typename NodeType, typename KeyType>
BaseNode<NodeType, KeyType>::BaseNode(BaseNodeAddress address)
  : mAddress(std::move(address))
{
}

//-----
template<typename NodeType, typename KeyType>
NodeType& BaseNode<NodeType, KeyType>::setAddress(const BaseNodeAddress& address)
{
  // Close the connection since the address is changing.
  closeChannel();

  mAddress = address;
  return static_cast<NodeType&>(*this);
}

//-----
template<typename NodeType, typename KeyType>
std::shared_ptr<grpc::Channel> BaseNode<NodeType, KeyType>::getChannel()
{
  if (!mChannel)
  {
    grpc::ChannelArguments channelArguments;
    channelArguments.SetInt(GRPC_ARG_ENABLE_RETRIES, 0);
    channelArguments.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 10000);
    channelArguments.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);

    if (const std::string authority = getAuthority(); !authority.empty())
    {
      channelArguments.SetString(GRPC_ARG_DEFAULT_AUTHORITY, authority);
    }

    mChannel = grpc::CreateCustomChannel(mAddress.toString(),
                                         mAddress.isTransportSecurity() ? getTlsChannelCredentials()
                                                                        : grpc::InsecureChannelCredentials(),
                                         channelArguments);
    initializeStubs();
  }

  return mChannel;
}

//-----
template<typename NodeType, typename KeyType>
std::shared_ptr<grpc::ChannelCredentials> BaseNode<NodeType, KeyType>::getTlsChannelCredentials() const
{
  return grpc::experimental::TlsCredentials(grpc::experimental::TlsChannelCredentialsOptions());
}

//-----
template<typename NodeType, typename KeyType>
void BaseNode<NodeType, KeyType>::closeChannel()
{
  closeStubs();

  // The connection is closed automatically upon destruction of the channel.
  mChannel = nullptr;
}

/**
 * Explicit template instantiations.
 */
template class BaseNode<Node, AccountId>;
template class BaseNode<MirrorNode, BaseNodeAddress>;

} // namespace Hiero::internal
