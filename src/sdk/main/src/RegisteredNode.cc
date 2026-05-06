// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNode.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PublicKey.h"
#include "PublicKey.h"
#include "impl/HexConverter.h"

#include <nlohmann/json.hpp>

namespace Hiero
{

//-----
RegisteredNode RegisteredNode::fromJson(const nlohmann::json& json)
{
  RegisteredNode node;

  if (json.contains("registered_node_id") && !json["registered_node_id"].is_null())
  {
    const auto& idField = json["registered_node_id"];
    node.mRegisteredNodeId = idField.is_string() ? std::stoull(idField.get<std::string>()) : idField.get<uint64_t>();
  }

  if (json.contains("admin_key") && !json["admin_key"].is_null())
  {
    const auto& keyObj = json["admin_key"];
    const std::string keyHex = keyObj.value("key", std::string{});
    const std::string keyType = keyObj.value("_type", std::string{});
    if (!keyHex.empty())
    {
      const auto keyBytes = internal::HexConverter::hexToBytes(keyHex);
      if (keyType == "ED25519")
      {
        node.mAdminKey = std::shared_ptr<Key>(ED25519PublicKey::fromBytes(keyBytes));
      }
      else if (keyType == "ECDSA_SECP256K1")
      {
        node.mAdminKey = std::shared_ptr<Key>(ECDSAsecp256k1PublicKey::fromBytes(keyBytes));
      }
      else
      {
        node.mAdminKey = std::shared_ptr<Key>(PublicKey::fromBytes(keyBytes));
      }
    }
  }

  if (json.contains("description") && !json["description"].is_null())
  {
    node.mDescription = json["description"].get<std::string>();
  }

  if (json.contains("service_endpoints") && json["service_endpoints"].is_array())
  {
    for (const auto& epJson : json["service_endpoints"])
    {
      node.mServiceEndpoints.push_back(RegisteredServiceEndpoint::fromJson(epJson));
    }
  }

  return node;
}

//-----
std::string RegisteredNode::toString() const
{
  nlohmann::json json;
  json["mRegisteredNodeId"] = mRegisteredNodeId;
  json["mDescription"] = mDescription;

  for (const auto& ep : mServiceEndpoints)
  {
    json["mServiceEndpoints"].push_back(ep->toString());
  }

  return json.dump();
}

} // namespace Hiero
