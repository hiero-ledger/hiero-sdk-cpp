// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_ASN1_EC_KEY_H_
#define HIERO_SDK_CPP_IMPL_ASN1_EC_KEY_H_

#include "ASN1Object.h"

namespace Hiero::internal::asn1
{
constexpr size_t EC_KEY_LENGTH = 32; // bytes
// more than this would be a malicious attempt
constexpr size_t MAX_ENCRYPTED_KEY_LENGHT = 160; // bytes ~ 320 characters

/**
 * @class ASN1Key
 * ASN.1 key object.
 */
class ASN1ECKey : public ASN1Object
{
public:
  /**
   * Get the key value associated with the ASN.1 key.
   *
   * @return The key as a vector of bytes.
   */
  virtual std::vector<std::byte> getKey() const = 0;

protected:
  /**
   * Decode ASN.1 data representing an Elliptic Curve Key.
   *
   * This method decodes basic ASN.1 data, extracting key data and storing it in the `asn1KeyData` map.
   * EC Keys in ASN1 format always follow a common structure:
   *
   *   ECKey ::= SEQUENCE {
   *   version INTEGER { ecPrivkeyVer1(1) } (ecPrivkeyVer1) OPTIONAL,
   *   key STRING_DATA,
   *   parameters [0] ECParameters {{ NamedCurve }} OPTIONAL,
   *   otherKey [1] STRING_DATA OPTIONAL
   *   }
   *
   * @param bytes The ASN.1-encoded data representing the Elliptic Curve Key.
   */
  virtual void decode(const std::vector<std::byte>& bytes);

  /**
   * Get the value associated with the given ASN.1 tag.
   *
   * @param tag The ASN.1 tag.
   * @return The value associated with the tag as a vector of bytes.
   */
  virtual const std::vector<std::byte> get(const std::byte tag) const;

  /**
   * A map to store ASN.1 key data with their associated tags.
   */
  std::unordered_map<std::byte, std::vector<std::byte>> asn1KeyData;
};

} // namespace Hiero::internal:asn1

#endif // HIERO_SDK_CPP_IMPL_ASN1_EC_KEY_H_
