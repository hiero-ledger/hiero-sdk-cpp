// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_BLOCK_NODE_API_H_
#define HIERO_SDK_CPP_BLOCK_NODE_API_H_

#include <string_view>
#include <unordered_map>

namespace com::hedera::hapi::node::addressbook
{
enum RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi : int;
}

namespace Hiero
{
/**
 * An enumeration of well-known block node endpoint APIs.
 */
enum class BlockNodeApi
{
  /**
   * Any other API type associated with a block node. The caller must consult local documentation to determine the
   * correct call semantics.
   */
  OTHER,
  /**
   * The Block Node Status API.
   */
  STATUS,
  /**
   * The Block Node Publish API.
   */
  PUBLISH,
  /**
   * The Block Node Subscribe Stream API.
   */
  SUBSCRIBE_STREAM,
  /**
   * The Block Node State Proof API.
   */
  STATE_PROOF
};

/**
 * Map of protobuf BlockNodeApi values to the corresponding BlockNodeApi.
 */
extern const std::unordered_map<
  com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi,
  BlockNodeApi>
  gProtobufBlockNodeApiToBlockNodeApi;

/**
 * Map of BlockNodeApi values to the corresponding protobuf BlockNodeApi.
 */
extern const std::unordered_map<
  BlockNodeApi,
  com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi>
  gBlockNodeApiToProtobufBlockNodeApi;

/**
 * Map of BlockNodeApi to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<BlockNodeApi, const char*> gBlockNodeApiToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_BLOCK_NODE_API_H_
