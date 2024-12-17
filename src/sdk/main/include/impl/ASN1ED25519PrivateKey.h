// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_ASN1_ED25519_PRIVATE_KEY_H_
#define HIERO_SDK_CPP_IMPL_ASN1_ED25519_PRIVATE_KEY_H_

#include "ASN1ECKey.h"

namespace Hiero::internal::asn1
{
// The ASN.1 algorithm identifier prefix bytes for an EC ED25519PrivateKey
const std::vector<std::byte> ASN1_EDPRK_PREFIX_BYTES = {
  std::byte(0x30), std::byte(0x2E), std::byte(0x02), std::byte(0x01), std::byte(0x00), std::byte(0x30),
  std::byte(0x05), std::byte(0x06), std::byte(0x03), std::byte(0x2B), std::byte(0x65), std::byte(0x70),
  std::byte(0x04), std::byte(0x22), std::byte(0x04), std::byte(0x20)
};

// PEM Format prefix/suffix string EC ED25519 Key
constexpr std::string_view PEM_EDPRK_PREFIX_STRING = "-----BEGIN PRIVATE KEY-----";
constexpr std::string_view PEM_EDPRK_SUFFIX_STRING = "-----END PRIVATE KEY-----";

/**
 * @class ASN1Key
 * @brief ASN.1 key object.
 */
class ASN1ED25519PrivateKey : public ASN1ECKey
{
public:
  /**
   * @brief Constructor for ASN.1 key from a vector of bytes.
   *
   * @param bytes The vector of bytes containing the ASN.1 key data.
   */
  ASN1ED25519PrivateKey(const std::vector<std::byte>& bytes);

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
  ASN1ED25519PrivateKey() = default;
};

} // namespace Hiero::internal:asn1

#endif // HIERO_SDK_CPP_IMPL_ASN1_ED25519_PRIVATE_KEY_H_
