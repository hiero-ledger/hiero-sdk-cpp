// SPDX-License-Identifier: Apache-2.0

#include "impl/HttpClient.h"

#include <httplib.h>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Hiero::internal
{
namespace
{
// The index in a URL to begin searching for the path after the end of the URL scheme ("http://" or "https://").
const int SCHEME_END_INDEX = 8;

//
// Perform an HTTP request.
//
// @param url         The URL to which to send the request.
// @param method      The HTTP method type of this request.
// @param body        The body of the request.
// @param contentType The content type of the request.
// @return The response of the request.
//
[[nodiscard]] std::string performRequest(std::string_view url,
                                         std::string_view method,
                                         std::string_view body,
                                         std::string_view contentType = "application/json")
{
  // Create an HTTP client to communicate with the given URL.
  httplib::Client client(std::string(url.substr(0, url.find('/', SCHEME_END_INDEX))));
  const std::string path = url.substr(url.find('/', SCHEME_END_INDEX)).data();

  httplib::Result res;

  // Perform the request based on the HTTP method
  if (method == "GET")
  {
    res = client.Get(path);
  }
  else if (method == "POST")
  {
    res = client.Post(path, body.data(), body.size(), contentType.data());
  }
  else
  {
    throw std::invalid_argument(std::string("Unsupported HTTP method: ") + method.data());
  }

  if (!res)
  {
    throw std::runtime_error("HTTP error: " + httplib::to_string(res.error()));
  }

  return res->body;
}

//
// Perform an HTTP request and return the status code.
//
// @param url         The URL to which to send the request.
// @param method      The HTTP method type of this request.
// @param body        The body of the request.
// @param contentType The content type of the request.
// @param statusCode  Output parameter for the HTTP status code.
// @return The response of the request.
//
[[nodiscard]] std::string performRequestWithStatus(std::string_view url,
                                                   std::string_view method,
                                                   std::string_view body,
                                                   std::string_view contentType,
                                                   int& statusCode)
{
  // Create an HTTP client to communicate with the given URL.
  httplib::Client client(std::string(url.substr(0, url.find('/', SCHEME_END_INDEX))));
  const std::string path = url.substr(url.find('/', SCHEME_END_INDEX)).data();

  httplib::Result res;

  // Perform the request based on the HTTP method
  if (method == "GET")
  {
    res = client.Get(path);
  }
  else if (method == "POST")
  {
    res = client.Post(path, body.data(), body.size(), contentType.data());
  }
  else
  {
    throw std::invalid_argument(std::string("Unsupported HTTP method: ") + method.data());
  }

  if (!res)
  {
    statusCode = -1;
    throw std::runtime_error("HTTP error: " + httplib::to_string(res.error()));
  }

  statusCode = res->status;
  return res->body;
}

} // namespace

// example mirrorNode query:
// httpClient.invokeREST("https://testnet.mirrornode.hedera.com/api/v1/accounts/" + newAccountId ,"GET", "");
// note: should time out before calling this function because the mirror node is not updated on time if accountID has
// been created exactly before the call. Works without timeout if the data in the mirror node is there from some seconds
// beforehand
std::string HttpClient::invokeREST(std::string_view url,
                                   std::string_view httpMethod,
                                   std::string_view requestBody,
                                   std::string_view contentType)
{
  return performRequest(url, httpMethod, requestBody, contentType);
}

//-----
std::string HttpClient::invokeRESTWithStatus(std::string_view url,
                                             std::string_view httpMethod,
                                             std::string_view requestBody,
                                             std::string_view contentType,
                                             int& statusCode)
{
  return performRequestWithStatus(url, httpMethod, requestBody, contentType, statusCode);
}

//-----

} // namespace Hiero::internal