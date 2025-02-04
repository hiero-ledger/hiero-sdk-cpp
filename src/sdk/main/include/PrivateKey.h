// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_PRIVATE_KEY_H_
#define HIERO_SDK_CPP_PRIVATE_KEY_H_

#include "Key.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace Hiero
{
template<typename SdkRequestType>
class Transaction;

class PublicKey;
class WrappedTransaction;
}

namespace Hiero::internal::OpenSSLUtils
{
class EVP_PKEY;
}

namespace Hiero
{
/**
 * A generic class representing a private key.
 */
class PrivateKey : public Key
{
public:
  /**
   * The number of bytes in a PrivateKey chain code.
   */
  static constexpr const size_t CHAIN_CODE_SIZE = 32ULL;

  /**
   * Default destructor, but must define after PrivateKeyImpl is defined (in source file).
   */
  ~PrivateKey() override;

  /**
   * Construct a PrivateKey object from a hex-encoded, DER-encoded key string.
   *
   * @param key The DER-encoded hex string from which to construct a PrivateKey.
   * @return A pointer to an PrivateKey representing the input DER-encoded hex string.
   * @throws BadKeyException If the private key type (ED25519 or ECDSAsecp256k1) is unable to be determined or realized
   *                         from the input hex string.
   */
  [[nodiscard]] static std::unique_ptr<PrivateKey> fromStringDer(std::string_view key);

  /**
   * Construct a PrivateKey object from a DER-encoded byte vector.
   *
   * @param bytes The vector of DER-encoded bytes from which to construct a PrivateKey.
   * @return A pointer to a PrivateKey representing the input DER-encoded bytes.
   * @throws BadKeyException If the private key type (ED25519 or ECDSAsecp256k1) is unable to be determined or realized
   *                         from the input byte array.
   */
  [[nodiscard]] static std::unique_ptr<PrivateKey> fromBytesDer(const std::vector<std::byte>& bytes);

  /**
   * Derive a child PrivateKey from this PrivateKey.
   *
   * @param childIndex The index of the child to derive.
   * @return A pointer to the derived PrivateKey child.
   */
  [[nodiscard]] virtual std::unique_ptr<PrivateKey> derive(uint32_t childIndex) const = 0;

  /**
   * Sign an arbitrary byte array.
   *
   * @param bytesToSign The bytes to sign.
   * @return The signature of the byte array.
   */
  [[nodiscard]] virtual std::vector<std::byte> sign(const std::vector<std::byte>& bytesToSign) const = 0;

  /**
   * Get the hex-encoded string of the DER-encoded bytes of this PrivateKey.
   *
   * @return The hex-encoded string of the DER-encoded bytes of this PrivateKey.
   */
  [[nodiscard]] virtual std::string toStringDer() const = 0;

  /**
   * Get the hex-encoded string of the raw, non-DER-encoded bytes of this PrivateKey.
   *
   * @return The hex-encoded string of the raw bytes of this PrivateKey.
   */
  [[nodiscard]] virtual std::string toStringRaw() const = 0;

  /**
   * Get the DER-encoded bytes of this PrivateKey.
   *
   * @return The DER-encoded bytes of this PrivateKey.
   */
  [[nodiscard]] virtual std::vector<std::byte> toBytesDer() const = 0;

  /**
   * Get the raw, non-DER-encoded bytes of this PrivateKey.
   *
   * @return The raw bytes of this PrivateKey.
   */
  [[nodiscard]] virtual std::vector<std::byte> toBytesRaw() const = 0;

  /**
   * Sign a Transaction with this PrivateKey.
   *
   * @param transaction The Transaction to sign.
   * @return The generated signature.
   * @throws IllegalStateException If there is not exactly one node account ID set for the Transaction or if the
   *                               Transaction is not frozen and doesn't have a TransactionId set.
   */
  template<typename SdkRequestType>
  std::vector<std::byte> signTransaction(Transaction<SdkRequestType>& transaction) const;
  std::vector<std::byte> signTransaction(WrappedTransaction& transaction) const;

  /**
   * Get this PrivateKey's chain code. It is possible that the chain code could be empty.
   *
   * @return This PrivateKey's chaincode if it exists, otherwise an empty vector.
   */
  [[nodiscard]] std::vector<std::byte> getChainCode() const;

  /**
   * Get the PublicKey that corresponds to this PrivateKey.
   *
   * @return A pointer to the PublicKey that corresponds to this PrivateKey.
   */
  [[nodiscard]] std::shared_ptr<PublicKey> getPublicKey() const;

  /**
   * Write this PrivateKey in DER-encoded hex to an output stream.
   *
   * @param os  The output stream.
   * @param key The PrivateKey to print.
   * @return The output stream with this PrivateKey written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const PrivateKey& key);

protected:
  /**
   * Prevent public copying and moving to prevent slicing. Use the 'clone()' virtual method instead.
   */
  PrivateKey(const PrivateKey&);
  PrivateKey& operator=(const PrivateKey&);
  PrivateKey(PrivateKey&&) noexcept;
  PrivateKey& operator=(PrivateKey&&) noexcept;

  /**
   * Construct from a wrapped OpenSSL key object and optionally a chain code.
   *
   * @param key       The wrapped OpenSSL key object.
   * @param chainCode The chain code.
   * @throws OpenSSLException If OpenSSL is unable to get this PrivateKey's corresponding PrivateKey's bytes.
   * @throws BadKeyException  If the chain code is malformed.
   */
  explicit PrivateKey(internal::OpenSSLUtils::EVP_PKEY&& key,
                      std::vector<std::byte> chainCode = std::vector<std::byte>());

  /**
   * Get this PrivateKey's wrapped OpenSSL key object.
   *
   * @return This PrivateKey's wrapped OpenSSL key object.
   */
  [[nodiscard]] internal::OpenSSLUtils::EVP_PKEY getInternalKey() const;

private:
  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct PrivateKeyImpl;
  std::unique_ptr<PrivateKeyImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_PRIVATE_KEY_H_
