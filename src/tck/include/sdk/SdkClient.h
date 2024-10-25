/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef HEDERA_TCK_CPP_SDK_CLIENT_H_
#define HEDERA_TCK_CPP_SDK_CLIENT_H_

#include <Client.h>

#include <chrono>
#include <nlohmann/json_fwd.hpp>

namespace Hedera::TCK::SdkClient
{
/**
 * Forward declarations.
 */
class ResetParams;
class SetupParams;

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
 * Get the Hedera C++ SDK client this SdkClient is using to execute requests.
 *
 * @return Reference to the Hedera C++ SDK client this SdkClient is using to execute requests.
 */
[[nodiscard]] const Client& getClient();

} // namespace Hedera::TCK::SdkClient

#endif // HEDERA_TCK_CPP_SDK_CLIENT_H_
