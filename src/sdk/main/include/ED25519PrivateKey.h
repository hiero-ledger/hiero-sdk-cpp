// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ED25519_PRIVATE_KEY_H_
#define HIERO_SDK_CPP_ED25519_PRIVATE_KEY_H_

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
 * A class representing an ED25519 private key.
 */
class ED25519PrivateKey : public PrivateKey
{
public:
  /**
   * The number of bytes in an ED25519PrivateKey.
   */
  static constexpr const size_t KEY_SIZE = 32ULL;

  /**
   * The prefix bytes of a DER-encoded ED25519PrivateKey.
   */
  static inline const std::vector<std::byte> DER_ENCODED_PREFIX_BYTES = {
    std::byte(0x30), std::byte(0x2E), std::byte(0x02), std::byte(0x01), std::byte(0x00), std::byte(0x30),
    std::byte(0x05), std::byte(0x06), std::byte(0x03), std::byte(0x2B), std::byte(0x65), std::byte(0x70),
    std::byte(0x04), std::byte(0x22), std::byte(0x04), std::byte(0x20)
  };

  /**
   * The hex-encoded string of the DER-encoded prefix bytes of an ED25519PrivateKey.
   */
  static inline const std::string DER_ENCODED_PREFIX_HEX = "302E020100300506032B657004220420";

  /**
   * SDK needs to provide a way to set an unusable key such as an Ed25519 all-zeros
   * key, since it is (presumably) impossible to find the 32-byte string whose SHA-512 hash begins with 32 bytes
   * of zeros. We recommend using all-zeros to clearly advertise any unsuable keys.
   */
  static inline const std::string ZERO_KEY_STR = "0000000000000000000000000000000000000000000000000000000000000000";

  /**
   * Disallow default construction of an ED25519PrivateKey, as an uninitialized ED25519PrivateKey provides no
   * functionality. Instead, a factory function should be used.
   */
  ED25519PrivateKey() = delete;

  /**
   * Generate a new ED25519PrivateKey.
   *
   * @return A pointer to the generated ED25519PrivateKey.
   * @throws OpenSSLException If OpenSSL fails to generate a key.
   */
  [[nodiscard]] static std::unique_ptr<ED25519PrivateKey> generatePrivateKey();

  /**
   * Construct an ED25519PrivateKey object from a hex-encoded string (DER-encoded or raw).
   *
   * @param key The hex string from which to construct an ED25519PrivateKey.
   * @return A pointer to an ED25519PrivateKey representing the input hex string.
   * @throws BadKeyException If an ED25519PrivateKey cannot be realized from the input hex string.
   */
  [[nodiscard]] static std::unique_ptr<ED25519PrivateKey> fromString(std::string_view key);

  /**
   * Construct an ED25519PrivateKey object from a byte vector (DER-encoded or raw).
   *
   * @param bytes The vector of bytes from which to construct an ED25519PrivateKey.
   * @return A pointer to an ED25519PrivateKey representing the input bytes.
   * @throws BadKeyException If an ED25519PrivateKey cannot be realized from the input bytes.
   */
  [[nodiscard]] static std::unique_ptr<ED25519PrivateKey> fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct an ED25519PrivateKey from a seed array.
   *
   * @param seed The seed byte array from which to construct an ED25519PrivateKey.
   * @return A pointer to an ED25519PrivateKey representing the input seed bytes.
   * @throws BadKeyException If an ED25519PrivateKey cannot be realized from the input seed bytes.
   */
  [[nodiscard]] static std::unique_ptr<ED25519PrivateKey> fromSeed(const std::vector<std::byte>& seed);

  /**
   * Derived from Key. Create a clone of this ED25519PrivateKey object.
   *
   * @return A pointer to the created clone of this ED25519PrivateKey.
   */
  [[nodiscard]] std::unique_ptr<Key> clone() const override;

  /**
   * Derived from Key. Construct a Key protobuf object from this ED25519PrivateKey object's ED25519PublicKey.
   *
   * @return A pointer to a created Key protobuf object filled with this ED25519PrivateKey object's ED25519PublicKey
   *         data.
   * @throws OpenSSLException If OpenSSL is unable to serialize this ED25519PrivateKey object's ED25519PublicKey.
   */
  [[nodiscard]] std::unique_ptr<proto::Key> toProtobufKey() const override;

  /**
   * Derived from PrivateKey. Derive a child ED25519PrivateKey from this ED25519PrivateKey.
   *
   * @param childIndex The unhardened index of the child to derive.
   * @return A pointer to the derived ED25519PrivateKey child.
   * @throws OpenSSLException       If OpenSSL is unable to derive a key with the given childIndex.
   * @throws UninitializedException If this ED25519PrivateKey was not initialized with a chain code.
   * @throws std::invalid_argument  If the input index is hardened.
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
   * Derived from PrivateKey. Get the hex-encoded string of the DER-encoded bytes of this ED25519PrivateKey.
   *
   * @return The hex-encoded string of the DER-encoded bytes of this ED25519PrivateKey.
   */
  [[nodiscard]] std::string toStringDer() const override;

  /**
   * Derived from PrivateKey. Get the hex-encoded string of the raw, non-DER-encoded bytes of this ED25519PrivateKey.
   *
   * @return The hex-encoded string of the raw bytes of this ED25519PrivateKey.
   */
  [[nodiscard]] std::string toStringRaw() const override;

  /**
   * Derived from Key. Get the byte representation of this ED25519PrivateKey. Returns the same result as toBytesRaw().
   *
   * @return The DER-encoded bytes of this ED25519PrivateKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const override;

  /**
   * Derived from PrivateKey. Get the DER-encoded bytes of this ED25519PrivateKey.
   *
   * @return The DER-encoded bytes of this ED25519PrivateKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesDer() const override;

  /**
   * Derived from PrivateKey. Get the raw, non-DER-encoded bytes of this ED25519PrivateKey.
   *
   * @return The raw bytes of this ED25519PrivateKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesRaw() const override;

  /**
   * Construct an ED25519PrivateKey object from zero key string.
   *
   * @return A pointer to an ED25519PrivateKey all zero key. Unusable key defined in HIP-540.
   * @throws BadKeyException If an ED25519PrivateKey cannot be realized zero key hex string.
   */
  [[nodiscard]] static std::unique_ptr<ED25519PrivateKey> getZeroKey();

private:
  /**
   * Construct from a wrapped OpenSSL key object and optionally a chain code.
   *
   * @param key       The wrapped OpenSSL key object.
   * @param chainCode The chain code.
   * @throws OpenSSLException If OpenSSL is unable to get this ED25519PrivateKey's corresponding ED25519PrivateKey's
   *                          bytes.
   * @throws BadKeyException  If the chain code is malformed.
   */
  explicit ED25519PrivateKey(internal::OpenSSLUtils::EVP_PKEY&& key,
                             std::vector<std::byte> chainCode = std::vector<std::byte>());
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ED25519_PRIVATE_KEY_H_
