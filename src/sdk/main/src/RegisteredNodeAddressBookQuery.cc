// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNodeAddressBookQuery.h"
#include "Client.h"
#include "RegisteredNode.h"
#include "RegisteredNodeAddressBook.h"
#include "impl/HttpClient.h"

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero
{

namespace
{

void appendNodes(const nlohmann::json& json, RegisteredNodeAddressBook& result)
{
  if (json.contains("registered_nodes") && json["registered_nodes"].is_array())
  {
    for (const auto& nodeJson : json["registered_nodes"])
    {
      result.mRegisteredNodes.push_back(RegisteredNode::fromJson(nodeJson));
    }
  }
}

std::string resolveNextUrl(const nlohmann::json& json, const std::string& mirrorBase)
{
  if (!json.contains("links") || !json["links"].is_object() || !json["links"].contains("next"))
  {
    return {};
  }
  if (json["links"]["next"].is_null())
  {
    return {};
  }
  const std::string next = json["links"]["next"].get<std::string>();
  if (next.empty())
  {
    return {};
  }
  return (next.rfind("http", 0) == 0) ? next : mirrorBase + next;
}

} // namespace

//-----
RegisteredNodeAddressBook RegisteredNodeAddressBookQuery::execute(const Client& client) const
{
  const std::vector<std::string> mirrorNetworks = client.getMirrorNetwork();
  if (mirrorNetworks.empty())
  {
    return {};
  }

  // The registered-nodes endpoint lives on the mirror node Java REST API. For local environments that's
  // 127.0.0.1:8084, not the gRPC port (5600) stored in mirrorNetwork. For remote environments, the same
  // hostname serves both gRPC (port 443) and the REST API.
  const std::string& mirrorAddress = mirrorNetworks.front();
  const std::string host = mirrorAddress.substr(0, mirrorAddress.find(':'));
  const bool isLocal = (host == "127.0.0.1" || host == "localhost");
  const std::string mirrorBase = isLocal ? "http://" + host + ":8084" : "https://" + mirrorAddress;
  std::string currentUrl = mirrorBase + "/api/v1/network/registered-nodes";

  if (mLimit.has_value())
  {
    currentUrl += "?limit=" + std::to_string(mLimit.value());
  }

  RegisteredNodeAddressBook result;
  while (!currentUrl.empty())
  {
    const nlohmann::json json = nlohmann::json::parse(internal::HttpClient::invokeREST(currentUrl, "GET"));
    appendNodes(json, result);
    currentUrl = resolveNextUrl(json, mirrorBase);
  }

  return result;
}

//-----
RegisteredNodeAddressBookQuery& RegisteredNodeAddressBookQuery::setLimit(uint32_t limit)
{
  mLimit = limit;
  return *this;
}

} // namespace Hiero