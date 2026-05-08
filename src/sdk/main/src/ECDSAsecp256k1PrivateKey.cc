// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "exceptions/BadKeyException.h"
#include "exceptions/OpenSSLException.h"
#include "exceptions/UninitializedException.h"
#include "impl/ASN1ECPrivateKey.h"
#include "impl/DerivationPathUtils.h"
#include "impl/HexConverter.h"
#include "impl/PrivateKeyImpl.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/BIGNUM.h"
#include "impl/openssl_utils/ECDSA_SIG.h"
#include "impl/openssl_utils/EVP_MD.h"
#include "impl/openssl_utils/EVP_MD_CTX.h"
#include "impl/openssl_utils/OSSL_LIB_CTX.h"
#include "impl/openssl_utils/OpenSSLUtils.h"
#include "impl/openssl_utils/Secp256k1Context.h"

#include <openssl/ec.h>
#include <openssl/x509.h>

#include <secp256k1.h>
#include <secp256k1_recovery.h>

#include <cstring>

#include <services/basic_types.pb.h>

namespace Hiero
{
namespace
{
// The seed to use to compute the SHA512 HMAC, as defined in BIP32.
const std::vector<std::byte> BIP32_SEED = { std::byte('B'), std::byte('i'), std::byte('t'), std::byte('c'),
                                            std::byte('o'), std::byte('i'), std::byte('n'), std::byte(' '),
                                            std::byte('s'), std::byte('e'), std::byte('e'), std::byte('d') };
// The order of the secp256k1 curve.
const internal::OpenSSLUtils::BIGNUM CURVE_ORDER =
  internal::OpenSSLUtils::BIGNUM::fromHex("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");

/**
 * Create a wrapped OpenSSL key object from a byte vector (raw or DER-encoded) representing an ECDSAsecp256k1PrivateKey.
 *
 * @param bytes The bytes representing a ECDSAsecp256k1PrivateKey.
 * @return The newly created wrapped OpenSSL keypair object.
 * @throws OpenSSLException If OpenSSL is unable to create a keypair from the input bytes.
 */
[[nodiscard]] internal::OpenSSLUtils::EVP_PKEY bytesToPKEY(const std::vector<std::byte>& bytes)
{
  std::vector<std::byte> buildPrivateKeyBytes;

  // This means potentially only the key bytes will be in the input not standard for ASN1 encodings but the SDK needs to
  // be able to process them
  if (bytes.size() == internal::asn1::EC_KEY_LENGTH)
  {
    buildPrivateKeyBytes = internal::Utilities::concatenateVectors(
      { internal::asn1::ASN1_PRK_PREFIX_BYTES, bytes, internal::asn1::ASN1_PRK_SUFFIX_BYTES });
  }
  else
  {
    internal::asn1::ASN1ECPrivateKey asn1key(bytes);
    buildPrivateKeyBytes = internal::Utilities::concatenateVectors(
      { internal::asn1::ASN1_PRK_PREFIX_BYTES, asn1key.getKey(), internal::asn1::ASN1_PRK_SUFFIX_BYTES });
  }

  const std::vector<std::byte> privateKeyBytes = buildPrivateKeyBytes;

  auto rawKeyBytes = internal::Utilities::toTypePtr<unsigned char>(privateKeyBytes.data());
  internal::OpenSSLUtils::EVP_PKEY key(
    d2i_PrivateKey(EVP_PKEY_EC, nullptr, &rawKeyBytes, static_cast<long>(privateKeyBytes.size())));
  if (!key)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("d2i_PrivateKey"));
  }

  return key;
}

/**
 * Attempt to recover a public key using the given recovery ID and compare it to the expected key.
 *
 * @param ctx              The secp256k1 context.
 * @param compactSig       The compact signature (r || s) as 64 bytes.
 * @param recId            The recovery ID to try (0-3).
 * @param messageHash      The 32-byte Keccak-256 hash of the original message.
 * @param expectedPubKey   The expected uncompressed public key bytes (65 bytes).
 * @return True if the recovered public key matches the expected key.
 */
[[nodiscard]] bool tryRecoverWithId(const secp256k1_context* ctx,
                                    const unsigned char* compactSig,
                                    int recId,
                                    const unsigned char* messageHash,
                                    const std::vector<std::byte>& expectedPubKey)
{
  secp256k1_ecdsa_recoverable_signature recoverableSig;
  if (secp256k1_ecdsa_recoverable_signature_parse_compact(ctx, &recoverableSig, compactSig, recId) != 1)
  {
    return false;
  }

  secp256k1_pubkey recoveredPubKey;
  if (secp256k1_ecdsa_recover(ctx, &recoveredPubKey, &recoverableSig, messageHash) != 1)
  {
    return false;
  }

  // Serialize the recovered public key in uncompressed form.
  unsigned char serializedPubKey[ECDSAsecp256k1PublicKey::UNCOMPRESSED_KEY_SIZE];
  size_t outputLen = sizeof(serializedPubKey);
  secp256k1_ec_pubkey_serialize(ctx, serializedPubKey, &outputLen, &recoveredPubKey, SECP256K1_EC_UNCOMPRESSED);

  return outputLen == expectedPubKey.size() && std::memcmp(serializedPubKey, expectedPubKey.data(), outputLen) == 0;
}

} // namespace

//-----
std::unique_ptr<ECDSAsecp256k1PrivateKey> ECDSAsecp256k1PrivateKey::generatePrivateKey()
{
  internal::OpenSSLUtils::EVP_PKEY key(EVP_EC_gen("secp256k1"));
  if (!key)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_EC_gen"));
  }

  return std::make_unique<ECDSAsecp256k1PrivateKey>(ECDSAsecp256k1PrivateKey(std::move(key)));
}

//-----
std::unique_ptr<ECDSAsecp256k1PrivateKey> ECDSAsecp256k1PrivateKey::fromString(std::string_view key)
{
  std::string formattedKey = key.data();
  // Remove PEM prefix/suffix if is present and hex the base64 val
  if (formattedKey.compare(
        0, internal::asn1::PEM_ECPRK_PREFIX_STRING.size(), internal::asn1::PEM_ECPRK_PREFIX_STRING) == 0)
  {
    formattedKey = formattedKey.substr(internal::asn1::PEM_ECPRK_PREFIX_STRING.size(), formattedKey.size());

    if (formattedKey.compare(formattedKey.size() - internal::asn1::PEM_ECPRK_SUFFIX_STRING.size(),
                             formattedKey.size(),
                             internal::asn1::PEM_ECPRK_SUFFIX_STRING) == 0)
    {
      formattedKey = formattedKey.substr(0, formattedKey.size() - internal::asn1::PEM_ECPRK_SUFFIX_STRING.size());
    }
    formattedKey = internal::HexConverter::base64ToHex(formattedKey);
  }

  try
  {
    return std::make_unique<ECDSAsecp256k1PrivateKey>(
      ECDSAsecp256k1PrivateKey(bytesToPKEY(internal::HexConverter::hexToBytes(formattedKey))));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(std::string("ECDSAsecp256k1PrivateKey cannot be realized from input string: ") +
                          openSSLException.what());
  }
}

//-----
std::unique_ptr<ECDSAsecp256k1PrivateKey> ECDSAsecp256k1PrivateKey::fromBytes(const std::vector<std::byte>& bytes)
{
  try
  {
    return std::make_unique<ECDSAsecp256k1PrivateKey>(ECDSAsecp256k1PrivateKey(bytesToPKEY(bytes)));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(std::string("ECDSAsecp256k1PrivateKey cannot be realized from input bytes: ") +
                          openSSLException.what());
  }
}

//-----
std::unique_ptr<ECDSAsecp256k1PrivateKey> ECDSAsecp256k1PrivateKey::fromSeed(const std::vector<std::byte>& seed)
{
  try
  {
    const std::vector<std::byte> hmacOutput = internal::OpenSSLUtils::computeSHA512HMAC(BIP32_SEED, seed);

    // The hmac is the key bytes followed by the chain code bytes
    return std::make_unique<ECDSAsecp256k1PrivateKey>(
      ECDSAsecp256k1PrivateKey(bytesToPKEY({ hmacOutput.cbegin(), hmacOutput.cbegin() + KEY_SIZE }),
                               { hmacOutput.cbegin() + KEY_SIZE, hmacOutput.cbegin() + KEY_SIZE + CHAIN_CODE_SIZE }));
  }
  catch (const OpenSSLException& openSSLException)
  {
    throw BadKeyException(openSSLException.what());
  }
}

//-----
std::unique_ptr<Key> ECDSAsecp256k1PrivateKey::clone() const
{
  return std::make_unique<ECDSAsecp256k1PrivateKey>(*this);
}

//-----
std::unique_ptr<proto::Key> ECDSAsecp256k1PrivateKey::toProtobufKey() const
{
  return getPublicKey()->toProtobufKey();
}

//-----
std::unique_ptr<PrivateKey> ECDSAsecp256k1PrivateKey::derive(uint32_t childIndex) const
{
  if (getChainCode().empty())
  {
    throw UninitializedException("Key not initialized with chain code, unable to derive keys");
  }

  const std::vector<std::byte> hmacOutput = internal::OpenSSLUtils::computeSHA512HMAC(
    getChainCode(),
    internal::Utilities::concatenateVectors(
      { (internal::DerivationPathUtils::isHardenedChildIndex(childIndex))
          ? internal::Utilities::concatenateVectors({ { std::byte(0x0) }, toBytesRaw() })
          : getPublicKey()->toBytesRaw(),
        internal::DerivationPathUtils::indexToBigEndianArray(childIndex) }));

  // Modular add the private key bytes computed from the HMAC to the existing private key (using the secp256k1 curve
  // order as the modulo), and compute the new chain code from the HMAC
  return std::make_unique<ECDSAsecp256k1PrivateKey>(ECDSAsecp256k1PrivateKey(
    bytesToPKEY(internal::OpenSSLUtils::BIGNUM::fromBytes({ hmacOutput.cbegin(), hmacOutput.cbegin() + KEY_SIZE })
                  .modularAdd(internal::OpenSSLUtils::BIGNUM::fromBytes(toBytesRaw()), CURVE_ORDER)
                  .toBytes()),
    { hmacOutput.cbegin() + KEY_SIZE, hmacOutput.cbegin() + KEY_SIZE + CHAIN_CODE_SIZE }));
}

//-----
std::vector<std::byte> ECDSAsecp256k1PrivateKey::sign(const std::vector<std::byte>& bytesToSign) const
{
  internal::OpenSSLUtils::OSSL_LIB_CTX libraryContext(OSSL_LIB_CTX_new());
  if (!libraryContext)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("OSSL_LIB_CTX_new"));
  }

  const internal::OpenSSLUtils::EVP_MD messageDigest(EVP_MD_fetch(libraryContext.get(), "KECCAK-256", nullptr));
  if (!messageDigest)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_MD_fetch"));
  }

  internal::OpenSSLUtils::EVP_MD_CTX messageDigestContext(EVP_MD_CTX_new());
  if (!messageDigestContext)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_MD_CTX_new"));
  }

  if (EVP_DigestSignInit(messageDigestContext.get(), nullptr, messageDigest.get(), nullptr, getInternalKey().get()) <=
      0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestSignInit"));
  }

  // 72 is the maximum required size. actual signature may be slightly smaller
  size_t signatureLength = MAX_SIGNATURE_SIZE;
  std::vector<std::byte> signature(signatureLength);

  if (EVP_DigestSign(messageDigestContext.get(),
                     internal::Utilities::toTypePtr<unsigned char>(signature.data()),
                     &signatureLength,
                     internal::Utilities::toTypePtr<unsigned char>(bytesToSign.data()),
                     bytesToSign.size()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestSign"));
  }

  // we have the signature complete, now we need to turn it into its raw form of (r,s)

  const unsigned char* signaturePointer = internal::Utilities::toTypePtr<unsigned char>(signature.data());
  const internal::OpenSSLUtils::ECDSA_SIG signatureObject(
    d2i_ECDSA_SIG(nullptr, &signaturePointer, static_cast<long>(signatureLength)));
  if (!signatureObject)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("d2i_ECDSA_SIG"));
  }

  const BIGNUM* signatureR = ECDSA_SIG_get0_r(signatureObject.get());
  if (!signatureR)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("ECDSA_SIG_get0_r"));
  }

  const BIGNUM* signatureS = ECDSA_SIG_get0_s(signatureObject.get());
  if (!signatureS)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("ECDSA_SIG_get0_s"));
  }

  // signature is returned in the raw, 64 byte form (r, s)
  std::vector<std::byte> outputArray(RAW_SIGNATURE_SIZE);

  if (BN_bn2binpad(signatureR, internal::Utilities::toTypePtr<unsigned char>(outputArray.data()), R_SIZE) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("BN_bn2binpad"));
  }

  if (BN_bn2binpad(signatureS, internal::Utilities::toTypePtr<unsigned char>(outputArray.data()) + R_SIZE, S_SIZE) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("BN_bn2binpad"));
  }

  return outputArray;
}

//-----
int ECDSAsecp256k1PrivateKey::getRecoveryId(const std::vector<std::byte>& r,
                                            const std::vector<std::byte>& s,
                                            const std::vector<std::byte>& message) const
{
  // Validate input sizes.
  if (r.size() != R_SIZE || s.size() != S_SIZE)
  {
    return -1;
  }

  // Compute the Keccak-256 hash of the message.
  const std::vector<std::byte> messageHash = internal::OpenSSLUtils::computeKECCAK256(message);

  // Build compact signature (r || s) as 64-byte array.
  std::vector<unsigned char> compactSig(RAW_SIGNATURE_SIZE);
  std::memcpy(compactSig.data(), r.data(), R_SIZE);
  std::memcpy(compactSig.data() + R_SIZE, s.data(), S_SIZE);

  // Get the expected uncompressed public key bytes (65 bytes: 0x04 || x || y).
  const std::vector<std::byte> expectedPubKeyBytes =
    ECDSAsecp256k1PublicKey::uncompressBytes(getPublicKey()->toBytesRaw());

  // Create a secp256k1 context for verification/recovery using RAII.
  internal::OpenSSLUtils::Secp256k1Context ctx(secp256k1_context_create(SECP256K1_CONTEXT_NONE));
  if (!ctx)
  {
    return -1;
  }

  // Try each recovery ID (0-3) and check which one recovers our public key.
  for (int recId = 0; recId < 4; ++recId)
  {
    if (tryRecoverWithId(ctx.get(),
                         compactSig.data(),
                         recId,
                         reinterpret_cast<const unsigned char*>(messageHash.data()),
                         expectedPubKeyBytes))
    {
      return recId;
    }
  }

  return -1;
}

//-----
std::string ECDSAsecp256k1PrivateKey::toStringDer() const
{
  return internal::HexConverter::bytesToHex(toBytesDer());
}

//-----
std::string ECDSAsecp256k1PrivateKey::toStringRaw() const
{
  return internal::HexConverter::bytesToHex(toBytesRaw());
}

//-----
std::vector<std::byte> ECDSAsecp256k1PrivateKey::toBytes() const
{
  return toBytesDer();
}

//-----
std::vector<std::byte> ECDSAsecp256k1PrivateKey::toBytesDer() const
{
  return internal::Utilities::concatenateVectors({ DER_ENCODED_PREFIX_BYTES, toBytesRaw() });
}

//-----
std::vector<std::byte> ECDSAsecp256k1PrivateKey::toBytesRaw() const
{
  std::vector<std::byte> outputBytes(i2d_PrivateKey(getInternalKey().get(), nullptr));

  if (auto rawBytes = internal::Utilities::toTypePtr<unsigned char>(outputBytes.data());
      i2d_PrivateKey(getInternalKey().get(), &rawBytes) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("i2d_PrivateKey"));
  }

  // The return value of i2d_PrivateKey can be either 48 or 118 bytes, depending on how the private key was constructed.
  // This difference doesn't change anything with the return here: the first 7 bytes of each are algorithm identifiers,
  // the next 32 are private key bytes, and the rest are for other purposes.
  return { outputBytes.cbegin() + static_cast<long>(internal::asn1::ASN1_PRK_PREFIX_BYTES.size()),
           outputBytes.cbegin() + static_cast<long>(internal::asn1::ASN1_PRK_PREFIX_BYTES.size()) + KEY_SIZE };
}

//-----
ECDSAsecp256k1PrivateKey::ECDSAsecp256k1PrivateKey(internal::OpenSSLUtils::EVP_PKEY&& key,
                                                   std::vector<std::byte> chainCode)
  : PrivateKey(std::move(key), std::move(chainCode))
{
}

} // namespace Hiero
