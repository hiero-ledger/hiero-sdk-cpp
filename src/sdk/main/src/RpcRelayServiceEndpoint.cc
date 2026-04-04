// SPDX-License-Identifier: Apache-2.0
#include "RpcRelayServiceEndpoint.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <services/registered_service_endpoint.pb.h>

namespace Hiero
{

//-----
RpcRelayServiceEndpoint RpcRelayServiceEndpoint::fromProtobuf(
  const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto)
{
  RpcRelayServiceEndpoint endpoint;
  endpoint.readCommonFields(proto);
  return endpoint;
}

//-----
std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> RpcRelayServiceEndpoint::toProtobuf()
  const
{
  auto proto = std::make_unique<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint>();
  fillCommonFields(*proto);
  proto->mutable_rpc_relay(); // sets the oneof to rpc_relay (empty message)
  return proto;
}

//-----
std::string RpcRelayServiceEndpoint::toString() const
{
  nlohmann::json json;
  json["type"] = "RpcRelayServiceEndpoint";
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
  return json.dump();
}

} // namespace Hiero
