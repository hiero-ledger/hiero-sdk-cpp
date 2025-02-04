// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_BASE_NETWORK_H_
#define HIERO_SDK_CPP_IMPL_BASE_NETWORK_H_

#include "AccountId.h"
#include "BaseNode.h"
#include "Defaults.h"
#include "LedgerId.h"
#include "TLSBehavior.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace Hiero::internal
{
template<typename NetworkType, typename KeyType, typename NodeType>
class BaseNetwork
{
public:
  /**
   * Set the NodeTypes in this BaseNetwork. It shutdowns and removes any NodeType from this BaseNetwork if the input
   * network doesn't contain its representation. It will then add any new NodeTypes that don't already exist in this
   * BaseNetwork.
   *
   * @param network The map of addresses to their keys that represent NodeTypes to add to this BaseNetwork.
   * @return A reference to this derived BaseNetwork object with the newly-set NodeTypes.
   */
  NetworkType& setNetwork(const std::unordered_map<std::string, KeyType>& network);

  /**
   * Increase the backoff of the input NodeType.
   *
   * @param node The NodeType of which to increase the backoff.
   */
  void increaseBackoff(const std::shared_ptr<NodeType>& node);

  /**
   * Decrease the backoff of the input NodeType.
   *
   * @param node The NodeType of which to decrease the backoff.
   */
  void decreaseBackoff(const std::shared_ptr<NodeType>& node) const;

  /**
   * Get the list of all proxies at a specified key.
   *
   * @return The list of all proxies at a specified key.
   */
  [[nodiscard]] std::vector<std::shared_ptr<NodeType>> getNodeProxies(const KeyType& key);

  /**
   * Close the connects on this BaseNetwork.
   */
  void close() const;

  /**
   * Set the maximum number of times to try to use a NodeType to submit a request. Once the NodeType exceeds this number
   * of submission attempts that have returned a bad gRPC status, that NodeType will be permanently removed from this
   * BaseNetwork.
   *
   * @param attempts The maximum number of times a NodeType can be retried.
   * @return A reference to this derived BaseNetwork object with the newly-set maximum node attempts.
   */
  NetworkType& setMaxNodeAttempts(unsigned int attempts);

  /**
   * Set the minimum amount of time to backoff from a NodeType after a bad gRPC status is received.
   *
   * @param backoff The minimum backoff time to use for a NodeType after a bad gRPC status is received.
   * @return A reference to this derived BaseNetwork object with the newly-set minimum backoff.
   */
  NetworkType& setMinNodeBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Set the maximum amount of time to backoff from a NodeType after a bad gRPC status is received.
   *
   * @param backoff The maximum backoff time to use for a NodeType after a bad gRPC status is received.
   * @return A reference to this derived BaseNetwork object with the newly-set maximum backoff.
   */
  NetworkType& setMaxNodeBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Set the minimum amount of time to wait before readmitting NodeTypes as "healthy".
   *
   * @param time The minimum amount of time to wait before readmitting NodeTypes as "healthy".
   * @return A reference to this derived BaseNetwork object with the newly-set minimum node readmit time.
   */
  NetworkType& setMinNodeReadmitTime(const std::chrono::system_clock::duration& time);

  /**
   * Set the maximum amount of time to wait before readmitting NodeTypes as "healthy".
   *
   * @param time The maximum amount of time to wait before readmitting NodeTypes as "healthy".
   * @return A reference to this derived BaseNetwork object with the newly-set maximum node readmit time.
   */
  NetworkType& setMaxNodeReadmitTime(const std::chrono::system_clock::duration& time);

  /**
   * Set the amount of time to allow gRPC connections on this BaseNetwork to close gracefully before forcibly
   * terminating them.
   *
   * @param timeout The amount of time to allow gRPC connections on this BaseNetwork to close gracefully.
   * @return A reference to this derived BaseNetwork object with the newly-set close timeout.
   */
  NetworkType& setCloseTimeout(const std::chrono::system_clock::duration& timeout);

  /**
   * Set the ledger ID of this BaseNetwork.
   *
   * @param ledgerId The ledger ID to set.
   * @return A reference to this derived BaseNetwork object with the newly-set ledger ID.
   */
  virtual NetworkType& setLedgerId(const LedgerId& ledgerId);

  /**
   * Get the transport security policy of this BaseNetwork.
   *
   * @return The transport security policy of this BaseNetwork.
   */
  [[nodiscard]] inline TLSBehavior isTransportSecurity() const { return mTransportSecurity; }

  /**
   * Get the maximum number of times to try to use a NodeType before removing it from this BaseNetwork.
   *
   * @return The maximum number of times to try to use a NodeType before removing it from this BaseNetwork.
   */
  [[nodiscard]] inline unsigned int getMaxNodeAttempts() const { return mMaxNodeAttempts; }

  /**
   * Get the minimum amount of time to backoff from a NodeType after a bad gRPC status is received.
   *
   * @return The minimum amount of time to backoff from a NodeType after a bad gRPC status is received.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getMinNodeBackoff() const { return mMinNodeBackoff; }

  /**
   * Get the maximum amount of time to backoff from a NodeType after a bad gRPC status is received.
   *
   * @return The maximum amount of time to backoff from a NodeType after a bad gRPC status is received.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getMaxNodeBackoff() const { return mMaxNodeBackoff; }

  /**
   * Get the minimum amount of time to wait before readmitting NodeTypes as "healthy".
   *
   * @return The minimum amount of time to wait before readmitting NodeTypes as "healthy".
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getMinNodeReadmitTime() const { return mMinNodeReadmitTime; }

  /**
   * Get the maximum amount of time to wait before readmitting NodeTypes as "healthy".
   *
   * @return The maximum amount of time to wait before readmitting NodeTypes as "healthy".
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getMaxNodeReadmitTime() const { return mMaxNodeReadmitTime; }

  /**
   * Get the amount of time to allow gRPC connections on this BaseNetwork to close gracefully before forcibly
   * terminating them.
   *
   * @return The amount of time to allow gRPC connections on this BaseNetwork to close gracefully before forcibly
   *         terminating them.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getCloseTimeout() const { return mCloseTimeout; }

  /**
   * Get the ledger ID of this Network.
   *
   * @return The ledger ID of this Network.
   */
  [[nodiscard]] inline LedgerId getLedgerId() const { return mLedgerId; }

protected:
  ~BaseNetwork() = default;

  /**
   * Get a number of the most healthy nodes on this BaseNetwork. "Healthy"-ness is determined by sort order; the lower
   * index nodes in the returned vector are considered the most healthy.
   *
   * This will also remove any nodes which have hit or exceeded mMaxNodeAttempts permanently from the BaseNetwork.
   *
   * @param count The number of nodes to get.
   * @return A list of pointers to the healthiest BaseNodes on this BaseNetwork.
   */
  [[nodiscard]] std::vector<std::shared_ptr<NodeType>> getNumberOfMostHealthyNodes(unsigned int count);

  /**
   * Set the transport security policy.
   *
   * @param tls The transport security policy to set.
   */
  void setTransportSecurityInternal(TLSBehavior tls);

  /**
   * Get the map of KeyTypes to their NodeTypes on this BaseNetwork.
   *
   * @return The map of KeyTypes to their NodeTypes on this BaseNetwork.
   */
  [[nodiscard]] inline const std::unordered_map<KeyType, std::unordered_set<std::shared_ptr<NodeType>>>&
  getNetworkInternal() const
  {
    return mNetwork;
  }
  [[nodiscard]] inline std::unordered_map<KeyType, std::unordered_set<std::shared_ptr<NodeType>>> getNetworkInternal()
  {
    return mNetwork;
  }

  /**
   * Get the list of NodeTypes on this BaseNetwork.
   *
   * @return The list of NodeTypes on this BaseNetwork.
   */
  [[nodiscard]] inline const std::unordered_set<std::shared_ptr<NodeType>>& getNodes() const { return mNodes; }
  [[nodiscard]] inline std::unordered_set<std::shared_ptr<NodeType>>& getNodes() { return mNodes; }

  /**
   * Get this BaseNetwork's mutex.
   *
   * @return This BaseNetwork's mutex.
   */
  [[nodiscard]] inline std::shared_ptr<std::mutex> getLock() const { return mMutex; }

private:
  /**
   * Create a NodeType for this BaseNetwork based on a network entry.
   *
   * @param address The address of the NodeType.
   * @param key     The key for the NodeType.
   * @return A pointer to the created NodeType.
   */
  [[nodiscard]] virtual std::shared_ptr<NodeType> createNodeFromNetworkEntry(
    std::string_view address,
    [[maybe_unused]] const KeyType& key) const = 0;

  /**
   * Readmit nodes from the mNodes list to the mHealthyNodes list when the time has passed the mEarliestReadmitTime.
   * While readmitting nodes, mEarliestReadmitTime will be updated to a new value. This value is either the value of
   * the node with the smallest readmission time from now, or mMinNodeReadmitTime or mMaxNodeReadmitTime.
   */
  void readmitNodes();

  /**
   * Remove a BaseNode from this BaseNetwork.
   *
   * @param node A pointer to the BaseNode to remove.
   */
  void removeNodeFromNetwork(const std::shared_ptr<NodeType>& node);

  /**
   * Map of node identifiers (KeyTypes) to their NodeTypes.
   */
  std::unordered_map<KeyType, std::unordered_set<std::shared_ptr<NodeType>>> mNetwork;

  /**
   * The list of all nodes on this BaseNetwork.
   */
  std::unordered_set<std::shared_ptr<NodeType>> mNodes;

  /**
   * The list of current healthy nodes on this BaseNetwork.
   */
  std::unordered_set<std::shared_ptr<NodeType>> mHealthyNodes;

  /**
   * The transport security policy of this BaseNetwork.
   */
  TLSBehavior mTransportSecurity = TLSBehavior::REQUIRE;

  /**
   * The maximum number of times to try to use a NodeType to submit a request. Once the NodeType exceeds this number of
   * submission attempts that have returned a bad gRPC status, that NodeType will be permanently removed from this
   * BaseNetwork.
   */
  unsigned int mMaxNodeAttempts = DEFAULT_MAX_NODE_ATTEMPTS;

  /**
   * The minimum amount of time to wait to use a NodeType after it has received a bad gRPC status.
   */
  std::chrono::system_clock::duration mMinNodeBackoff = DEFAULT_MIN_NODE_BACKOFF;

  /**
   * The maximum amount of time to wait to use a NodeType after it has received a bad gRPC status.
   */
  std::chrono::system_clock::duration mMaxNodeBackoff = DEFAULT_MAX_NODE_BACKOFF;

  /**
   * The minimum time to wait before attempting to readmit nodes.
   */
  std::chrono::system_clock::duration mMinNodeReadmitTime = DEFAULT_MIN_NODE_BACKOFF;

  /**
   * The maximum time to wait before attempting to readmit nodes.
   */
  std::chrono::system_clock::duration mMaxNodeReadmitTime = DEFAULT_MAX_NODE_BACKOFF;

  /**
   * The earliest time that a node should be readmitted.
   */
  std::chrono::system_clock::time_point mEarliestReadmitTime = std::chrono::system_clock::now();

  /**
   * The timeout for closing either a single node when setting a new network, or closing the entire network.
   */
  std::chrono::system_clock::duration mCloseTimeout = DEFAULT_CLOSE_TIMEOUT;

  /**
   * The ledger ID of the network.
   */
  LedgerId mLedgerId;

  /**
   * The mutex for this BaseNetwork, kept inside a std::shared_ptr to keep BaseNetwork copyable/movable.
   */
  std::shared_ptr<std::mutex> mMutex = std::make_shared<std::mutex>();
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_BASE_NETWORK_H_
