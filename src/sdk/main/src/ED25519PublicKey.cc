// SPDX-License-Identifier: Apache-2.0
#include "ED25519PublicKey.h"
#include "exceptions/BadKeyException.h"
#include "exceptions/OpenSSLException.h"
#include "impl/ASN1ED25519PublicKey.h"
#include "impl/HexConverter.h"
#include "impl/PublicKeyImpl.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/EVP_MD_CTX.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <basic_types.pb.h>
#include <openssl/x509.h>

namespace Hiero
{
namespace
{
/**
 * Create a wrapped OpenSSL keypair object from a byte vector (raw or DER-encoded) representing an ED25519PublicKey.
 *
 * @param bytes The bytes representing a ED25519PublicKey.
 * @return The newly created wrapped OpenSSL keypair object.
 * @throws OpenSSLException If OpenSSL is unable to create a keypair from the input bytes.
 */
[[nodiscard]] internal::OpenSSLUtils::EVP_PKEY bytesToPKEY(std::vector<std::byte> bytes)
{
  if (bytes.size() == ED25519PublicKey::KEY_SIZE)
  {
    bytes = internal::Utilities::concatenateVectors({ internal::asn1::ASN1_EDPBK_PREFIX_BYTES, bytes });
  }
  else
  {
    internal::asn1::ASN1ED25519PublicKey asn1key(bytes);
    bytes = internal::Utilities::concatenateVectors({ internal::asn1::ASN1_EDPBK_PREFIX_BYTES, asn1key.getKey() });
  }

  const auto* bytesPtr = internal::Utilities::toTypePtr<unsigned char>(bytes.data());
  internal::OpenSSLUtils::EVP_PKEY key(d2i_PUBKEY(nullptr, &bytesPtr, static_cast<long>(bytes.size())));
  if (!key)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("d2i_PUBKEY"));
  }

  return key;
}

} // namespace

//-----
std::unique_ptr<ED25519PublicKey> ED25519PublicKey::fromString(std::string_view key)
{
  std::string formattedKey = key.data();
  // Remove PEM prefix/suffix if is present and hex the base64 val
  if (formattedKey.compare(
        0, internal::asn1::PEM_ECPBK_PREFIX_STRING.size(), internal::asn1::PEM_ECPBK_PREFIX_STRING) == 0)
  {
    formattedKey = formattedKey.substr(internal::asn1::PEM_ECPBK_PREFIX_STRING.size(), formattedKey.size());

    if (formattedKey.compare(formattedKey.size() - internal::asn1::PEM_ECPBK_SUFFIX_STRING.size(),
                             formattedKey.size(),
                             internal::asn1::PEM_ECPBK_SUFFIX_STRING) == 0)
    {
      formattedKey = formattedKey.substr(0, formattedKey.size() - internal::asn1::PEM_ECPBK_SUFFIX_STRING.size());
    }

    formattedKey = internal::HexConverter::base64ToHex(formattedKey);
  }

  try
  {
    return std::make_unique<ED25519PublicKey>(
      ED25519PublicKey(bytesToPKEY(internal::HexConverter::hexToBytes(formattedKey))));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(std::string("ED25519PublicKey cannot be realized from input string: ") +
                          openSSLException.what());
  }
}

//-----
std::unique_ptr<ED25519PublicKey> ED25519PublicKey::fromBytes(const std::vector<std::byte>& bytes)
{
  try
  {
    return std::make_unique<ED25519PublicKey>(ED25519PublicKey(bytesToPKEY(bytes)));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(std::string("ED25519PublicKey cannot be realized from the input bytes: ") +
                          openSSLException.what());
  }
}

//-----
std::unique_ptr<Key> ED25519PublicKey::clone() const
{
  return std::make_unique<ED25519PublicKey>(*this);
}

//----
std::unique_ptr<proto::Key> ED25519PublicKey::toProtobufKey() const
{
  auto keyProtobuf = std::make_unique<proto::Key>();
  keyProtobuf->set_ed25519(internal::Utilities::byteVectorToString(toBytesRaw()));
  return keyProtobuf;
}

//-----
bool ED25519PublicKey::verifySignature(const std::vector<std::byte>& signatureBytes,
                                       const std::vector<std::byte>& signedBytes) const
{
  internal::OpenSSLUtils::EVP_MD_CTX messageDigestContext(EVP_MD_CTX_new());
  if (!messageDigestContext)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_MD_CTX_new"));
  }

  if (EVP_DigestVerifyInit(messageDigestContext.get(), nullptr, nullptr, nullptr, getInternalKey().get()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestVerifyInit"));
  }

  const int verificationResult = EVP_DigestVerify(messageDigestContext.get(),
                                                  internal::Utilities::toTypePtr<unsigned char>(signatureBytes.data()),
                                                  signatureBytes.size(),
                                                  internal::Utilities::toTypePtr<unsigned char>(signedBytes.data()),
                                                  signedBytes.size());

  // Any value other than 0 or 1 means an error occurred
  if (verificationResult != 0 && verificationResult != 1)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestVerify"));
  }

  return verificationResult == 1;
}

//-----
std::string ED25519PublicKey::toStringDer() const
{
  return internal::HexConverter::bytesToHex(toBytesDer());
}

//-----
std::string ED25519PublicKey::toStringRaw() const
{
  return internal::HexConverter::bytesToHex(toBytesRaw());
}

//-----
std::vector<std::byte> ED25519PublicKey::toBytes() const
{
  return toBytesRaw();
}

//-----
std::vector<std::byte> ED25519PublicKey::toBytesDer() const
{
  int bytesLength = i2d_PUBKEY(getInternalKey().get(), nullptr);

  std::vector<std::byte> publicKeyBytes(bytesLength);

  if (auto rawPublicKeyBytes = internal::Utilities::toTypePtr<unsigned char>(publicKeyBytes.data());
      i2d_PUBKEY(getInternalKey().get(), &rawPublicKeyBytes) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("i2d_PUBKEY"));
  }

  return publicKeyBytes;
}

//-----
std::vector<std::byte> ED25519PublicKey::toBytesRaw() const
{
  return internal::Utilities::removePrefix(toBytesDer(), static_cast<long>(DER_ENCODED_PREFIX_BYTES.size()));
}

//-----
std::unique_ptr<proto::SignaturePair> ED25519PublicKey::toSignaturePairProtobuf(
  const std::vector<std::byte>& signature) const
{
  auto signaturePair = std::make_unique<proto::SignaturePair>();
  signaturePair->set_pubkeyprefix(internal::Utilities::byteVectorToString(toBytesRaw()));
  signaturePair->set_ed25519(internal::Utilities::byteVectorToString(signature));
  return signaturePair;
}

//-----
ED25519PublicKey::ED25519PublicKey(internal::OpenSSLUtils::EVP_PKEY&& key)
  : PublicKey(std::move(key))
{
}

//-----
std::shared_ptr<PublicKey> ED25519PublicKey::getShared() const
{
  return std::const_pointer_cast<ED25519PublicKey>(
    std::dynamic_pointer_cast<const ED25519PublicKey>(shared_from_this()));
}

} // namespace Hiero
