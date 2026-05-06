// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeServiceEndpoint.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <services/registered_service_endpoint.pb.h>

namespace Hiero
{

using ProtoApi = com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi;

//-----
BlockNodeServiceEndpoint BlockNodeServiceEndpoint::fromProtobuf(
  const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto)
{
  BlockNodeServiceEndpoint endpoint;
  endpoint.readCommonFields(proto);
  if (proto.block_node().endpoint_api_size() > 0)
  {
    endpoint.mEndpointApi =
      gProtobufBlockNodeApiToBlockNodeApi.at(static_cast<ProtoApi>(proto.block_node().endpoint_api(0)));
  }
  return endpoint;
}

//-----
std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> BlockNodeServiceEndpoint::toProtobuf()
  const
{
  auto proto = std::make_unique<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint>();
  fillCommonFields(*proto);
  proto->mutable_block_node()->add_endpoint_api(gBlockNodeApiToProtobufBlockNodeApi.at(mEndpointApi));
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
BlockNodeServiceEndpoint BlockNodeServiceEndpoint::fromJson(const nlohmann::json& json)
{
  static const std::unordered_map<std::string, BlockNodeApi> kStringToApi = {
    {"OTHER",             BlockNodeApi::OTHER           },
    { "STATUS",           BlockNodeApi::STATUS          },
    { "PUBLISH",          BlockNodeApi::PUBLISH         },
    { "SUBSCRIBE_STREAM", BlockNodeApi::SUBSCRIBE_STREAM},
    { "STATE_PROOF",      BlockNodeApi::STATE_PROOF     },
  };

  BlockNodeServiceEndpoint endpoint;
  endpoint.readCommonFieldsFromJson(json);

  if (json.contains("block_node") && !json["block_node"].is_null())
  {
    const auto& bn = json["block_node"];
    if (bn.contains("endpoint_apis") && bn["endpoint_apis"].is_array() && !bn["endpoint_apis"].empty())
    {
      const std::string apiStr = bn["endpoint_apis"][0].get<std::string>();
      const auto it = kStringToApi.find(apiStr);
      endpoint.mEndpointApi = (it != kStringToApi.end()) ? it->second : BlockNodeApi::OTHER;
    }
  }

  return endpoint;
}

//-----
BlockNodeServiceEndpoint& BlockNodeServiceEndpoint::setEndpointApi(BlockNodeApi api)
{
  mEndpointApi = api;
  return *this;
}

} // namespace Hiero
