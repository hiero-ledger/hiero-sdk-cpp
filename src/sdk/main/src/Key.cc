// SPDX-License-Identifier: Apache-2.0
#include "Key.h"
#include "ContractId.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PublicKey.h"
#include "KeyList.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
std::unique_ptr<Key> Key::fromProtobuf(const proto::Key& key)
{
  switch (key.key_case())
  {
    case proto::Key::KeyCase::kContractID:
      return std::make_unique<ContractId>(ContractId::fromProtobuf(key.contractid()));
    case proto::Key::KeyCase::kEd25519:
      return ED25519PublicKey::fromBytes(internal::Utilities::stringToByteVector(key.ed25519()));
    case proto::Key::KeyCase::kECDSASecp256K1:
      return ECDSAsecp256k1PublicKey::fromBytes(internal::Utilities::stringToByteVector(key.ecdsa_secp256k1()));
    case proto::Key::KeyCase::kKeyList:
      return std::make_unique<KeyList>(KeyList::fromProtobuf(key.keylist()));
    case proto::Key::KeyCase::kThresholdKey:
      return std::make_unique<KeyList>(KeyList::fromProtobuf(key.thresholdkey()));
    default:
      throw std::invalid_argument("Key protobuf case not recognized");
  }
}

//-----
std::vector<std::byte> Key::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobufKey()->SerializeAsString());
}

} // namespace Hiero
