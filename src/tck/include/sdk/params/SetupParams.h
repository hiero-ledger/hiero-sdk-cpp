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
#ifndef HEDERA_TCK_CPP_SETUP_PARAMS_H_
#define HEDERA_TCK_CPP_SETUP_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hedera::TCK::SdkClient
{
/**
 * Struct to hold the arguments for a `setup` JSON-RPC method call.
 */
struct SetupParams
{
  /**
   * The ID of the operator account.
   */
  std::string operatorAccountId;

  /**
   * The private key of the operator account.
   */
  std::string operatorPrivateKey;

  /**
   * The IP of the consensus node. Required for local network configurations.
   */
  std::optional<std::string> nodeIp;

  /**
   * The ID of the node account. Required for local network configurations.
   */
  std::optional<std::string> nodeAccountId;

  /**
   * The IP of the mirror node. Required for local network configurations.
   */
  std::optional<std::string> mirrorNetworkIp;
};

} // namespace Hedera::TCK::SdkClient

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert SetupParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hedera::TCK::SdkClient::SetupParams>
{
  /**
   * Convert a JSON object to a SetupParams.
   *
   * @param jsonFrom The JSON object with which to fill the SetupParams.
   * @param params   The SetupParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hedera::TCK::SdkClient::SetupParams& params)
  {
    params.operatorAccountId = Hedera::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "operatorAccountId");
    params.operatorPrivateKey = Hedera::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "operatorPrivateKey");
    params.nodeIp = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "nodeIp");
    params.nodeAccountId = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "nodeAccountId");
    params.mirrorNetworkIp = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "mirrorNetworkIp");
  }
};

} // namespace nlohmann

#endif // HEDERA_TCK_CPP_SETUP_PARAMS_H_
