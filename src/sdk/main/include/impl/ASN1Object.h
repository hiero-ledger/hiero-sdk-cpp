// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_ASN1_OBJECT_H_
#define HIERO_SDK_CPP_IMPL_ASN1_OBJECT_H_

#include <iostream>
#include <unordered_map>
#include <vector>

namespace Hiero::internal::asn1
{

/**
 * Constants for ASN.1 standard.
 */
constexpr std::byte INTEGER = std::byte(0x02);
constexpr std::byte BIT_STRING = std::byte(0x03);
constexpr std::byte OCTET_STRING = std::byte(0x04);
constexpr std::byte OBJECT_IDENTIFIER = std::byte(0x06);
constexpr std::byte SEQUENCE = std::byte(0x30);

/**
 * @class ASN1Object
 * Abstract base class for ASN.1 objects.
 */
class ASN1Object
{
protected:
  /**
   * Get the ASN.1 object's value in bytes.
   *
   * @param tag The ASN.1 tag of the object.
   * @return The object's value in bytes.
   */
  virtual const std::vector<std::byte> get(const std::byte tag) const = 0;

  /**
   * Decode the ASN.1 object from a vector of bytes.
   *
   * @param data The vector of bytes containing the ASN.1 object's data.
   */
  virtual void decode(const std::vector<std::byte>& data) = 0;
};

} // namespace Hiero::internal::asn1

#endif // HIERO_SDK_CPP_IMPL_ASN1_OBJECT_H_