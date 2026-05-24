// SPDX-License-Identifier: Apache-2.0
#include "GeneralServiceEndpoint.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <services/registered_service_endpoint.pb.h>

namespace Hiero
{

//-----
GeneralServiceEndpoint GeneralServiceEndpoint::fromProtobuf(
  const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto)
{
  GeneralServiceEndpoint endpoint;
  endpoint.readCommonFields(proto);
  if (!proto.general_service().description().empty())
  {
    endpoint.mDescription = proto.general_service().description();
  }
  return endpoint;
}

//-----
GeneralServiceEndpoint GeneralServiceEndpoint::fromJson(const nlohmann::json& json)
{
  GeneralServiceEndpoint endpoint;
  endpoint.readCommonFieldsFromJson(json);

  if (json.contains("general_service") && !json["general_service"].is_null())
  {
    const auto& gs = json["general_service"];
    if (gs.contains("description") && !gs["description"].is_null())
    {
      endpoint.mDescription = gs["description"].get<std::string>();
    }
  }

  return endpoint;
}

//-----
std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> GeneralServiceEndpoint::toProtobuf()
  const
{
  auto proto = std::make_unique<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint>();
  fillCommonFields(*proto);
  if (mDescription.has_value())
  {
    proto->mutable_general_service()->set_description(mDescription.value());
  }
  else
  {
    proto->mutable_general_service();
  }
  return proto;
}

//-----
std::string GeneralServiceEndpoint::toString() const
{
  nlohmann::json json;
  json["type"] = "GeneralServiceEndpoint";
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
  if (mDescription.has_value())
  {
    json["description"] = mDescription.value();
  }
  return json.dump();
}

//-----
GeneralServiceEndpoint& GeneralServiceEndpoint::setDescription(std::string_view description)
{
  mDescription = description;
  return *this;
}

} // namespace Hiero