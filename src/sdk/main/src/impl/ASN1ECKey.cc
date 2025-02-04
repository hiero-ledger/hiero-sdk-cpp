// SPDX-License-Identifier: Apache-2.0

#include "exceptions/BadKeyException.h"

#include "impl/ASN1ECKey.h"
#include "impl/HexConverter.h"

namespace Hiero::internal::asn1
{
void ASN1ECKey::decode(const std::vector<std::byte>& bytes)
{
  int currentByteIndex = 0;
  while (currentByteIndex < bytes.size() - 1)
  {
    const std::byte asn1Tag = bytes[currentByteIndex++];
    const int asn1TagSize = static_cast<int>(bytes[currentByteIndex++]);

    if (currentByteIndex + asn1TagSize > bytes.size())
    {
      throw BadKeyException("Bad PEM/DER EC KEY bytes data!");
    }
    // Ignore sequence as ASN1 for EC Key is in basic format
    if (asn1Tag != SEQUENCE)
    {
      std::vector<std::byte> asn1DataAtTag(bytes.begin() + currentByteIndex,
                                           bytes.begin() + currentByteIndex + asn1TagSize);
      currentByteIndex += asn1TagSize;

      asn1KeyData.insert({ asn1Tag, asn1DataAtTag });
    }
  }
}

const std::vector<std::byte> ASN1ECKey::get(const std::byte tag) const
{
  auto entry = asn1KeyData.find(tag);
  if (entry != asn1KeyData.end())
    return entry->second;
  else
    throw BadKeyException("Data not decoded properly for input PEM/DER EC KEY bytes!");
}

} // namespace Hiero::internal:asn1
