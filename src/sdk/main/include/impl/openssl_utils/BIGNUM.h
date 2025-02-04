// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_BIGNUM_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_BIGNUM_H_

#include "exceptions/OpenSSLException.h"
#include "impl/HexConverter.h"
#include "impl/openssl_utils/BN_CTX.h"
#include "impl/openssl_utils/OpenSSLObjectWrapper.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <openssl/bn.h>
#include <string_view>
#include <vector>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL BIGNUM object.
 */
class BIGNUM : public OpenSSLObjectWrapper<::BIGNUM>
{
public:
  /**
   * Construct with the input BIGNUM, its BN_clear_free deleter, and its BN_dup copier.
   *
   * @param bignum The BIGNUM OpenSSL object to wrap.
   */
  explicit BIGNUM(::BIGNUM* bignum)
    : OpenSSLObjectWrapper(bignum, &BN_clear_free, &BN_dup)
  {
  }

  /**
   * Create a new BIGNUM from a hex string.
   *
   * @param hexString A string representing the BIGNUM.
   * @return A newly constructed BIGNUM.
   */
  [[nodiscard]] static BIGNUM fromHex(std::string_view hexString)
  {
    ::BIGNUM* bigNum = nullptr;
    if (BN_hex2bn(&bigNum, hexString.data()) <= 0)
    {
      throw OpenSSLException(getErrorMessage("BN_hex2bn"));
    }

    return BIGNUM(bigNum);
  }

  /**
   * Create a new BIGNUM from a bytes vector.
   *
   * @param bytes The vector of bytes representing the BIGNUM.
   * @return A newly constructed BIGNUM.
   */
  [[nodiscard]] static BIGNUM fromBytes(const std::vector<std::byte>& bytes)
  {
    return fromHex(HexConverter::bytesToHex(bytes));
  }

  /**
   * Add another BIGNUM to this one, and take the modulo of the sum.
   *
   * @param other  The other BIGNUM to add to this one.
   * @param modulo The modulo to take the sum to.
   * @return The modular sum.
   */
  [[nodiscard]] BIGNUM modularAdd(const BIGNUM& other, const BIGNUM& modulo) const
  {
    BN_CTX context(BN_CTX_secure_new());
    if (!context)
    {
      throw OpenSSLException(getErrorMessage("BN_CTX_secure_new"));
    }

    BIGNUM result(BN_secure_new());
    if (!result)
    {
      throw OpenSSLException(getErrorMessage("BN_secure_new"));
    }

    if (BN_mod_add(result.get(), get(), other.get(), modulo.get(), context.get()) <= 0)
    {
      throw OpenSSLException(getErrorMessage("BN_mod_add"));
    }

    return result;
  }

  /**
   * Get a vector of bytes representing this BIGNUM.
   *
   * @return The byte vector representing this BIGNUM.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const
  {
    char* hex = BN_bn2hex(get());
    const std::string hexString(hex);
    OPENSSL_free(hex);

    return HexConverter::hexToBytes(hexString);
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_BIGNUM_H_
