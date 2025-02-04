// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GENERATE_KEY_PARAMS_H_
#define HIERO_TCK_CPP_GENERATE_KEY_PARAMS_H_

#include "key/KeyService.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <vector>

namespace Hiero::TCK::KeyService
{
/**
 * Struct to hold the arguments for a `generateKey` JSON-RPC method call.
 */
struct GenerateKeyParams
{
  /**
   *The type of Key to generate.
   */
  KeyType mType = KeyType::KEY_TYPE_SIZE;

  /**
   * For ED25519Public and ECDSAsecp256k1Public types, the DER-encoded hex string private key from which to generate the
   * public key. No value means a random ED25519Public or ECDSAsecp256k1Public will be generated, respectively. For
   * EvmAddress, the DER-encoded hex string of an ECDSAsecp256k1Private or ECDSAsecp256k1Public from which to generate
   * the EVM address. An ECDSAsecp256k1Private will first generate its respective ECDSAsecp256k1Public, and then
   * generate the EVM address from that public key. No value means a random EVM address will be generated.
   */
  std::optional<std::string> mFromKey;

  /**
   * Required for Threshold types. The number of keys that must sign for a threshold key.
   */
  std::optional<int> mThreshold;

  /**
   * Required for List and Threshold types. Specify the keys to be generated and put in the List or Threshold.
   */
  std::optional<std::vector<GenerateKeyParams>> mKeys;
};

} // namespace Hiero::TCK::KeyService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert GenerateKeyParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::KeyService::GenerateKeyParams>
{
  /**
   * Convert a JSON object to a GenerateKeyParams.
   *
   * @param jsonFrom The JSON object with which to fill the GenerateKeyParams.
   * @param params   The GenerateKeyParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::KeyService::GenerateKeyParams& params)
  {
    params.mType =
      Hiero::TCK::KeyService::gStringToKeyType.at(Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "type"));
    params.mFromKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "fromKey");
    params.mThreshold = Hiero::TCK::getOptionalJsonParameter<int>(jsonFrom, "threshold");
    params.mKeys =
      Hiero::TCK::getOptionalJsonParameter<std::vector<Hiero::TCK::KeyService::GenerateKeyParams>>(jsonFrom, "keys");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_GENERATE_KEY_PARAMS_H_
