// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNode.h"

#include <nlohmann/json.hpp>

namespace Hiero
{

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
