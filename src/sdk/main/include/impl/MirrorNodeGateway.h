// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_MIRRORNODEGATEWAY_H
#define HIERO_SDK_CPP_MIRRORNODEGATEWAY_H

#include "impl/HttpClient.h"
#include "impl/MirrorNodeRouter.h"

#include <string_view>

#include <nlohmann/json.hpp>

namespace Hiero::internal::MirrorNodeGateway
{
/**
 * Perform a mirror node query.
 *
 * This function constructs a URL based on the provided mirror node URL, query type,
 * and parameters. It then sends a REST request to the constructed URL using an
 * HttpClient, retrieves the response, and parses it into a JSON object.
 *
 * @param mirrorNodeUrl The mirror node URL.
 * @param params A vector of strings representing parameters for the query.
 * @param queryType The type of the query.
 * @return A JSON object representing the response of the mirror node query.
 * @throws IllegalStateException If an error occurs during the mirror node query or JSON parsing.
 */
nlohmann::json MirrorNodeQuery(std::string_view mirrorNodeUrl,
                               const std::vector<std::string>& params,
                               std::string_view queryType);

/**
 * Replaces all occurrences of a substring in a string.
 *
 * This function replaces all occurrences of the substring specified by 'search'
 * with the string specified by 'replace' in the original string.
 *
 * @param original The original string to be modified.
 * @param search The substring to search for.
 * @param replace The string to replace the occurrences of 'search'.
 */
void replaceParameters(std::string& original, std::string_view search, std::string_view replace);

/**
 * Builds a URL based on a mirror node URL, query type, and parameters.
 *
 * This function constructs a URL by combining a mirror node URL, a query type,
 * and a list of parameters. It replaces occurrences of "$" in the route with
 * the provided parameters.
 *
 * @param mirrorNodeUrl The mirror node URL.
 * @param queryType The type of the query.
 * @param params A vector of strings representing parameters.
 * @return The constructed URL.
 */
std::string buildUrlForNetwork(std::string_view mirrorNodeUrl,
                               std::string_view queryType,
                               const std::vector<std::string>& params,
                               bool& isLocalNetwork);
} // namespace Hiero::internal::MirrorNodeGateway
#endif // HIERO_SDK_CPP_MIRRORNODEGATEWAY_H