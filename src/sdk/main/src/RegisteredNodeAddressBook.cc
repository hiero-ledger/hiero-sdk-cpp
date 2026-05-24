// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNodeAddressBook.h"

#include <nlohmann/json.hpp>

namespace Hiero
{

//-----
std::string RegisteredNodeAddressBook::toString() const
{
  nlohmann::json json;
  for (const auto& node : mRegisteredNodes)
  {
    json["mRegisteredNodes"].push_back(node.toString());
  }
  return json.dump();
}

} // namespace Hiero
