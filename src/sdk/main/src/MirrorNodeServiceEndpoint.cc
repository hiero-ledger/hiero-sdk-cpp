// SPDX-License-Identifier: Apache-2.0
#include "MirrorNodeServiceEndpoint.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <services/registered_service_endpoint.pb.h>

namespace Hiero
{

//-----
MirrorNodeServiceEndpoint MirrorNodeServiceEndpoint::fromProtobuf(
  const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto)
{
  MirrorNodeServiceEndpoint endpoint;
  endpoint.readCommonFields(proto);
  return endpoint;
}

//-----
std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> MirrorNodeServiceEndpoint::toProtobuf()
  const
{
  auto proto = std::make_unique<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint>();
  fillCommonFields(*proto);
  proto->mutable_mirror_node(); // sets the oneof to mirror_node (empty message)
  return proto;
}

//-----
std::string MirrorNodeServiceEndpoint::toString() const
{
  nlohmann::json json;
  json["type"] = "MirrorNodeServiceEndpoint";
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
