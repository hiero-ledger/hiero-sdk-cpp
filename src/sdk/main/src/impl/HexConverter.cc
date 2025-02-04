// SPDX-License-Identifier: Apache-2.0
#include "impl/HexConverter.h"
#include "exceptions/OpenSSLException.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>

#include <stdexcept>

namespace Hiero::internal::HexConverter
{
//-----
std::string bytesToHex(const std::vector<std::byte>& bytes)
{
  size_t stringLength;
  if (OPENSSL_buf2hexstr_ex(
        nullptr, 0, &stringLength, Utilities::toTypePtr<unsigned char>(bytes.data()), bytes.size(), '\0') <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("OPENSSL_buf2hexstr_ex"));
  }

  std::string charString(stringLength, '\0');
  if (OPENSSL_buf2hexstr_ex(charString.data(),
                            stringLength,
                            nullptr,
                            Utilities::toTypePtr<unsigned char>(bytes.data()),
                            bytes.size(),
                            '\0') <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("OPENSSL_buf2hexstr_ex"));
  }

  charString.pop_back(); // Remove the '\0'
  return charString;
}

//-----
std::vector<std::byte> hexToBytes(std::string_view hex)
{
  size_t bufferLength;
  if (OPENSSL_hexstr2buf_ex(nullptr, 0, &bufferLength, hex.data(), '\0') <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("OPENSSL_hexstr2buf_ex"));
  }

  std::vector<std::byte> outputBytes(bufferLength);
  if (OPENSSL_hexstr2buf_ex(
        Utilities::toTypePtr<unsigned char>(outputBytes.data()), bufferLength, nullptr, hex.data(), '\0') <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("OPENSSL_hexstr2buf_ex"));
  }

  return outputBytes;
}

//-----
std::string base64ToHex(std::string_view base64)
{

  BIO* b64 = BIO_new(BIO_f_base64());
  BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
  BIO* bmem = BIO_new_mem_buf(base64.data(), base64.length());
  bmem = BIO_push(b64, bmem);

  std::vector<unsigned char> binaryData;
  char buffer[4096]; // Some buffer size

  int bytesRead = 0;
  while ((bytesRead = BIO_read(bmem, buffer, sizeof(buffer))) > 0)
  {
    binaryData.insert(binaryData.end(), buffer, buffer + bytesRead);
  }

  BIO_free_all(bmem);

  std::string hexString;
  hexString.reserve(binaryData.size() * 2);

  for (unsigned char byte : binaryData)
  {
    char hex[3];
    snprintf(hex, sizeof(hex), "%02X", byte);
    hexString.append(hex);
  }

  return hexString;
}

} // namespace Hiero::internal::HexConverter
