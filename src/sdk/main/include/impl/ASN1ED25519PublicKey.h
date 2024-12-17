// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_ASN1_ED25519_PUBLIC_KEY_H_
#define HIERO_SDK_CPP_IMPL_ASN1_ED25519_PUBLIC_KEY_H_

#include "ASN1ECKey.h"

namespace Hiero::internal::asn1
{
/**
 * The prefix bytes of a DER-encoded ED25519PublicKey.
 */
static inline const std::vector<std::byte> ASN1_EDPBK_PREFIX_BYTES = {
  std::byte(0x30), std::byte(0x2A), std::byte(0x30), std::byte(0x05), std::byte(0x06), std::byte(0x03),
  std::byte(0x2B), std::byte(0x65), std::byte(0x70), std::byte(0x03), std::byte(0x21), std::byte(0x00)
};

// PEM Format prefix/suffix string
constexpr std::string_view PEM_ECPBK_PREFIX_STRING = "-----BEGIN PUBLIC KEY-----";
constexpr std::string_view PEM_ECPBK_SUFFIX_STRING = "-----END PUBLIC KEY-----";

/**
 * @class ASN1ED25519PublicKey
 * @brief ASN.1 elliptic curve public key object.
 */
class ASN1ED25519PublicKey : public ASN1ECKey
{
public:
  /**
   * @brief Constructor for ASN.1 elliptic curve public key from a vector of bytes.
   * @param bytes The vector of bytes containing the ASN.1 key data.
   */
  ASN1ED25519PublicKey(const std::vector<std::byte>& bytes);

  /**
   * @brief Get the key value associated with the ASN.1 elliptic curve public key.
   * @return The key as a vector of bytes.
   */
  std::vector<std::byte> getKey() const override;

private:
  /**
   * @brief Constructor for ASN.1 elliptic curve public key.
   */
  ASN1ED25519PublicKey() = default;
};

} // namespace Hiero::internal:asn1

#endif // HIERO_SDK_CPP_IMPL_ASN1_ED25519_PUBLIC_KEY_H_
