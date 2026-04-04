// SPDX-License-Identifier: Apache-2.0
#include "RegisteredServiceEndpoint.h"
#include "BlockNodeServiceEndpoint.h"
#include "MirrorNodeServiceEndpoint.h"
#include "RpcRelayServiceEndpoint.h"
#include "impl/Utilities.h"

#include <services/registered_service_endpoint.pb.h>

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
    default:
      throw std::invalid_argument("RegisteredServiceEndpoint protobuf has no endpoint_type set");
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

} // namespace Hiero
