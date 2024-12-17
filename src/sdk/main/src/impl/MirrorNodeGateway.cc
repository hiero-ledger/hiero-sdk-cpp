// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNodeGateway.h"
#include "exceptions/IllegalStateException.h"
#include "impl/HttpClient.h"

#include <chrono>
#include <string>
#include <thread>

using json = nlohmann::json;

namespace Hiero::internal::MirrorNodeGateway
{
//-----
json MirrorNodeQuery(std::string_view mirrorNodeUrl, const std::vector<std::string>& params, std::string_view queryType)
{
  std::string response;
  try
  {
    bool isLocalNetwork = true;
    const std::string url = buildUrlForNetwork(mirrorNodeUrl, queryType, params, isLocalNetwork);

    // this is needed because of Mirror Node update delay time
    // agreed to be handled by the user not a local network
    if (isLocalNetwork)
    {
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    response = HttpClient::invokeREST(url);
  }
  catch (const std::exception& e)
  {
    throw IllegalStateException(std::string(e.what() + std::string("Illegal json state!")));
  }
  return json::parse(response);
}

//-----
void replaceParameters(std::string& original, std::string_view search, std::string_view replace)
{
  size_t startPos = 0;

  while ((startPos = original.find(search, startPos)) != std::string::npos)
  {
    original.replace(startPos, search.length(), replace);
    startPos += replace.length();
  }
}

//-----
std::string buildUrlForNetwork(std::string_view mirrorNodeUrl,
                               std::string_view queryType,
                               const std::vector<std::string>& params,
                               bool& isLocalNetwork)
{
  std::string httpPrefix = "http://";
  std::string localPrefix = "127.0.0.1:5600";
  std::string url = mirrorNodeUrl.data();
  if (url.compare(0, httpPrefix.length(), httpPrefix) != 0 && url.compare(0, localPrefix.length(), localPrefix) != 0)
  {
    isLocalNetwork = false;
    url = "https://" + url;
  }
  if (url == localPrefix)
  {
    url = httpPrefix + url;
    url.replace(url.length() - 4, 4, "5551");
  }
  MirrorNodeRouter router;
  std::string route = router.getRoute(queryType.data()).data();
  for_each(
    params.begin(), params.end(), [&route](const std::string& replace) { replaceParameters(route, "$", replace); });
  url += route;
  return url;
}
}