// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNodeRouter.h"
#include "impl/Utilities.h"

#include <filesystem>
#include <iostream>

namespace Hiero::internal::MirrorNodeGateway
{
//-----
std::string MirrorNodeRouter::getRoute(std::string_view queryType) const
{
  std::string queryRoute;
  auto route = routes.find(queryType.data());
  if (route != routes.end())
  {
    queryRoute = route->second;
  }
  else
  {
    throw std::runtime_error("Route not found in the routes map");
  }
  return queryRoute;
}
} // namespace Hiero::internal::MirrorNodeGateway