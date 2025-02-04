// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_PUBLIC_KEY_H_
#define HIERO_SDK_CPP_PUBLIC_KEY_H_

#include "Key.h"

#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class Key;
class SignaturePair;
}

namespace Hiero
{
namespace internal::OpenSSLUtils
{
class EVP_PKEY;
}

class AccountId;
}

namespace Hiero
{
/**
 * A generic class representing a public key.
 */
class PublicKey
  : public Key
  , public std::enable_shared_from_this<PublicKey>
{
public:
  /**
   * Default destructor, but must define after PublicKeyImpl is defined (in source file).
   */
  ~PublicKey() override;

  /**
   * Construct a PublicKey object from a hex-encoded, DER-encoded key string.
   *
   * @param key The DER-encoded hex string from which to construct a PublicKey.
   * @return A pointer to an PublicKey representing the input DER-encoded hex string.
   * @throws BadKeyException If the public key type (ED25519 or ECDSAsecp256k1) is unable to be determined or realized
   *                         from the input hex string.
   */
  [[nodiscard]] static std::unique_ptr<PublicKey> fromStringDer(std::string_view key);

  /**
   * Construct a PublicKey object from a raw byte vector. This will attempt to determine the type of key based on the
   * input byte vector length.
   *
   * @param bytes The vector of raw bytes from which to construct a PublicKey.
   * @return A pointer to a PublicKey representing the input DER-encoded bytes.
   * @throws BadKeyException If the public key type (ED25519 or ECDSAsecp256k1) is unable to be determined or realized
   *                         from the input byte array.
   */
  [[nodiscard]] static std::unique_ptr<PublicKey> fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a PublicKey object from a DER-encoded byte vector.
   *
   * @param bytes The vector of DER-encoded bytes from which to construct a PublicKey.
   * @return A pointer to a PublicKey representing the input DER-encoded bytes.
   * @throws BadKeyException If the public key type (ED25519 or ECDSAsecp256k1) is unable to be determined or realized
   *                         from the input byte array.
   */
  [[nodiscard]] static std::unique_ptr<PublicKey> fromBytesDer(const std::vector<std::byte>& bytes);

  /**
   * Construct a PublicKey object from a byte array representing an alias.
   *
   * @param alias The bytes representing an alias.
   * @return A pointer to a PublicKey representing the input alias bytes, or nullptr if the input alias byte array does
   *         not represent a PublicKey.
   */
  [[nodiscard]] static std::unique_ptr<PublicKey> fromAliasBytes(const std::vector<std::byte>& bytes);

  /**
   * Verify that a signature was made by the PrivateKey which corresponds to this PublicKey.
   *
   * @param signatureBytes The byte vector representing the signature.
   * @param signedBytes    The bytes which were purportedly signed to create the signature.
   * @return \c TRUE if the signature is valid, otherwise \c FALSE.
   */
  [[nodiscard]] virtual bool verifySignature(const std::vector<std::byte>& signatureBytes,
                                             const std::vector<std::byte>& signedBytes) const = 0;

  /**
   * Get the hex-encoded string of the DER-encoded bytes of this PublicKey.
   *
   * @return The hex-encoded string of the DER-encoded bytes of this PublicKey.
   */
  [[nodiscard]] virtual std::string toStringDer() const = 0;

  /**
   * Get the hex-encoded string of the raw bytes of this PublicKey.
   *
   * @return The hex-encoded string of the raw bytes of this PublicKey.
   */
  [[nodiscard]] virtual std::string toStringRaw() const = 0;

  /**
   * Get the DER-encoded bytes of this PublicKey.
   *
   * @return The DER-encoded bytes of this PublicKey.
   */
  [[nodiscard]] virtual std::vector<std::byte> toBytesDer() const = 0;

  /**
   * Get the raw bytes of this PublicKey.
   *
   * @return The raw bytes of this PublicKey.
   */
  [[nodiscard]] virtual std::vector<std::byte> toBytesRaw() const = 0;

  /**
   * Serialize this PublicKey to a SignaturePair protobuf object with the given signature.
   *
   * @param signature The signature created by this PublicKey.
   */
  [[nodiscard]] virtual std::unique_ptr<proto::SignaturePair> toSignaturePairProtobuf(
    const std::vector<std::byte>& signature) const = 0;

  /**
   * Construct an AccountId object using this PublicKey as its alias.
   *
   * @param shard The shard of the AccountId.
   * @param realm The realm of the AccountId.
   * @return The constructed AccountId.
   */
  [[nodiscard]] AccountId toAccountId(uint64_t shard = 0ULL, uint64_t realm = 0ULL) const;

  /**
   * Write this PublicKey in DER-encoded hex to an output stream.
   *
   * @param os  The output stream.
   * @param key The PublicKey to print.
   * @return The output stream with this PublicKey written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const PublicKey& key);

protected:
  /**
   * Prevent public copying and moving to prevent slicing. Use the 'clone()' virtual method instead.
   */
  PublicKey(const PublicKey&);
  PublicKey& operator=(const PublicKey&);
  PublicKey(PublicKey&&) noexcept;
  PublicKey& operator=(PublicKey&&) noexcept;

  /**
   * Construct with a wrapped OpenSSL key object.
   *
   * @param key The wrapped OpenSSL key object.
   */
  explicit PublicKey(internal::OpenSSLUtils::EVP_PKEY&& key);

  /**
   * Get this PublicKey's wrapped OpenSSL key object.
   *
   * @return This PublicKey's wrapped OpenSSL key object.
   */
  [[nodiscard]] internal::OpenSSLUtils::EVP_PKEY getInternalKey() const;

private:
  /**
   * Get a std::shared_ptr to this PublicKey.
   *
   * @returns A pointer to this PublicKey.
   */
  [[nodiscard]] virtual std::shared_ptr<PublicKey> getShared() const = 0;

  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct PublicKeyImpl;
  std::unique_ptr<PublicKeyImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_PUBLIC_KEY_H_
