// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_KEY_SERVICE_H_
#define HIERO_TCK_CPP_KEY_SERVICE_H_

#include <Key.h>

#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>

namespace Hiero::TCK::KeyService
{
/**
 * Forward declarations.
 */
struct GenerateKeyParams;

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
std::shared_ptr<Key> getHieroKey(const std::string& key);

} // namespace Hiero::TCK::KeyService

#endif // HIERO_TCK_CPP_KEY_SERVICE_H_
