// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNodeAddressBookQuery.h"
#include "Client.h"
#include "RegisteredNode.h"
#include "RegisteredNodeAddressBook.h"
#include "impl/MirrorNodeGateway.h"
#include "impl/MirrorNodeRouter.h"

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero
{

//-----
RegisteredNodeAddressBook RegisteredNodeAddressBookQuery::execute(const Client& client) const
{
  const std::vector<std::string> mirrorNetworks = client.getMirrorNetwork();
  if (mirrorNetworks.empty())
  {
    return {};
  }

  std::string currentUrl = internal::MirrorNodeGateway::buildUrlForNetwork(
    mirrorNetworks.front(), internal::MirrorNodeGateway::REGISTERED_NODES_QUERY, {}, "GET");

  if (mLimit.has_value())
  {
    currentUrl += "?limit=" + std::to_string(mLimit.value());
  }

  // Extract the mirror base URL (everything before "/api/") for resolving relative next links.
  std::string mirrorBase;
  const auto apiPos = currentUrl.find("/api/");
  if (apiPos != std::string::npos)
  {
    mirrorBase = currentUrl.substr(0, apiPos);
  }

  RegisteredNodeAddressBook result;

  while (!currentUrl.empty())
  {
    const std::string responseStr = internal::HttpClient::invokeREST(currentUrl, "GET");
    const nlohmann::json json = nlohmann::json::parse(responseStr);

    if (json.contains("registered_nodes") && json["registered_nodes"].is_array())
    {
      for (const auto& nodeJson : json["registered_nodes"])
      {
        result.mRegisteredNodes.push_back(RegisteredNode::fromJson(nodeJson));
      }
    }

    currentUrl.clear();
    if (json.contains("links") && json["links"].is_object() && json["links"].contains("next") &&
        !json["links"]["next"].is_null())
    {
      const std::string next = json["links"]["next"].get<std::string>();
      if (!next.empty())
      {
        // Mirror node returns relative paths (e.g. "/api/v1/network/registered-nodes?...").
        if (next.rfind("http", 0) == 0)
        {
          currentUrl = next;
        }
        else
        {
          currentUrl = mirrorBase + next;
        }
      }
    }
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