// SPDX-License-Identifier: Apache-2.0

#include "exceptions/BadKeyException.h"

#include "impl/ASN1ED25519PublicKey.h"
#include "impl/HexConverter.h"

namespace Hiero::internal::asn1
{
ASN1ED25519PublicKey::ASN1ED25519PublicKey(const std::vector<std::byte>& bytes)
{
  if (bytes.size() >= MAX_ENCRYPTED_KEY_LENGHT)
  {
    throw BadKeyException("Over maximum possible input bytes for EC Key!");
  }
  else
  {
    decode(bytes);
  }
}

std::vector<std::byte> ASN1ED25519PublicKey::getKey() const
{
  std::vector<std::byte> publicKey = get(BIT_STRING);
  if (publicKey.size() < EC_KEY_LENGTH)
  {
    throw BadKeyException("Data not decoded properly for input PEM/DER EC KEY bytes!");
  }
  if (publicKey.size() > EC_KEY_LENGTH) // remove redundant padded bytes if any
  {
    publicKey = std::vector<std::byte>(publicKey.end() - EC_KEY_LENGTH, publicKey.end());
  }

  return publicKey;
}

} // namespace Hiero::internal:asn1
