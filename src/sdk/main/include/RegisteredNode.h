// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_NODE_H_
#define HIERO_SDK_CPP_REGISTERED_NODE_H_

#include "RegisteredServiceEndpoint.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Hiero
{
class Key;

/**
 * An immutable representation of a registered node as stored in network state. Returned by mirror node queries.
 */
class RegisteredNode
{
public:
  /**
   * The unique identifier assigned by the network when the registered node was created.
   */
  uint64_t mRegisteredNodeId = 0;

  /**
   * The administrative key controlled by the node operator.
   */
  std::shared_ptr<Key> mAdminKey;

  /**
   * A short description of the node.
   */
  std::string mDescription;

  /**
   * The service endpoints published by this registered node.
   */
  std::vector<std::shared_ptr<RegisteredServiceEndpoint>> mServiceEndpoints;

  /**
   * Construct a string representation of this RegisteredNode object.
   *
   * @return The string representation of this RegisteredNode object.
   */
  [[nodiscard]] std::string toString() const;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_NODE_H_
