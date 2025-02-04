// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_ASN1_EC_PUBLIC_KEY_H_
#define HIERO_SDK_CPP_IMPL_ASN1_EC_PUBLIC_KEY_H_

#include "ASN1ECKey.h"

namespace Hiero::internal::asn1
{
/**
 * @brief The ASN.1 algorithm identifier prefix bytes for a compressed
 * ECDSA secp256k1 public key.
 * @details
 * Byte sequence: 3036301006072A8648CE3D020106052B8104000A0322
 */
const std::vector<std::byte> ASN1_CPUBK_PREFIX_BYTES = {
  std::byte(0x30), std::byte(0x36), std::byte(0x30), std::byte(0x10), std::byte(0x06), std::byte(0x07),
  std::byte(0x2A), std::byte(0x86), std::byte(0x48), std::byte(0xCE), std::byte(0x3D), std::byte(0x02),
  std::byte(0x01), std::byte(0x06), std::byte(0x05), std::byte(0x2B), std::byte(0x81), std::byte(0x04),
  std::byte(0x00), std::byte(0x0A), std::byte(0x03), std::byte(0x22)
};

/**
 * @brief The ASN.1 algorithm identifier prefix bytes for an uncompressed
 * ECDSA secp256k1 public key.
 * @details
 * Byte sequence: 3056301006072A8648CE3D020106052B8104000A0342
 */
const std::vector<std::byte> ASN1_UPUBK_PREFIX_BYTES = {
  std::byte(0x30), std::byte(0x56), std::byte(0x30), std::byte(0x10), std::byte(0x06), std::byte(0x07),
  std::byte(0x2A), std::byte(0x86), std::byte(0x48), std::byte(0xCE), std::byte(0x3D), std::byte(0x02),
  std::byte(0x01), std::byte(0x06), std::byte(0x05), std::byte(0x2B), std::byte(0x81), std::byte(0x04),
  std::byte(0x00), std::byte(0x0A), std::byte(0x03), std::byte(0x42)
};

// PEM Format prefix/suffix string
constexpr std::string_view PEM_ECPUBK_PREFIX_STRING = "-----BEGIN EC PUBLIC KEY-----";
constexpr std::string_view PEM_ECPUBK_SUFFIX_STRING = "-----END EC PUBLIC KEY-----";

/**
 * @class ASN1ECPublicKey
 * @brief ASN.1 elliptic curve public key object.
 */
class ASN1ECPublicKey : public ASN1ECKey
{
public:
  /**
   * @brief Constructor for ASN.1 elliptic curve public key from a vector of bytes.
   * @param bytes The vector of bytes containing the ASN.1 key data.
   */
  ASN1ECPublicKey(const std::vector<std::byte>& bytes);

  /**
   * @brief Get the key value associated with the ASN.1 elliptic curve public key.
   * @return The key as a vector of bytes.
   */
  std::vector<std::byte> getKey() const override;

private:
  /**
   * @brief Constructor for ASN.1 elliptic curve public key.
   */
  ASN1ECPublicKey() = default;

  /**
   * @brief Populate X and Y coordinates from the ASN.1 key bytes.
   */
  void populateXYcoords();

  std::vector<std::byte> ecXcoord; ///< X coordinate of the elliptic curve public key.
  std::vector<std::byte> ecYcoord; ///< Y coordinate of the elliptic curve public key.
};

} // namespace Hiero::internal:asn1

#endif // HIERO_SDK_CPP_IMPL_ASN1_EC_PUBLIC_KEY_H_
