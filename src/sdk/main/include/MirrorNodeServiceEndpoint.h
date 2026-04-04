// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_MIRROR_NODE_SERVICE_ENDPOINT_H_
#define HIERO_SDK_CPP_MIRROR_NODE_SERVICE_ENDPOINT_H_

#include "RegisteredServiceEndpoint.h"

namespace com::hedera::hapi::node::addressbook
{
class RegisteredServiceEndpoint;
}

namespace Hiero
{
/**
 * A registered service endpoint for a mirror node. Currently carries no additional fields beyond the base endpoint.
 * Defined as a distinct subtype so that mirror-node-specific fields can be added in future HIPs without breaking
 * changes.
 */
class MirrorNodeServiceEndpoint : public RegisteredServiceEndpoint
{
public:
  MirrorNodeServiceEndpoint() = default;

  /**
   * Construct a MirrorNodeServiceEndpoint from a RegisteredServiceEndpoint protobuf object.
   *
   * @param proto The RegisteredServiceEndpoint protobuf object from which to construct.
   * @return The constructed MirrorNodeServiceEndpoint.
   */
  [[nodiscard]] static MirrorNodeServiceEndpoint fromProtobuf(
    const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto);

  /**
   * Construct a RegisteredServiceEndpoint protobuf object from this MirrorNodeServiceEndpoint.
   *
   * @return A unique_ptr to the constructed RegisteredServiceEndpoint protobuf object.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> toProtobuf()
    const override;

  /**
   * Construct a string representation of this MirrorNodeServiceEndpoint.
   *
   * @return The string representation of this MirrorNodeServiceEndpoint.
   */
  [[nodiscard]] std::string toString() const override;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_MIRROR_NODE_SERVICE_ENDPOINT_H_
