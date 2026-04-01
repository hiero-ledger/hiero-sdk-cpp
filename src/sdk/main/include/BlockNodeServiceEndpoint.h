// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_BLOCK_NODE_SERVICE_ENDPOINT_H_
#define HIERO_SDK_CPP_BLOCK_NODE_SERVICE_ENDPOINT_H_

#include "BlockNodeApi.h"
#include "RegisteredServiceEndpoint.h"

namespace com::hedera::hapi::node::addressbook
{
class RegisteredServiceEndpoint;
}

namespace Hiero
{
/**
 * A registered service endpoint for a block node. Extends the base endpoint with the specific block node API that the
 * endpoint exposes.
 */
class BlockNodeServiceEndpoint : public RegisteredServiceEndpoint
{
public:
  BlockNodeServiceEndpoint() = default;

  /**
   * Construct a BlockNodeServiceEndpoint from a RegisteredServiceEndpoint protobuf object.
   *
   * @param proto The RegisteredServiceEndpoint protobuf object from which to construct.
   * @return The constructed BlockNodeServiceEndpoint.
   */
  [[nodiscard]] static BlockNodeServiceEndpoint
  fromProtobuf(const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto);

  /**
   * Construct a RegisteredServiceEndpoint protobuf object from this BlockNodeServiceEndpoint.
   *
   * @return A unique_ptr to the constructed RegisteredServiceEndpoint protobuf object.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint>
  toProtobuf() const override;

  /**
   * Construct a string representation of this BlockNodeServiceEndpoint.
   *
   * @return The string representation of this BlockNodeServiceEndpoint.
   */
  [[nodiscard]] std::string toString() const override;

  /**
   * Set the block node API exposed by this endpoint.
   *
   * @param api The BlockNodeApi value.
   * @return A reference to this BlockNodeServiceEndpoint with the newly-set endpoint API.
   */
  BlockNodeServiceEndpoint& setEndpointApi(BlockNodeApi api);

  /**
   * Get the block node API exposed by this endpoint.
   *
   * @return The BlockNodeApi value.
   */
  [[nodiscard]] BlockNodeApi getEndpointApi() const { return mEndpointApi; }

private:
  /**
   * The block node API that this endpoint exposes.
   */
  BlockNodeApi mEndpointApi = BlockNodeApi::OTHER;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_BLOCK_NODE_SERVICE_ENDPOINT_H_
