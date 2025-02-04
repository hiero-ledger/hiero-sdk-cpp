// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ECDSA_SECP256K1_PRIVATE_KEY_H_
#define HIERO_SDK_CPP_ECDSA_SECP256K1_PRIVATE_KEY_H_

#include "PrivateKey.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Hiero::internal::OpenSSLUtils
{
class EVP_PKEY;
}

namespace Hiero
{
/**
 * A class representing an ECDSA secp256k1 private key.
 */
class ECDSAsecp256k1PrivateKey : public PrivateKey
{
public:
  /**
   * The number of bytes in an ECDSAsecp256k1PrivateKey.
   */
  static constexpr const size_t KEY_SIZE = 32ULL;

  /**
   * The prefix bytes of a DER-encoded ECDSAsecp256k1PrivateKey.
   */
  static inline const std::vector<std::byte> DER_ENCODED_PREFIX_BYTES = {
    std::byte(0x30), std::byte(0x30), std::byte(0x02), std::byte(0x01), std::byte(0x00), std::byte(0x30),
    std::byte(0x07), std::byte(0x06), std::byte(0x05), std::byte(0x2B), std::byte(0x81), std::byte(0x04),
    std::byte(0x00), std::byte(0x0A), std::byte(0x04), std::byte(0x22), std::byte(0x04), std::byte(0x20)
  };

  /**
   * The hex-encoded string of the DER-encoded prefix bytes of an ECDSAsecp256k1PrivateKey.
   */
  static inline const std::string DER_ENCODED_PREFIX_HEX = "3030020100300706052B8104000A04220420";

  /**
   * The maximum number of bytes in an ECDSAsecp256k1PrivateKey signature.
   */
  static constexpr const size_t MAX_SIGNATURE_SIZE = 72ULL;

  /**
   * The number of bytes in a raw ECDSAsecp256k1PrivateKey signature ((r,s) form).
   */
  static constexpr const size_t RAW_SIGNATURE_SIZE = 64ULL;

  /**
   * The number of bytes in the r value of an ECDSAsecp256k1PrivateKey signature.
   */
  static constexpr const size_t R_SIZE = 32ULL;

  /**
   * The number of bytes in the s value of an ECDSAsecp256k1PrivateKey signature.
   */
  static constexpr const size_t S_SIZE = 32ULL;

  /**
   * Disallow default construction of an ECDSAsecp256k1PrivateKey, as an uninitialized ECDSAsecp256k1PrivateKey provides
   * no functionality. Instead, a factory function should be used.
   */
  ECDSAsecp256k1PrivateKey() = delete;

  /**
   * Generate a new ECDSAsecp256k1PrivateKey.
   *
   * @return A pointer to the generated ECDSAsecp256k1PrivateKey.
   * @throws OpenSSLException If OpenSSL fails to generate a key.
   */
  [[nodiscard]] static std::unique_ptr<ECDSAsecp256k1PrivateKey> generatePrivateKey();

  /**
   * Construct an ECDSAsecp256k1PrivateKey object from a hex-encoded string (DER-encoded or raw).
   *
   * @param key The hex string from which to construct an ECDSAsecp256k1PrivateKey.
   * @return A pointer to an ECDSAsecp256k1PrivateKey representing the input hex string.
   * @throws BadKeyException If an ECDSAsecp256k1PrivateKey cannot be realized from the input hex string.
   */
  [[nodiscard]] static std::unique_ptr<ECDSAsecp256k1PrivateKey> fromString(std::string_view key);

  /**
   * Construct an ECDSAsecp256k1PrivateKey object from a byte vector (DER-encoded or raw).
   *
   * @param bytes The vector of bytes from which to construct an ECDSAsecp256k1PrivateKey.
   * @return A pointer to an ECDSAsecp256k1PrivateKey representing the input bytes.
   * @throws BadKeyException If an ECDSAsecp256k1PrivateKey cannot be realized from the input bytes.
   */
  [[nodiscard]] static std::unique_ptr<ECDSAsecp256k1PrivateKey> fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct an ECDSAsecp256k1PrivateKey from a seed array.
   *
   * @param seed The seed byte array from which to construct an ECDSAsecp256k1PrivateKey.
   * @return A pointer to an ECDSAsecp256k1PrivateKey representing the input seed bytes.
   * @throws BadKeyException If an ECDSAsecp256k1PrivateKey cannot be realized from the input seed bytes.
   */
  [[nodiscard]] static std::unique_ptr<ECDSAsecp256k1PrivateKey> fromSeed(const std::vector<std::byte>& seed);

  /**
   * Derived from Key. Create a clone of this ECDSAsecp256k1PrivateKey object.
   *
   * @return A pointer to the created clone of this ECDSAsecp256k1PrivateKey.
   */
  [[nodiscard]] std::unique_ptr<Key> clone() const override;

  /**
   * Derived from Key. Construct a Key protobuf object from this ECDSAsecp256k1PrivateKey object's
   * ECDSAsecp256k1PublicKey.
   *
   * @return A pointer to a created Key protobuf object filled with this ECDSAsecp256k1PrivateKey object's
   *         ECDSAsecp256k1PublicKey data.
   * @throws OpenSSLException If OpenSSL is unable to serialize this ECDSAsecp256k1PrivateKey object's
   *                          ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::unique_ptr<proto::Key> toProtobufKey() const override;

  /**
   * Derived from PrivateKey. Derive a child ECDSAsecp256k1PrivateKey from this ECDSAsecp256k1PrivateKey.
   *
   * @param childIndex The index of the child to derive.
   * @return A pointer to the derived ECDSAsecp256k1PrivateKey child.
   * @throws OpenSSLException       If OpenSSL is unable to derive a key with the given childIndex.
   * @throws UninitializedException If this ECDSAsecp256k1PrivateKey was not initialized with a chain code.
   */
  [[nodiscard]] std::unique_ptr<PrivateKey> derive(uint32_t childIndex) const override;

  /**
   * Derived from PrivateKey. Sign an arbitrary byte array.
   *
   * @param bytesToSign The bytes to sign.
   * @return The signature of the byte array.
   * @throws OpenSSLException If OpenSSL is unable to generate a signature.
   */
  [[nodiscard]] std::vector<std::byte> sign(const std::vector<std::byte>& bytesToSign) const override;

  /**
   * Derived from PrivateKey. Get the hex-encoded string of the DER-encoded bytes of this ECDSAsecp256k1PrivateKey.
   *
   * @return The hex-encoded string of the DER-encoded bytes of this ECDSAsecp256k1PrivateKey.
   */
  [[nodiscard]] std::string toStringDer() const override;

  /**
   * Derived from PrivateKey. Get the hex-encoded string of the raw, non-DER-encoded bytes of this
   * ECDSAsecp256k1PrivateKey.
   *
   * @return The hex-encoded string of the raw bytes of this ECDSAsecp256k1PrivateKey.
   */
  [[nodiscard]] std::string toStringRaw() const override;

  /**
   * Derived from Key. Get the byte representation of this ECDSAsecp256k1PrivateKey. Returns the same result as
   * toBytesDer().
   *
   * @return The DER-encoded bytes of this ECDSAsecp256k1PrivateKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const override;

  /**
   * Derived from PrivateKey. Get the DER-encoded bytes of this ECDSAsecp256k1PrivateKey.
   *
   * @return The DER-encoded bytes of this ECDSAsecp256k1PrivateKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesDer() const override;

  /**
   * Derived from PrivateKey. Get the raw, non-DER-encoded bytes of this ECDSAsecp256k1PrivateKey.
   *
   * @return The raw bytes of this ECDSAsecp256k1PrivateKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesRaw() const override;

private:
  /**
   * Construct from a wrapped OpenSSL key object and optionally a chain code.
   *
   * @param key       The wrapped OpenSSL key object.
   * @param chainCode The chain code.
   * @throws OpenSSLException If OpenSSL is unable to get this ECDSAsecp256k1PrivateKey's corresponding
   *                          ECDSAsecp256k1PrivateKey's bytes.
   * @throws BadKeyException  If the chain code is malformed.
   */
  explicit ECDSAsecp256k1PrivateKey(internal::OpenSSLUtils::EVP_PKEY&& key,
                                    std::vector<std::byte> chainCode = std::vector<std::byte>());
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ECDSA_SECP256K1_PRIVATE_KEY_H_
