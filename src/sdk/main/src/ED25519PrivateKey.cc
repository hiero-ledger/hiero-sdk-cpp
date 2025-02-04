// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "ED25519PublicKey.h"
#include "exceptions/BadKeyException.h"
#include "exceptions/OpenSSLException.h"
#include "exceptions/UninitializedException.h"
#include "impl/ASN1ED25519PrivateKey.h"
#include "impl/DerivationPathUtils.h"
#include "impl/HexConverter.h"
#include "impl/PrivateKeyImpl.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/EVP_MD_CTX.h"
#include "impl/openssl_utils/EVP_PKEY.h"
#include "impl/openssl_utils/EVP_PKEY_CTX.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <basic_types.pb.h>
#include <openssl/x509.h>

namespace Hiero
{
namespace
{
// The seed to use to compute the SHA512 HMAC, as defined in SLIP 10.
const std::vector<std::byte> SLIP10_SEED = { std::byte('e'), std::byte('d'), std::byte('2'), std::byte('5'),
                                             std::byte('5'), std::byte('1'), std::byte('9'), std::byte(' '),
                                             std::byte('s'), std::byte('e'), std::byte('e'), std::byte('d') };

/**
 * Create a wrapped OpenSSL key object from a byte vector (raw or DER-encoded) representing an ED25519PrivateKey.
 *
 * @param bytes The bytes representing a ED25519PrivateKey.
 * @return The newly created wrapped OpenSSL keypair object.
 * @throws OpenSSLException If OpenSSL is unable to create a keypair from the input bytes.
 */
[[nodiscard]] internal::OpenSSLUtils::EVP_PKEY bytesToPKEY(std::vector<std::byte> bytes)
{
  if (bytes.size() == ED25519PrivateKey::KEY_SIZE)
  {
    bytes = internal::Utilities::concatenateVectors({ internal::asn1::ASN1_EDPRK_PREFIX_BYTES, bytes });
  }
  else
  {
    internal::asn1::ASN1ED25519PrivateKey asn1key(bytes);
    bytes = internal::Utilities::concatenateVectors({ internal::asn1::ASN1_EDPRK_PREFIX_BYTES, asn1key.getKey() });
  }

  const unsigned char* rawKeyBytes = internal::Utilities::toTypePtr<unsigned char>(bytes.data());
  internal::OpenSSLUtils::EVP_PKEY key(
    d2i_PrivateKey(EVP_PKEY_ED25519, nullptr, &rawKeyBytes, static_cast<long>(bytes.size())));
  if (!key)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("d2i_PrivateKey"));
  }

  return key;
}

} // namespace

//-----
std::unique_ptr<ED25519PrivateKey> ED25519PrivateKey::generatePrivateKey()
{
  internal::OpenSSLUtils::EVP_PKEY_CTX keyAlgorithmContext(EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr));
  if (!keyAlgorithmContext)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_PKEY_CTX_new_id"));
  }

  if (EVP_PKEY_keygen_init(keyAlgorithmContext.get()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_PKEY_keygen_init"));
  }

  EVP_PKEY* keypair = nullptr;
  if (EVP_PKEY_generate(keyAlgorithmContext.get(), &keypair) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_PKEY_generate"));
  }

  return std::make_unique<ED25519PrivateKey>(ED25519PrivateKey(internal::OpenSSLUtils::EVP_PKEY(keypair)));
}

//-----
std::unique_ptr<ED25519PrivateKey> ED25519PrivateKey::fromString(std::string_view key)
{
  std::string formattedKey = key.data();
  // Remove PEM prefix/suffix if is present and hex the base64 val
  if (formattedKey.compare(
        0, internal::asn1::PEM_EDPRK_PREFIX_STRING.size(), internal::asn1::PEM_EDPRK_PREFIX_STRING) == 0)
  {
    formattedKey = formattedKey.substr(internal::asn1::PEM_EDPRK_PREFIX_STRING.size(), formattedKey.size());

    if (formattedKey.compare(formattedKey.size() - internal::asn1::PEM_EDPRK_SUFFIX_STRING.size(),
                             formattedKey.size(),
                             internal::asn1::PEM_EDPRK_SUFFIX_STRING) == 0)
      formattedKey = formattedKey.substr(0, formattedKey.size() - internal::asn1::PEM_EDPRK_SUFFIX_STRING.size());

    formattedKey = internal::HexConverter::base64ToHex(formattedKey);
  }

  try
  {
    return std::make_unique<ED25519PrivateKey>(
      ED25519PrivateKey(bytesToPKEY(internal::HexConverter::hexToBytes(formattedKey))));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(std::string("ED25519PrivateKey cannot be realized from input string: ") +
                          openSSLException.what());
  }
}

//-----
std::unique_ptr<ED25519PrivateKey> ED25519PrivateKey::fromBytes(const std::vector<std::byte>& bytes)
{
  try
  {
    return std::make_unique<ED25519PrivateKey>(ED25519PrivateKey(bytesToPKEY(bytes)));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(std::string("ED25519PrivateKey cannot be realized from input bytes: ") +
                          openSSLException.what());
  }
}

//-----
std::unique_ptr<ED25519PrivateKey> ED25519PrivateKey::fromSeed(const std::vector<std::byte>& seed)
{
  try
  {
    const std::vector<std::byte> hmacOutput = internal::OpenSSLUtils::computeSHA512HMAC(SLIP10_SEED, seed);

    // The hmac is the key bytes followed by the chain code bytes
    return std::make_unique<ED25519PrivateKey>(
      ED25519PrivateKey(bytesToPKEY({ hmacOutput.cbegin(), hmacOutput.cbegin() + KEY_SIZE }),
                        { hmacOutput.cbegin() + KEY_SIZE, hmacOutput.cbegin() + KEY_SIZE + CHAIN_CODE_SIZE }));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(std::string("ED25519PrivateKey cannot be realized from input seed bytes: ") +
                          openSSLException.what());
  }
}

//-----
std::unique_ptr<Key> ED25519PrivateKey::clone() const
{
  return std::make_unique<ED25519PrivateKey>(*this);
}

//-----
std::unique_ptr<proto::Key> ED25519PrivateKey::toProtobufKey() const
{
  if (toStringRaw() == ZERO_KEY_STR)
  {
    auto keyProtobuf = std::make_unique<proto::Key>();
    keyProtobuf->set_ed25519(internal::Utilities::byteVectorToString(toBytesRaw()));
    return keyProtobuf;
  }
  return getPublicKey()->toProtobufKey();
}

//-----
std::vector<std::byte> ED25519PrivateKey::sign(const std::vector<std::byte>& bytesToSign) const
{
  internal::OpenSSLUtils::EVP_MD_CTX messageDigestContext(EVP_MD_CTX_new());
  if (!messageDigestContext)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_MD_CTX_new"));
  }

  if (EVP_DigestSignInit(messageDigestContext.get(), nullptr, nullptr, nullptr, getInternalKey().get()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestSignInit"));
  }

  // Calculate the required size for the signature
  size_t signatureLength;
  if (EVP_DigestSign(messageDigestContext.get(),
                     nullptr,
                     &signatureLength,
                     internal::Utilities::toTypePtr<unsigned char>(bytesToSign.data()),
                     bytesToSign.size()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestSign"));
  }

  std::vector<std::byte> signature(signatureLength);
  if (EVP_DigestSign(messageDigestContext.get(),
                     internal::Utilities::toTypePtr<unsigned char>(signature.data()),
                     &signatureLength,
                     internal::Utilities::toTypePtr<unsigned char>(bytesToSign.data()),
                     bytesToSign.size()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestSign"));
  }

  return signature;
}

//-----
std::string ED25519PrivateKey::toStringDer() const
{
  return internal::HexConverter::bytesToHex(toBytesDer());
}

//-----
std::string ED25519PrivateKey::toStringRaw() const
{
  return internal::HexConverter::bytesToHex(toBytesRaw());
}

//-----
std::vector<std::byte> ED25519PrivateKey::toBytes() const
{
  return toBytesRaw();
}

//-----
std::vector<std::byte> ED25519PrivateKey::toBytesDer() const
{
  int bytesLength = i2d_PrivateKey(getInternalKey().get(), nullptr);

  std::vector<std::byte> outputBytes(bytesLength);

  if (auto* rawBytes = internal::Utilities::toTypePtr<unsigned char>(outputBytes.data());
      i2d_PrivateKey(getInternalKey().get(), &rawBytes) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("i2d_PrivateKey"));
  }

  return outputBytes;
}

//-----
std::vector<std::byte> ED25519PrivateKey::toBytesRaw() const
{
  return internal::Utilities::removePrefix(toBytesDer(), static_cast<long>(DER_ENCODED_PREFIX_BYTES.size()));
}

//-----
std::unique_ptr<ED25519PrivateKey> ED25519PrivateKey::getZeroKey()
{
  return fromString(ZERO_KEY_STR);
}

//-----
std::unique_ptr<PrivateKey> ED25519PrivateKey::derive(uint32_t childIndex) const
{
  if (getChainCode().empty())
  {
    throw UninitializedException("Key not initialized with chain code, unable to derive keys");
  }

  // As per SLIP0010, private key must be padded to 33 bytes
  const std::vector<std::byte> hmacOutput = internal::OpenSSLUtils::computeSHA512HMAC(
    getChainCode(),
    internal::Utilities::concatenateVectors({ { std::byte(0x0) },
                                              toBytesRaw(),
                                              internal::DerivationPathUtils::indexToBigEndianArray(
                                                internal::DerivationPathUtils::getHardenedIndex(childIndex)) }));

  // The hmac is the key bytes followed by the chain code bytes
  return std::make_unique<ED25519PrivateKey>(
    ED25519PrivateKey(bytesToPKEY({ hmacOutput.cbegin(), hmacOutput.cbegin() + KEY_SIZE }),
                      { hmacOutput.cbegin() + KEY_SIZE, hmacOutput.cbegin() + KEY_SIZE + CHAIN_CODE_SIZE }));
}

//-----
ED25519PrivateKey::ED25519PrivateKey(internal::OpenSSLUtils::EVP_PKEY&& key, std::vector<std::byte> chainCode)
  : PrivateKey(std::move(key), std::move(chainCode))
{
}

} // namespace Hiero
