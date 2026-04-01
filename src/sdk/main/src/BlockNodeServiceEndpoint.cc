// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeServiceEndpoint.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <services/registered_service_endpoint.pb.h>

namespace Hiero
{

//-----
BlockNodeServiceEndpoint BlockNodeServiceEndpoint::fromProtobuf(
  const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto)
{
  BlockNodeServiceEndpoint endpoint;
  endpoint.readCommonFields(proto);
  endpoint.mEndpointApi = gProtobufBlockNodeApiToBlockNodeApi.at(proto.block_node().endpoint_api());
  return endpoint;
}

//-----
std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint>
BlockNodeServiceEndpoint::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint>();
  fillCommonFields(*proto);
  proto->mutable_block_node()->set_endpoint_api(gBlockNodeApiToProtobufBlockNodeApi.at(mEndpointApi));
  return proto;
}

//-----
std::string BlockNodeServiceEndpoint::toString() const
{
  nlohmann::json json;
  json["type"] = "BlockNodeServiceEndpoint";
  if (mIpAddress.has_value())
  {
    json["ipAddress"] = internal::HexConverter::bytesToHex(mIpAddress.value());
  }
  if (mDomainName.has_value())
  {
    json["domainName"] = mDomainName.value();
  }
  json["port"] = mPort;
  json["requiresTls"] = mRequiresTls;
  json["endpointApi"] = gBlockNodeApiToString.at(mEndpointApi);
  return json.dump();
}

//-----
BlockNodeServiceEndpoint& BlockNodeServiceEndpoint::setEndpointApi(BlockNodeApi api)
{
  mEndpointApi = api;
  return *this;
}

} // namespace Hiero
