// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_ASN1_EC_PRIVATE_KEY_H_
#define HIERO_SDK_CPP_IMPL_ASN1_EC_PRIVATE_KEY_H_

#include "ASN1ECKey.h"

namespace Hiero::internal::asn1
{
// The ASN.1 algorithm identifier prefix bytes for an ECDSAsecp256k1PrivateKey.
const std::vector<std::byte> ASN1_PRK_PREFIX_BYTES = { std::byte(0x30), std::byte(0x2E), std::byte(0x02),
                                                       std::byte(0x01), std::byte(0x01), std::byte(0x04),
                                                       std::byte(0x20) };

// The ASN.1 algorithm identifier suffix bytes for an ECDSAsecp256k1KeyPrivateKey.
const std::vector<std::byte> ASN1_PRK_SUFFIX_BYTES = { std::byte(0xA0), std::byte(0x07), std::byte(0x06),
                                                       std::byte(0x05), std::byte(0x2B), std::byte(0x81),
                                                       std::byte(0x04), std::byte(0x00), std::byte(0x0A) };

// PEM Format prefix/suffix string ECDSA
constexpr std::string_view PEM_ECPRK_PREFIX_STRING = "-----BEGIN EC PRIVATE KEY-----";
constexpr std::string_view PEM_ECPRK_SUFFIX_STRING = "-----END EC PRIVATE KEY-----";

/**
 * @class ASN1Key
 * @brief ASN.1 key object.
 */
class ASN1ECPrivateKey : public ASN1ECKey
{
public:
  /**
   * @brief Constructor for ASN.1 key from a vector of bytes.
   *
   * @param bytes The vector of bytes containing the ASN.1 key data.
   */
  ASN1ECPrivateKey(const std::vector<std::byte>& bytes);

  /**
   * @brief Get the key value associated with the ASN.1 key.
   *
   * @return The key as a vector of bytes.
   */
  std::vector<std::byte> getKey() const override;

private:
  /**
   * @brief Constructor for ASN.1 key.
   */
  ASN1ECPrivateKey() = default;
};

} // namespace Hiero::internal:asn1

#endif // HIERO_SDK_CPP_IMPL_ASN1_EC_PRIVATE_KEY_H_
