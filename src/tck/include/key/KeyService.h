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
#ifndef HEDERA_TCK_CPP_KEY_SERVICE_H_
#define HEDERA_TCK_CPP_KEY_SERVICE_H_

#include <Key.h>

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>

namespace Hedera::TCK::KeyService
{
/**
 * Forward declarations.
 */
class GenerateKeyParams;

/**
 * Enumeration of the possible types of Keys the SDK server can generate.
 */
enum class KeyType : unsigned char
{
  ED25519_PRIVATE_KEY_TYPE,
  ED25519_PUBLIC_KEY_TYPE,
  ECDSA_SECP256k1_PRIVATE_KEY_TYPE,
  ECDSA_SECP256k1_PUBLIC_KEY_TYPE,
  LIST_KEY_TYPE,
  THRESHOLD_KEY_TYPE,
  EVM_ADDRESS_KEY_TYPE,
  KEY_TYPE_SIZE [[maybe_unused]]
};

/**
 * Map of KeyTypes to their string representation.
 */
extern const std::unordered_map<std::string, KeyType> gStringToKeyType;

/**
 * Map of KeyType string representations to KeyType.
 */
extern const std::unordered_map<KeyType, std::string> gKeyTypeToString;

/**
 * Generate a Key.
 *
 * @param params The parameters to use to generate a key.
 * @return The JSON object which contains the generated key.
 */
nlohmann::json generateKey(const GenerateKeyParams& params);

/**
 * Generate a Key from a key hex string. The string must be either the DER-encoding of an ED25519 or ECDSAsecp256k1
 * private or public key, or the serialized Key protobuf of a KeyList or ThresholdKey.
 *
 * @param key The hex string from which to get the Key.
 * @return The Key of the input hex string.
 */
std::shared_ptr<Key> getHederaKey(const std::string& key);

} // namespace Hedera::TCK::KeyService

#endif // HEDERA_TCK_CPP_KEY_SERVICE_H_
