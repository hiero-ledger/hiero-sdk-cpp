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
#include "key/KeyService.h"
#include "key/params/GenerateKeyParams.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"

#include <ECDSAsecp256k1PrivateKey.h>
#include <ECDSAsecp256k1PublicKey.h>
#include <ED25519PrivateKey.h>
#include <EvmAddress.h>
#include <Key.h>
#include <KeyList.h>
#include <PrivateKey.h>
#include <PublicKey.h>
#include <exceptions/BadKeyException.h>
#include <impl/HexConverter.h>
#include <impl/Utilities.h>

#include <algorithm>
#include <memory>
#include <nlohmann/json.hpp>
#include <proto/basic_types.pb.h>
#include <string>

namespace Hedera::TCK::KeyService
{
namespace
{
// Process a GenerateKeyParams and return the generated key. For ED25519 or ECDSAsecp256k1 private or public key types,
// this will be the DER-encoding of the key. For KeyList of ThresholdKey types, this will be the serialized Key protobuf
// of the key, as well as the private keys contained in the list.
//
// @param params   The GenerateKeyParams to process.
// @param response The JSON object to possibly fill with a list of private keys for KeyList and ThresholdKey types.
// @param isList   If the current GenerateKeyParams is a KeyList or ThresholdKey type and should add private keys to the
//                 response.
// @return The hex encoding of the generated key.
std::string generateKeyRecursively(const GenerateKeyParams& params, nlohmann::json& response, bool isList = false)
{
  // Make sure fromKey is only provided for ED25519_PUBLIC_KEY_TYPE, ECDSA_SECP256k1_PUBLIC_KEY_TYPE, or
  // EVM_ADDRESS_KEY_TYPE.
  if (params.mFromKey.has_value() && params.mType != KeyType::ED25519_PUBLIC_KEY_TYPE &&
      params.mType != KeyType::ECDSA_SECP256k1_PUBLIC_KEY_TYPE && params.mType != KeyType::EVM_ADDRESS_KEY_TYPE)
  {
    throw JsonRpcException(JsonErrorType::INVALID_PARAMS,
                           "invalid parameters: fromKey should only be provided for ed25519PublicKey, "
                           "ecdsaSecp256k1PublicKey, or evmAddress types.");
  }

  // Make sure threshold is only provided for THRESHOLD_KEY_TYPE.
  if (params.mThreshold.has_value() && params.mType != KeyType::THRESHOLD_KEY_TYPE)
  {
    throw JsonRpcException(JsonErrorType::INVALID_PARAMS,
                           "invalid parameters: threshold should only be provided for thresholdKey types.");
  }

  // Make sure keys is only provided for LIST_KEY_TYPE or THRESHOLD_KEY_TYPE.
  if (params.mKeys.has_value() && params.mType != KeyType::LIST_KEY_TYPE && params.mType != KeyType::THRESHOLD_KEY_TYPE)
  {
    throw JsonRpcException(JsonErrorType::INVALID_PARAMS,
                           "invalid parameters: keys should only be provided for keyList or thresholdKey types.");
  }

  switch (params.mType)
  {
    case KeyType::ED25519_PRIVATE_KEY_TYPE:
    case KeyType::ECDSA_SECP256k1_PRIVATE_KEY_TYPE:
    {
      const std::string key = params.mType == KeyType::ED25519_PUBLIC_KEY_TYPE
                                ? ED25519PrivateKey::generatePrivateKey()->toStringDer()
                                : ECDSAsecp256k1PrivateKey::generatePrivateKey()->toStringDer();
      if (isList)
      {
        response["privateKeys"].push_back(key);
      }

      return key;
    }

    case KeyType::ED25519_PUBLIC_KEY_TYPE:
    case KeyType::ECDSA_SECP256k1_PUBLIC_KEY_TYPE:
    {
      if (params.mFromKey.has_value())
      {
        return PrivateKey::fromStringDer(params.mFromKey.value())->getPublicKey()->toStringDer();
      }

      const std::unique_ptr<PrivateKey> key =
        params.mType == KeyType::ED25519_PUBLIC_KEY_TYPE
          ? static_cast<std::unique_ptr<PrivateKey>>(ED25519PrivateKey::generatePrivateKey())
          : static_cast<std::unique_ptr<PrivateKey>>(ECDSAsecp256k1PrivateKey::generatePrivateKey());
      if (isList)
      {
        response["privateKeys"].push_back(key->toStringDer());
      }

      return key->getPublicKey()->toStringDer();
    }

    case KeyType::LIST_KEY_TYPE:
    case KeyType::THRESHOLD_KEY_TYPE:
    {
      if (!params.mKeys.has_value())
      {
        throw JsonRpcException(JsonErrorType::INVALID_REQUEST,
                               "invalid request: keys list is required for generating a KeyList type.");
      }

      const bool isThreshold = params.mType == KeyType::THRESHOLD_KEY_TYPE;
      if (isThreshold && !params.mThreshold.has_value())
      {
        throw JsonRpcException(JsonErrorType::INVALID_REQUEST,
                               "invalid request: threshold is required for generating a ThresholdKey type.");
      }

      KeyList keyList;
      std::for_each(params.mKeys->cbegin(),
                    params.mKeys->cend(),
                    [&keyList, &response](const GenerateKeyParams& params)
                    { keyList.push_back(getHederaKey(generateKeyRecursively(params, response, true))); });

      if (isThreshold)
      {
        keyList.setThreshold(params.mThreshold.value());
      }

      return internal::HexConverter::bytesToHex(
        internal::Utilities::stringToByteVector(keyList.toProtobufKey()->SerializeAsString()));
    }

    case KeyType::EVM_ADDRESS_KEY_TYPE:
    {
      if (params.mFromKey.has_value())
      {
        const std::shared_ptr<Key> key = getHederaKey(params.mFromKey.value());
        if (const std::shared_ptr<ECDSAsecp256k1PrivateKey> privateKey =
              std::dynamic_pointer_cast<ECDSAsecp256k1PrivateKey>(key);
            privateKey)
        {
          return std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(privateKey->getPublicKey())
            ->toEvmAddress()
            .toString();
        }

        if (const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
              std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(key);
            publicKey)
        {
          return publicKey->toEvmAddress().toString();
        }

        throw JsonRpcException(JsonErrorType::INVALID_PARAMS,
                               "invalid parameters: fromKey for evmAddress is not ECDSAsecp256k1.");
      }

      return std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(
               ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey())
        ->toEvmAddress()
        .toString();
    }

    default:
    {
      throw JsonRpcException(JsonErrorType::INVALID_REQUEST, "invalid request: key type not recognized.");
    }
  }
}

} // namespace

//-----
const std::unordered_map<std::string, KeyType> gStringToKeyType = {
  {"ed25519PrivateKey",         KeyType::ED25519_PRIVATE_KEY_TYPE        },
  { "ed25519PublicKey",         KeyType::ED25519_PUBLIC_KEY_TYPE         },
  { "ecdsaSecp256k1PrivateKey", KeyType::ECDSA_SECP256k1_PRIVATE_KEY_TYPE},
  { "ecdsaSecp256k1PublicKey",  KeyType::ECDSA_SECP256k1_PUBLIC_KEY_TYPE },
  { "keyList",                  KeyType::LIST_KEY_TYPE                   },
  { "thresholdKey",             KeyType::THRESHOLD_KEY_TYPE              },
  { "evmAddress",               KeyType::EVM_ADDRESS_KEY_TYPE            }
};

//-----
const std::unordered_map<KeyType, std::string> gKeyTypeToString = {
  {KeyType::ED25519_PRIVATE_KEY_TYPE,          "ed25519PrivateKey"       },
  { KeyType::ED25519_PUBLIC_KEY_TYPE,          "ed25519PublicKey"        },
  { KeyType::ECDSA_SECP256k1_PRIVATE_KEY_TYPE, "ecdsaSecp256k1PrivateKey"},
  { KeyType::ECDSA_SECP256k1_PUBLIC_KEY_TYPE,  "ecdsaSecp256k1PublicKey" },
  { KeyType::LIST_KEY_TYPE,                    "keyList"                 },
  { KeyType::THRESHOLD_KEY_TYPE,               "thresholdKey"            },
  { KeyType::EVM_ADDRESS_KEY_TYPE,             "evmAddress"              }
};

//-----
nlohmann::json generateKey(const GenerateKeyParams& params)
{
  nlohmann::json response;
  response["key"] = generateKeyRecursively(params, response);
  return response;
}

//-----
std::shared_ptr<Key> getHederaKey(const std::string& key)
{
  try
  {
    return PublicKey::fromStringDer(key);
  }
  catch (const BadKeyException&)
  {
    try
    {
      return PrivateKey::fromStringDer(key);
    }
    catch (const BadKeyException&)
    {
      proto::Key protoKey;
      protoKey.ParseFromString(internal::Utilities::byteVectorToString(internal::HexConverter::hexToBytes(key)));
      return Hedera::Key::fromProtobuf(protoKey);
    }
  }
}

} // namespace Hedera::TCK
