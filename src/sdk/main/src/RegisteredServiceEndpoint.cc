// SPDX-License-Identifier: Apache-2.0
#include "RegisteredServiceEndpoint.h"
#include "BlockNodeServiceEndpoint.h"
#include "GeneralServiceEndpoint.h"
#include "MirrorNodeServiceEndpoint.h"
#include "RpcRelayServiceEndpoint.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <services/registered_service_endpoint.pb.h>

#include <sstream>
#include <stdexcept>

namespace Hiero
{

using ProtoEndpoint = com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint;

//-----
std::unique_ptr<RegisteredServiceEndpoint> RegisteredServiceEndpoint::fromProtobuf(const ProtoEndpoint& proto)
{
  switch (proto.endpoint_type_case())
  {
    case ProtoEndpoint::kBlockNode:
      return std::make_unique<BlockNodeServiceEndpoint>(BlockNodeServiceEndpoint::fromProtobuf(proto));
    case ProtoEndpoint::kMirrorNode:
      return std::make_unique<MirrorNodeServiceEndpoint>(MirrorNodeServiceEndpoint::fromProtobuf(proto));
    case ProtoEndpoint::kRpcRelay:
      return std::make_unique<RpcRelayServiceEndpoint>(RpcRelayServiceEndpoint::fromProtobuf(proto));
    case ProtoEndpoint::kGeneralService:
      return std::make_unique<GeneralServiceEndpoint>(GeneralServiceEndpoint::fromProtobuf(proto));
    default:
      throw std::invalid_argument("RegisteredServiceEndpoint protobuf has no endpoint_type set");
  }
}

//-----
std::unique_ptr<RegisteredServiceEndpoint> RegisteredServiceEndpoint::fromJson(const nlohmann::json& json)
{
  const std::string type = json.value("type", std::string{});
  if (type == "BLOCK_NODE")
  {
    return std::make_unique<BlockNodeServiceEndpoint>(BlockNodeServiceEndpoint::fromJson(json));
  }
  else if (type == "MIRROR_NODE")
  {
    return std::make_unique<MirrorNodeServiceEndpoint>(MirrorNodeServiceEndpoint::fromJson(json));
  }
  else if (type == "RPC_RELAY")
  {
    return std::make_unique<RpcRelayServiceEndpoint>(RpcRelayServiceEndpoint::fromJson(json));
  }
  else if (type == "GENERAL_SERVICE")
  {
    return std::make_unique<GeneralServiceEndpoint>(GeneralServiceEndpoint::fromJson(json));
  }
  else
  {
    throw std::invalid_argument("Unknown RegisteredServiceEndpoint type: " + type);
  }
}

//-----
RegisteredServiceEndpoint& RegisteredServiceEndpoint::setIpAddress(const std::vector<std::byte>& ipAddress)
{
  mIpAddress = ipAddress;
  mDomainName.reset();
  return *this;
}

//-----
RegisteredServiceEndpoint& RegisteredServiceEndpoint::setDomainName(std::string_view domainName)
{
  mDomainName = domainName;
  mIpAddress.reset();
  return *this;
}

//-----
RegisteredServiceEndpoint& RegisteredServiceEndpoint::setPort(uint32_t port)
{
  mPort = port;
  return *this;
}

//-----
RegisteredServiceEndpoint& RegisteredServiceEndpoint::setRequiresTls(bool requiresTls)
{
  mRequiresTls = requiresTls;
  return *this;
}

//-----
void RegisteredServiceEndpoint::fillCommonFields(ProtoEndpoint& proto) const
{
  if (mIpAddress.has_value())
  {
    proto.set_ip_address(internal::Utilities::byteVectorToString(mIpAddress.value()));
  }
  else if (mDomainName.has_value())
  {
    proto.set_domain_name(mDomainName.value());
  }

  proto.set_port(mPort);
  proto.set_requires_tls(mRequiresTls);
}

//-----
void RegisteredServiceEndpoint::readCommonFields(const ProtoEndpoint& proto)
{
  switch (proto.address_case())
  {
    case ProtoEndpoint::kIpAddress:
      mIpAddress = internal::Utilities::stringToByteVector(proto.ip_address());
      break;
    case ProtoEndpoint::kDomainName:
      mDomainName = proto.domain_name();
      break;
    default:
      break;
  }

  mPort = proto.port();
  mRequiresTls = proto.requires_tls();
}

//-----
void RegisteredServiceEndpoint::readCommonFieldsFromJson(const nlohmann::json& json)
{
  if (json.contains("ip_address") && !json["ip_address"].is_null())
  {
    const std::string ipStr = json["ip_address"].get<std::string>();
    std::vector<std::byte> bytes;
    std::istringstream stream(ipStr);
    std::string octet;
    while (std::getline(stream, octet, '.'))
    {
      bytes.push_back(static_cast<std::byte>(std::stoi(octet)));
    }
    if (!bytes.empty())
    {
      setIpAddress(bytes);
    }
  }
  else if (json.contains("domain_name") && !json["domain_name"].is_null())
  {
    setDomainName(json["domain_name"].get<std::string>());
  }

  if (json.contains("port") && !json["port"].is_null())
  {
    setPort(json["port"].get<uint32_t>());
  }

  if (json.contains("requires_tls") && !json["requires_tls"].is_null())
  {
    setRequiresTls(json["requires_tls"].get<bool>());
  }
}

} // namespace Hiero
