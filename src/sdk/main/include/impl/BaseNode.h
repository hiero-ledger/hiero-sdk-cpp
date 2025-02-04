// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_BASE_NODE_H_
#define HIERO_SDK_CPP_IMPL_BASE_NODE_H_

#include <basic_types.pb.h> // This is needed for Windows to build for some reason.

#include "BaseNodeAddress.h"
#include "Defaults.h"

#include <chrono>
#include <grpcpp/channel.h>
#include <grpcpp/security/credentials.h>
#include <mutex>

namespace Hiero::internal
{
template<typename NodeType, typename KeyType>
class BaseNode
{
public:
  /**
   * Get this BaseNode's key.
   *
   * @return This BaseNode's key.
   */
  [[nodiscard]] virtual KeyType getKey() const = 0;

  /**
   * Close this BaseNode's connection to its remote node.
   */
  void close();

  /**
   * Increase the backoff of this BaseNode.
   */
  void increaseBackoff();

  /**
   * Decrease the backoff of this BaseNode.
   */
  void decreaseBackoff();

  /**
   * Is this BaseNode currently considered healthy?
   *
   * @return \c TRUE if this BaseNode is healthy, otherwise \c FALSE.
   */
  [[nodiscard]] bool isHealthy() const;

  /**
   * Has this BaseNode failed to connect to its remote node?
   *
   * @return \c TRUE if this BaseNode has failed to connect to its remote node, otherwise \c FALSE.
   */
  [[nodiscard]] bool channelFailedToConnect();

  /**
   * Get the remaining amount of time this BaseNode has in its backoff.
   *
   * @return The remaining amount of time this BaseNode has in its backoff.
   */
  [[nodiscard]] std::chrono::system_clock::duration getRemainingTimeForBackoff() const;

  /**
   * Set the minimum amount of time for this BaseNode to backoff after a bad gRPC status is received.
   *
   * @param backoff The minimum backoff time for this BaseNode after a bad gRPC status is received.
   * @return A reference to this derived BaseNode object with the newly-set minimum backoff.
   */
  NodeType& setMinNodeBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Set the maximum amount of time for this BaseNode to backoff after a bad gRPC status is received.
   *
   * @param backoff The maximum backoff time for this BaseNode after a bad gRPC status is received.
   * @return A reference to this derived BaseNode object with the newly-set minimum backoff.
   */
  NodeType& setMaxNodeBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Get this BaseNode's BaseNodeAddress.
   *
   * @return address The BaseNodeAddress of this BaseNode.
   */
  [[nodiscard]] inline BaseNodeAddress getAddress() const
  {
    std::unique_lock lock(*mMutex);
    return mAddress;
  }

  /**
   * Get the minimum amount of time for this BaseNode to backoff after a bad gRPC status is received.
   *
   * @return The minimum amount of time for this BaseNode to backoff after a bad gRPC status is received.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getMinNodeBackoff() const
  {
    std::unique_lock lock(*mMutex);
    return mMinNodeBackoff;
  }

  /**
   * Get the maximum amount of time for this BaseNode to backoff after a bad gRPC status is received.
   *
   * @return The maximum amount of time for this BaseNode to backoff after a bad gRPC status is received.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getMaxNodeBackoff() const
  {
    std::unique_lock lock(*mMutex);
    return mMaxNodeBackoff;
  }

  /**
   * Get the number of times this BaseNode has received a bad gRPC status when attempting to submit a request.
   *
   * @return The number of times this BaseNode has received a bad gRPC status.
   */
  [[nodiscard]] inline unsigned int getBadGrpcStatusCount() const
  {
    std::unique_lock lock(*mMutex);
    return mBadGrpcStatusCount;
  }

  /**
   * Get the time at which this BaseNode will be considered "healthy".
   *
   * @return The time at which this BaseNode will be considered "healthy".
   */
  [[nodiscard]] inline std::chrono::system_clock::time_point getReadmitTime() const
  {
    std::unique_lock lock(*mMutex);
    return mReadmitTime;
  }

  /**
   * Get this BaseNode's mutex.
   *
   * @return This BaseNode's mutex.
   */
  [[nodiscard]] inline std::shared_ptr<std::mutex> getLock() const { return mMutex; }

protected:
  ~BaseNode() = default;

  /**
   * Construct with a BaseNodeAddress.
   *
   * @param address The BaseNodeAddress of this BaseNode.
   */
  explicit BaseNode(BaseNodeAddress address);

  /**
   * Set the BaseNodeAddress of this BaseNode. This will also close this BaseNode's current connection.
   *
   * @param address The BaseNodeAddress to set.
   */
  NodeType& setAddress(const BaseNodeAddress& address);

  /**
   * Get this BaseNode's gRPC channel. Creates and initializes a new channel if one isn't already created.
   *
   * @return A pointer to this BaseNode's gRPC channel.
   */
  [[nodiscard]] std::shared_ptr<grpc::Channel> getChannel();

private:
  /**
   * How often to query for the state of the channel when determining if this BaseNode has connected to its remote node.
   */
  static constexpr auto GET_STATE_INTERVAL = std::chrono::milliseconds(50);

  /**
   * How long to try and let the channel connect before calling the connection a failure.
   */
  static constexpr auto GET_STATE_TIMEOUT = std::chrono::seconds(10);

  /**
   * Get the TLS credentials for this BaseNode's gRPC channel.
   *
   * @return A pointer to the TLS credentials for this BaseNode's gRPC channel.
   */
  [[nodiscard]] virtual std::shared_ptr<grpc::ChannelCredentials> getTlsChannelCredentials() const;

  /**
   * Initialize the stubs in this derived BaseNode with this BaseNode's gRPC channel.
   */
  virtual void initializeStubs()
  { // Intentionally unimplemented, derived BaseNodes that don't use stubs require no functionality.
  }

  /**
   * Close the stubs in this derived BaseNode.
   */
  virtual void closeStubs()
  { // Intentionally unimplemented, derived BaseNodes that don't use stubs require no functionality.
  }

  /**
   * Get the authority of this BaseNode.
   *
   * @return The authority of this BaseNode.
   */
  [[nodiscard]] virtual inline std::string getAuthority() const { return "127.0.0.1"; }

  /**
   * Close this BaseNode's channel and any stubs using that channel.
   */
  void closeChannel();

  /**
   * The address of this BaseNode.
   */
  BaseNodeAddress mAddress;

  /**
   * Pointer to the gRPC channel used to communicate with the gRPC server living on the remote node.
   */
  std::shared_ptr<grpc::Channel> mChannel = nullptr;

  /**
   * The minimum amount of time to wait to use this BaseNode after it has received a bad gRPC status.
   */
  std::chrono::system_clock::duration mMinNodeBackoff = DEFAULT_MIN_NODE_BACKOFF;

  /**
   * The maximum amount of time to wait to use this BaseNode after it has received a bad gRPC status.
   */
  std::chrono::system_clock::duration mMaxNodeBackoff = DEFAULT_MAX_NODE_BACKOFF;

  /**
   * The current amount of time to wait to use this BaseNode after it has received a bad gRPC status. This will increase
   * exponentially until mMaxNodeBackoff is reached.
   */
  std::chrono::system_clock::duration mCurrentBackoff = DEFAULT_MIN_NODE_BACKOFF;

  /**
   * The time at which this BaseNode will be considered "healthy".
   */
  std::chrono::system_clock::time_point mReadmitTime = std::chrono::system_clock::now();

  /**
   * The number of times this BaseNode has received a bad gRPC status.
   */
  unsigned int mBadGrpcStatusCount = 0U;

  /**
   * Is the gRPC channel being utilized by this BaseNode to communicate with its remote node initialized?
   */
  bool mIsConnected = false;

  /**
   * The mutex for this BaseNode, kept inside a std::shared_ptr to keep BaseNetwork copyable/movable.
   */
  std::shared_ptr<std::mutex> mMutex = std::make_shared<std::mutex>();
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_BASE_NODE_H_
