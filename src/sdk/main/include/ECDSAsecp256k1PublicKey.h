// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ECDSA_SECP256K1_PUBLIC_KEY_H_
#define HIERO_SDK_CPP_ECDSA_SECP256K1_PUBLIC_KEY_H_

#include "PublicKey.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace Hiero
{
class EvmAddress;
}

namespace Hiero
{
/**
 * A class representing an ECDSA secp256k1 public key.
 */
class ECDSAsecp256k1PublicKey : public PublicKey
{
public:
  /**
   * The number of raw bytes in an uncompressed ECDSAsecp256k1PublicKey.
   */
  static constexpr const size_t UNCOMPRESSED_KEY_SIZE = 65ULL;

  /**
   * The number of raw bytes in a compressed ECDSAsecp256k1PublicKey.
   */
  static constexpr const size_t COMPRESSED_KEY_SIZE = 33ULL;

  /**
   * The prefix bytes of a DER-encoded, uncompressed ECDSAsecp256k1PublicKey.
   */
  static inline const std::vector<std::byte> DER_ENCODED_UNCOMPRESSED_PREFIX_BYTES = {
    std::byte(0x30), std::byte(0x56), std::byte(0x30), std::byte(0x10), std::byte(0x06), std::byte(0x07),
    std::byte(0x2A), std::byte(0x86), std::byte(0x48), std::byte(0xCE), std::byte(0x3D), std::byte(0x02),
    std::byte(0x01), std::byte(0x06), std::byte(0x05), std::byte(0x2B), std::byte(0x81), std::byte(0x04),
    std::byte(0x00), std::byte(0x0A), std::byte(0x03), std::byte(0x42), std::byte(0x00)
  };

  /**
   * The hex-encoded string of the DER-encoded prefix bytes of an uncompressed ECDSAsecp256k1PublicKey.
   */
  static inline const std::string DER_ENCODED_UNCOMPRESSED_PREFIX_HEX =
    "3056301006072A8648CE3D020106052B8104000A034200";

  /**
   * The prefix bytes of a DER-encoded, compressed ECDSAsecp256k1PublicKey.
   */
  static inline const std::vector<std::byte> DER_ENCODED_COMPRESSED_PREFIX_BYTES = {
    std::byte(0x30), std::byte(0x2D), std::byte(0x30), std::byte(0x07), std::byte(0x06),
    std::byte(0x05), std::byte(0x2B), std::byte(0x81), std::byte(0x04), std::byte(0x00),
    std::byte(0x0A), std::byte(0x03), std::byte(0x22), std::byte(0x00)
  };

  /**
   * The hex-encoded string of the DER-encoded prefix bytes of a compressed ECDSAsecp256k1PublicKey.
   */
  static inline const std::string DER_ENCODED_COMPRESSED_PREFIX_HEX = "302D300706052B8104000A032200";

  /**
   * Disallow default construction of an ECDSAsecp256k1PublicKey, as an uninitialized ECDSAsecp256k1PublicKey provides
   * no functionality. Instead, a factory function should be used.
   */
  ECDSAsecp256k1PublicKey() = delete;

  /**
   * Construct an ECDSAsecp256k1PublicKey object from a hex-encoded string (DER-encoded or raw).
   *
   * @param key The hex string from which to construct an ECDSAsecp256k1PublicKey. May be either compressed or
   *            uncompressed.
   * @return A pointer to an ECDSAsecp256k1PublicKey representing the input hex string.
   * @throws BadKeyException If an ECDSAsecp256k1PublicKey cannot be realized from the input hex string.
   */
  [[nodiscard]] static std::unique_ptr<ECDSAsecp256k1PublicKey> fromString(std::string_view key);

  /**
   * Construct an ECDSAsecp256k1PublicKey object from a byte vector (DER-encoded or raw).
   *
   * @param bytes The vector of bytes from which to construct an ECDSAsecp256k1PublicKey.
   * @return A pointer to an ECDSAsecp256k1PublicKey representing the input bytes.
   * @throws BadKeyException If an ECDSAsecp256k1PublicKey cannot be realized from the input bytes.
   */
  [[nodiscard]] static std::unique_ptr<ECDSAsecp256k1PublicKey> fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Converts an uncompressed ECDSAsecp256k1PublicKey byte vector to a compressed ECDSAsecp256k1PublicKey byte vector.
   *
   * @param uncompressedBytes The uncompressed bytes of the ECDSAsecp256k1PublicKey.
   * @return A byte vector representing the ECDSAsecp256k1PublicKey in compressed form.
   * @throws std::invalid_argument If the input bytes are not the correct uncompressed key size or malformed.
   * @throws OpenSSLException      If OpenSSL is unable to compress the input bytes.
   */
  [[nodiscard]] static std::vector<std::byte> compressBytes(const std::vector<std::byte>& uncompressedBytes);

  /**
   * Converts a compressed ECDSAsecp256k1PublicKey byte vector to an uncompressed ECDSAsecp256k1PublicKey byte vector.
   *
   * @param compressedBytes The compressed bytes of the ECDSAsecp256k1PublicKey.
   * @return A byte vector representing the ECDSAsecp256k1PublicKey in uncompressed form.
   * @throws std::invalid_argument If the input bytes are not the correct compressed key size or malformed.
   * @throws OpenSSLException      If OpenSSL is unable to uncompress the input bytes.
   */
  [[nodiscard]] static std::vector<std::byte> uncompressBytes(const std::vector<std::byte>& compressedBytes);

  /**
   * Derived from Key. Create a clone of this ECDSAsecp256k1PublicKey object.
   *
   * @return A pointer to the created clone of this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::unique_ptr<Key> clone() const override;

  /**
   * Derived from Key. Construct a Key protobuf object from this ECDSAsecp256k1PublicKey object.
   *
   * @return A pointer to a created Key protobuf object filled with this ECDSAsecp256k1PublicKey object's data.
   * @throws OpenSSLException If OpenSSL is unable to serialize this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::unique_ptr<proto::Key> toProtobufKey() const override;

  /**
   * Derived from PublicKey. Verify that a signature was made by the ECDSAsecp256k1PrivateKey which corresponds to this
   * ECDSAsecp256k1PublicKey.
   *
   * @param signatureBytes The byte vector representing the signature.
   * @param signedBytes    The bytes which were purportedly signed to create the signature.
   * @return \c TRUE if the signature is valid, otherwise \c FALSE.
   * @throws OpenSSLException If OpenSSL is unable to verify the signature.
   */
  [[nodiscard]] bool verifySignature(const std::vector<std::byte>& signatureBytes,
                                     const std::vector<std::byte>& signedBytes) const override;

  /**
   * Derived from PublicKey. Get the hex-encoded string of the DER-encoded bytes of this ECDSAsecp256k1PublicKey.
   *
   * @return The hex-encoded string of the DER-encoded bytes of this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::string toStringDer() const override;

  /**
   * Derived from PublicKey. Get the hex-encoded string of the raw bytes of this ECDSAsecp256k1PublicKey.
   *
   * @return The hex-encoded string of the raw bytes of this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::string toStringRaw() const override;

  /**
   * Derived from Key. Get the byte representation of this ECDSAsecp256k1PublicKey. Returns the same result as
   * toBytesDer().
   *
   * @return The DER-encoded bytes of this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const override;

  /**
   * Derived from PublicKey. Get the DER-encoded bytes of this ECDSAsecp256k1PublicKey.
   *
   * @return The DER-encoded bytes of this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesDer() const override;

  /**
   * Derived from PublicKey. Get the raw bytes of this ECDSAsecp256k1PublicKey.
   *
   * @return The raw bytes of this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::vector<std::byte> toBytesRaw() const override;

  /**
   * Derived from PublicKey. Serialize this ECDSAsecp256k1PublicKey to a SignaturePair protobuf object with the given
   * signature.
   *
   * @param signature The signature created by this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::unique_ptr<proto::SignaturePair> toSignaturePairProtobuf(
    const std::vector<std::byte>& signature) const override;

  /**
   * Construct an EvmAddress from this ECDSAsecp256k1PublicKey. The constructed EvmAddress will be the last 20 bytes of
   * the keccak-256 hash of this ECDSAsecp256k1PublicKey.
   *
   * @return The constructed EvmAddress.
   */
  [[nodiscard]] EvmAddress toEvmAddress() const;

private:
  /**
   * Construct from a wrapped OpenSSL key object.
   *
   * @param key The wrapped OpenSSL key object from which to construct this ECDSAsecp256k1PublicKey.
   */
  explicit ECDSAsecp256k1PublicKey(internal::OpenSSLUtils::EVP_PKEY&& key);

  /**
   * Derived from PublicKey. Get a std::shared_ptr to this ECDSAsecp256k1PublicKey.
   *
   * @returns A pointer to this ECDSAsecp256k1PublicKey.
   */
  [[nodiscard]] std::shared_ptr<PublicKey> getShared() const override;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ECDSA_SECP256K1_PUBLIC_KEY_H_
