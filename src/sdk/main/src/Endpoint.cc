// SPDX-License-Identifier: Apache-2.0
#include "Endpoint.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
Endpoint Endpoint::fromProtobuf(const proto::ServiceEndpoint& protoServiceEndpoint)
{
  return Endpoint()
    .setAddress(IPv4Address::fromBytes(internal::Utilities::stringToByteVector(protoServiceEndpoint.ipaddressv4())))
    .setPort(static_cast<unsigned int>(protoServiceEndpoint.port()))
    .setDomainName(protoServiceEndpoint.domain_name());
}

//-----
std::unique_ptr<proto::ServiceEndpoint> Endpoint::toProtobuf() const
{
  auto proto = std::make_unique<proto::ServiceEndpoint>();
  proto->set_ipaddressv4(internal::Utilities::byteVectorToString(mAddress.toBytes()));
  proto->set_port(static_cast<int32_t>(mPort));
  proto->set_domain_name(mDomainName);
  return proto;
}

//-----
std::string Endpoint::toString() const
{
  if (!mAddress.isEmpty())
  {
    return mAddress.toString() + ':' + std::to_string(mPort);
  }
  else
  {
    return mDomainName + ':' + std::to_string(mPort);
  }
}

//-----
Endpoint& Endpoint::setAddress(const IPv4Address& address)
{
  mAddress = address;
  return *this;
}

//-----
Endpoint& Endpoint::setPort(unsigned int port)
{
  mPort = port;
  return *this;
}

//-----
Endpoint& Endpoint::setDomainName(std::string_view domainName)
{
  mDomainName = domainName;
  return *this;
}
} // namespace Hiero
