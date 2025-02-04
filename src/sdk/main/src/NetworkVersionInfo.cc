// SPDX-License-Identifier: Apache-2.0
#include "NetworkVersionInfo.h"
#include "impl/Utilities.h"

#include <network_get_version_info.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
NetworkVersionInfo::NetworkVersionInfo(const SemanticVersion& hapi, const SemanticVersion& hiero)
  : mProtobufVersion(hapi)
  , mServicesVersion(hiero)
{
}

//-----
NetworkVersionInfo NetworkVersionInfo::fromProtobuf(const proto::NetworkGetVersionInfoResponse& proto)
{
  return { SemanticVersion::fromProtobuf(proto.hapiprotoversion()),
           SemanticVersion::fromProtobuf(proto.hederaservicesversion()) };
}

//-----
NetworkVersionInfo NetworkVersionInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::NetworkGetVersionInfoResponse networkGetVersionInfoResponse;
  networkGetVersionInfoResponse.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(networkGetVersionInfoResponse);
}

//-----
std::unique_ptr<proto::NetworkGetVersionInfoResponse> NetworkVersionInfo::toProtobuf() const
{
  auto proto = std::make_unique<proto::NetworkGetVersionInfoResponse>();
  proto->set_allocated_hapiprotoversion(mProtobufVersion.toProtobuf().release());
  proto->set_allocated_hederaservicesversion(mServicesVersion.toProtobuf().release());
  return proto;
}

//-----
std::vector<std::byte> NetworkVersionInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string NetworkVersionInfo::toString() const
{
  nlohmann::json json;
  json["mProtobufVersion"] = mProtobufVersion.toString();
  json["mServicesVersion"] = mServicesVersion.toString();
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const NetworkVersionInfo& info)
{
  os << info.toString();
  return os;
}

} // namespace Hiero
