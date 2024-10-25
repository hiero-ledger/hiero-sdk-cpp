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
#ifndef HEDERA_TCK_CPP_GENERATE_KEY_PARAMS_H_
#define HEDERA_TCK_CPP_GENERATE_KEY_PARAMS_H_

#include "key/KeyService.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <vector>

namespace Hedera::TCK::KeyService
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

} // namespace Hedera::TCK::KeyService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert GenerateKeyParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hedera::TCK::KeyService::GenerateKeyParams>
{
  /**
   * Convert a JSON object to a GenerateKeyParams.
   *
   * @param jsonFrom The JSON object with which to fill the GenerateKeyParams.
   * @param params   The GenerateKeyParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hedera::TCK::KeyService::GenerateKeyParams& params)
  {
    params.mType = Hedera::TCK::KeyService::gStringToKeyType.at(
      Hedera::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "type"));
    params.mFromKey = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "fromKey");
    params.mThreshold = Hedera::TCK::getOptionalJsonParameter<int>(jsonFrom, "threshold");
    params.mKeys =
      Hedera::TCK::getOptionalJsonParameter<std::vector<Hedera::TCK::KeyService::GenerateKeyParams>>(jsonFrom, "keys");
  }
};

} // namespace nlohmann

#endif // HEDERA_TCK_CPP_GENERATE_KEY_PARAMS_H_
