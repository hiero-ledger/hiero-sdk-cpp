// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_RPC_RELAY_SERVICE_ENDPOINT_H_
#define HIERO_SDK_CPP_RPC_RELAY_SERVICE_ENDPOINT_H_

#include "RegisteredServiceEndpoint.h"

namespace com::hedera::hapi::node::addressbook
{
class RegisteredServiceEndpoint;
}

namespace Hiero
{
/**
 * A registered service endpoint for an RPC relay. Currently carries no additional fields beyond the base endpoint.
 * Defined as a distinct subtype so that RPC-relay-specific fields can be added in future HIPs without breaking changes.
 */
class RpcRelayServiceEndpoint : public RegisteredServiceEndpoint
{
public:
  RpcRelayServiceEndpoint() = default;

  /**
   * Construct a RpcRelayServiceEndpoint from a RegisteredServiceEndpoint protobuf object.
   *
   * @param proto The RegisteredServiceEndpoint protobuf object from which to construct.
   * @return The constructed RpcRelayServiceEndpoint.
   */
  [[nodiscard]] static RpcRelayServiceEndpoint fromProtobuf(
    const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto);

  /**
   * Construct a RegisteredServiceEndpoint protobuf object from this RpcRelayServiceEndpoint.
   *
   * @return A unique_ptr to the constructed RegisteredServiceEndpoint protobuf object.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> toProtobuf()
    const override;

  /**
   * Construct a string representation of this RpcRelayServiceEndpoint.
   *
   * @return The string representation of this RpcRelayServiceEndpoint.
   */
  [[nodiscard]] std::string toString() const override;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_RPC_RELAY_SERVICE_ENDPOINT_H_
