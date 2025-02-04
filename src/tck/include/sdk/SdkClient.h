// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_SDK_CLIENT_H_
#define HIERO_TCK_CPP_SDK_CLIENT_H_

#include <Client.h>

#include <chrono>
#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::SdkClient
{
/**
 * Forward declarations.
 */
struct ResetParams;
struct SetupParams;

/**
 * The default amount of time the SdkClient should wait for a gRPC request.
 */
constexpr static std::chrono::seconds DEFAULT_TCK_REQUEST_TIMEOUT = std::chrono::seconds(30);

/**
 * Reset the SDK client.
 *
 * @return The JSON response.
 */
nlohmann::json reset(const ResetParams& params);

/**
 * Setup the SDK client.
 *
 * @param operatorAccountId  The ID of the operator account to use.
 * @param operatorPrivateKey The private key of the operator to use.
 * @param nodeIp             The IP of the node with which the Client should communicate.
 * @param nodeAccountId      The ID of the node account running the node with which this Client should communicate.
 * @param mirrorNetworkIp    The IP of the mirror node with which the Client should communicate.
 * @return The JSON response.
 */
nlohmann::json setup(const SetupParams& params);

/**
 * Get the Hiero C++ SDK client this SdkClient is using to execute requests.
 *
 * @return Reference to the Hiero C++ SDK client this SdkClient is using to execute requests.
 */
[[nodiscard]] const Client& getClient();

} // namespace Hiero::TCK::SdkClient

#endif // HIERO_TCK_CPP_SDK_CLIENT_H_
