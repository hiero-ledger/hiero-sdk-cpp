// SPDX-License-Identifier: Apache-2.0

#include "exceptions/BadKeyException.h"

#include "impl/ASN1ECPublicKey.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <openssl/bn.h>

#include <algorithm>

namespace Hiero::internal::asn1
{
ASN1ECPublicKey::ASN1ECPublicKey(const std::vector<std::byte>& bytes)
{
  if (bytes.size() >= MAX_ENCRYPTED_KEY_LENGHT)
  {
    throw BadKeyException("Over maximum possible input bytes for EC Key!");
  }
  else
  {
    decode(bytes);
    populateXYcoords();
  }
}

std::vector<std::byte> ASN1ECPublicKey::getKey() const
{
  if (ecXcoord.empty())
    throw BadKeyException("Data not decoded properly for input PEM/DER EC KEY bytes!");
  else
    return ecXcoord;
}

void ASN1ECPublicKey::populateXYcoords()
{
  std::vector<std::byte> publicKey = get(BIT_STRING);
  if (publicKey[0] == std::byte(0x00) && publicKey[1] == std::byte(0x04)) // uncompressed
  {
    ecYcoord = std::vector<std::byte>(publicKey.rbegin(), publicKey.rbegin() + EC_KEY_LENGTH); // get y coordinate
    std::reverse(ecYcoord.begin(), ecYcoord.end());

    BIGNUM* yCoordBN = BN_new();
    BN_hex2bn(&yCoordBN, internal::HexConverter::bytesToHex(ecYcoord).c_str());
    // if y even pad 0x02 byte else 0x03
    ecXcoord = internal::Utilities::concatenateVectors({
  // get x coordinate
      { std::byte(0x00) },
      { !BN_is_bit_set(yCoordBN, 0) ? std::byte(0x02) : std::byte(0x03) },
      { publicKey.begin() + 2, publicKey.end() - EC_KEY_LENGTH }
    });
  }

  if (publicKey[0] == std::byte(0x00) &&
      (publicKey[1] == std::byte(0x02) || publicKey[1] == std::byte(0x03))) // compressed
  {
    // Just populate x coord. Padding one to ignore the 00 starting byte.
    ecXcoord = std::vector<std::byte>(publicKey.begin(), publicKey.begin() + EC_KEY_LENGTH + 2);
  }
}

} // namespace Hiero::internal:asn1
