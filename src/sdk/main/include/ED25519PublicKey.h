// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ED25519_PUBLIC_KEY_H_
#define HIERO_SDK_CPP_ED25519_PUBLIC_KEY_H_

#include "PublicKey.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Hiero
{
/**
 * A class representing an ED25519 public key.
 */
class ED25519PublicKey : public PublicKey
{
public:
  /**
   * The number of raw bytes in an ED25519PublicKey.
   */
  static constexpr const size_t KEY_SIZE = 32ULL;

  /**
   * The prefix bytes of a DER-encoded ED25519PublicKey.
   */
  static inline const std::vector<std::byte> DER_ENCODED_PREFIX_BYTES = {
    std::byte(0x30), std::byte(0x2A), std::byte(0x30), std::byte(0x05), std::byte(0x06), std::byte(0x03),
    std::byte(0x2B), std::byte(0x65), std::byte(0x70), std::byte(0x03), std::byte(0x21), std::byte(0x00)
  };

  /**
   * The hex-encoded string of the DER-encoded prefix bytes of an ED25519PublicKey.
   */
  static inline const std::string DER_ENCODED_PREFIX_HEX = "302A300506032B6570032100";

  /**
   * Disallow default construction of an ED25519PublicKey, as an uninitialized ED25519PublicKey provides no
   * functionality. Instead, a factory function should be used.
   */
  ED25519PublicKey() = delete;

  /**
   * Construct an ED25519PublicKey object from a hex-encoded string (DER-encoded or raw).
   *
   * @param key The hex string from which to construct an ED25519PublicKey.
   * @return A pointer to an ED25519PublicKey representing the input hex string.
   * @throws BadKeyException If an ED25519PublicKey cannot be realized from the input hex string.
   */
  [[nodiscard]] static std::unique_ptr<ED25519PublicKey> fromString(std::string_view key);

  /**
   * Construct an ED25519PublicKey object from a byte vector (DER-encoded or raw).
   *
   * @param bytes The vector of bytes from which to construct an ED25519PublicKey.
   * @return A pointer to an ED25519PublicKey representing the input bytes.
   * @throws BadKeyException If an ED25519PublicKey cannot be realized from the input bytes.
   */
  [[nodiscard]] static std::unique_ptr<ED25519PublicKey> fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Derived from Key. Create a clone of this ED25519PublicKey object.
   *
   * @return A pointer to the created clone of this ED25519PublicKey.
   */
  [[nodiscard]] std::unique_ptr<Key> clone() const override;

  /**
   * Derived from Key. Construct a Key protobuf object from this ED25519PublicKey object.
   *
   * @return A pointer to a created Key protobuf object filled with this ED25519PublicKey object's data.
   * @throws OpenSSLException If OpenSSL is unable to serialize this ED25519PublicKey.
   */
  [[nodiscard]] std::unique_ptr<proto::Key> toProtobufKey() const override;

  /**
   * Derived from PublicKey. Verify that a signature was made by the ED25519PrivateKey which corresponds to this
   * ED25519PublicKey.
   *
   * @param signatureBytes The byte vector representing the signature.
   * @param signedBytes    The bytes which were purportedly signed to create the signature.
   * @return \c TRUE if the signature is valid for this ED25519PublicKey's private key, otherwise \c FALSE.
   * @throws OpenSSLException If OpenSSL is unable to verify the signature.
   */
  [[nodiscard]] bool verifySignature(const std::vector<std::byte>& signatureBytes,
                                     const std::vector<std::byte>& signedBytes) const override;

  /**
   * Derived from PublicKey. Get the hex-encoded string of the DER-encoded bytes of this ED25519PublicKey.
   *
   * @return The hex-encoded string of the DER-encoded bytes of this ED25519PublicKey.
   */
  [[nodiscard]] std::string toStringDer() const override;

  /**
   * Derived from PublicKey. Get the hex-encoded string of the raw bytes of this ED25519PublicKey.
   *
   * @return The hex-encoded string of the raw bytes of this ED25519PublicKey.
   */
  [[nodiscard]] std::string toStringRaw() const override;

  /**
   * Derived from Key. Get the byte representation of this ED25519PublicKey. Returns the same result as toBytesRaw().
   *
   * @return The DER-encoded bytes of this ED25519PublicKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const override;

  /**
   * Derived from PublicKey. Get the DER-encoded bytes of this ED25519PublicKey.
   *
   * @return The DER-encoded bytes of this ED25519PublicKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesDer() const override;

  /**
   * Derived from PublicKey. Get the raw bytes of this ED25519PublicKey.
   *
   * @return The raw bytes of this ED25519PublicKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesRaw() const override;

  /**
   * Derived from PublicKey. Serialize this ED25519PublicKey to a SignaturePair protobuf object with the given
   * signature.
   *
   * @param signature The signature created by this ED25519PublicKey.
   */
  [[nodiscard]] std::unique_ptr<proto::SignaturePair> toSignaturePairProtobuf(
    const std::vector<std::byte>& signature) const override;

private:
  /**
   * Construct from a wrapped OpenSSL key object.
   *
   * @param key The wrapped OpenSSL key object from which to construct this ED25519PublicKey.
   */
  explicit ED25519PublicKey(internal::OpenSSLUtils::EVP_PKEY&& key);

  /**
   * Derived from PublicKey. Get a std::shared_ptr to this ED25519PublicKey.
   *
   * @returns A pointer to this ED25519PublicKey.
   */
  [[nodiscard]] std::shared_ptr<PublicKey> getShared() const override;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ED25519_PUBLIC_KEY_H_
