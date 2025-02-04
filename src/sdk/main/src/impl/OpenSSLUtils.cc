// SPDX-License-Identifier: Apache-2.0
#include "impl/openssl_utils/OpenSSLUtils.h"
#include "exceptions/OpenSSLException.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/EVP_MD.h"
#include "impl/openssl_utils/EVP_MD_CTX.h"
#include "impl/openssl_utils/OSSL_LIB_CTX.h"

#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdexcept>

namespace Hiero::internal::OpenSSLUtils
{
//-----
std::vector<std::byte> computeSHA256(const std::vector<std::byte>& data)
{
  auto outputBytes = std::vector<std::byte>(SHA256_HASH_SIZE);
  SHA256(Utilities::toTypePtr<unsigned char>(data.data()),
         data.size(),
         Utilities::toTypePtr<unsigned char>(outputBytes.data()));
  return outputBytes;
}

//-----
std::vector<std::byte> computeSHA384(const std::vector<std::byte>& data)
{
  auto outputBytes = std::vector<std::byte>(SHA384_HASH_SIZE);
  SHA384(Utilities::toTypePtr<unsigned char>(data.data()),
         data.size(),
         Utilities::toTypePtr<unsigned char>(outputBytes.data()));
  return outputBytes;
}

//-----
std::vector<std::byte> computeKECCAK256(const std::vector<std::byte>& data)
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

  if (EVP_DigestInit(messageDigestContext.get(), messageDigest.get()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestInit_ex"));
  }

  if (EVP_DigestUpdate(
        messageDigestContext.get(), internal::Utilities::toTypePtr<unsigned char>(data.data()), data.size()) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestUpdate"));
  }

  std::vector<std::byte> hash;
  hash.resize(KECCAK256_HASH_SIZE);
  if (EVP_DigestFinal(
        messageDigestContext.get(), internal::Utilities::toTypePtr<unsigned char>(hash.data()), nullptr) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("EVP_DigestFinal_ex"));
  }

  return hash;
}

//-----
std::vector<std::byte> computeSHA512HMAC(const std::vector<std::byte>& key, const std::vector<std::byte>& data)
{
  EVP_MD_CTX messageDigestContext(EVP_MD_CTX_new());
  if (!messageDigestContext)
  {
    throw OpenSSLException(getErrorMessage("EVP_MD_CTX_new"));
  }

  const EVP_MD messageDigest(EVP_MD_fetch(nullptr, "SHA512", nullptr));
  if (!messageDigest)
  {
    throw OpenSSLException(getErrorMessage("EVP_MD_fetch"));
  }

  if (EVP_DigestInit(messageDigestContext.get(), messageDigest.get()) <= 0)
  {
    throw OpenSSLException(getErrorMessage("EVP_DigestInit"));
  }

  std::vector<std::byte> digest(SHA512_HMAC_HASH_SIZE);
  if (!HMAC(messageDigest.get(),
            key.data(),
            static_cast<int>(key.size()),
            Utilities::toTypePtr<unsigned char>(data.data()),
            static_cast<int>(data.size()),
            Utilities::toTypePtr<unsigned char>(digest.data()),
            nullptr))
  {
    throw OpenSSLException(getErrorMessage("HMAC"));
  }

  return digest;
}

//-----
std::string getErrorMessage(std::string_view functionName)
{
  const unsigned long errorCode = ERR_get_error();

  // no error was found
  if (errorCode == 0)
  {
    return std::string("Error occurred in [").append(functionName).append("]");
  }

  std::string errorString(ERROR_MSG_SIZE, '\0');
  ERR_error_string_n(errorCode, errorString.data(), ERROR_MSG_SIZE);

  return std::string("Error occurred in [").append(functionName).append("]: ") + errorString;
}

//-----
std::vector<std::byte> getRandomBytes(int count)
{
  if (count <= 0)
  {
    throw std::invalid_argument("The number of random bytes to generate must be positive");
  }

  std::vector<std::byte> randomBytes(count);

  if (RAND_priv_bytes(Utilities::toTypePtr<unsigned char>(randomBytes.data()), count) <= 0)
  {
    throw OpenSSLException(getErrorMessage("RAND_priv_bytes"));
  }

  return randomBytes;
}

} // namespace Hiero::internal::OpenSSLUtils
