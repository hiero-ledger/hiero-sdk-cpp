// SPDX-License-Identifier: Apache-2.0

#include "exceptions/BadKeyException.h"

#include "impl/ASN1ED25519PrivateKey.h"
#include "impl/HexConverter.h"

namespace Hiero::internal::asn1
{
ASN1ED25519PrivateKey::ASN1ED25519PrivateKey(const std::vector<std::byte>& bytes)
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

std::vector<std::byte> ASN1ED25519PrivateKey::getKey() const
{
  std::vector<std::byte> privateKey = get(OCTET_STRING);
  if (privateKey.empty())
  {
    throw BadKeyException("Data not decoded properly for input PEM/DER EC KEY bytes!");
  }
  if (privateKey.size() > EC_KEY_LENGTH) // remove redundant padded bytes if any
  {
    privateKey = std::vector<std::byte>(privateKey.end() - EC_KEY_LENGTH, privateKey.end());
  }

  return privateKey;
}

} // namespace Hiero::internal:asn1
