// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_HTTP_CLIENT_H_
#define HIERO_SDK_CPP_IMPL_HTTP_CLIENT_H_

#ifndef CPPHTTPLIB_OPENSSL_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include <httplib.h>
#include <string>
#include <string_view>

namespace Hiero::internal::HttpClient
{
/**
 * Fetches data from the specified URL using the provided RPC method.
 * @param url       The URL to fetch data from.
 * @param rpcMethod The RPC method.
 * @return The response data as a string.
 */
[[nodiscard]] std::string invokeRPC(std::string_view url, std::string_view rpcMethod);

/**
 * Create a GET or POST request. Can be further extended for supporting other HTTP methods or handle more advanced
 * scenarios as needed.
 * @param url         The URL to which to submit the request.
 * @param httpMethod  The HTTP method.
 * @param requestBody The HTTP request body.
 * @return The response data as a string.
 */
[[nodiscard]] std::string invokeREST(std::string_view url,
                                     std::string_view httpMethod = "GET",
                                     std::string_view requestBody = "");

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_HTTP_CLIENT_H_