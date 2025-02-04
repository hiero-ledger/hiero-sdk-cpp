// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_SETUP_PARAMS_H_
#define HIERO_TCK_CPP_SETUP_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::SdkClient
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

} // namespace Hiero::TCK::SdkClient

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert SetupParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::SdkClient::SetupParams>
{
  /**
   * Convert a JSON object to a SetupParams.
   *
   * @param jsonFrom The JSON object with which to fill the SetupParams.
   * @param params   The SetupParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::SdkClient::SetupParams& params)
  {
    params.operatorAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "operatorAccountId");
    params.operatorPrivateKey = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "operatorPrivateKey");
    params.nodeIp = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "nodeIp");
    params.nodeAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "nodeAccountId");
    params.mirrorNetworkIp = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "mirrorNetworkIp");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_SETUP_PARAMS_H_
