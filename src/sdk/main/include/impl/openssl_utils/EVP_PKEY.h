// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_PKEY_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_PKEY_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/evp.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL EVP_PKEY object.
 */
class EVP_PKEY : public OpenSSLObjectWrapper<::EVP_PKEY, decltype(&EVP_PKEY_dup)>
{
public:
  EVP_PKEY() = default;

  /**
   * Construct with the input EVP_PKEY, its EVP_PKEY_free deleter function, and its EVP_PKEY_dup copier function.
   *
   * @param evpPkey The EVP_PKEY OpenSSL object to wrap.
   */
  explicit EVP_PKEY(::EVP_PKEY* evpPkey)
    : OpenSSLObjectWrapper(evpPkey, &EVP_PKEY_free, &EVP_PKEY_dup)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_PKEY_H_
