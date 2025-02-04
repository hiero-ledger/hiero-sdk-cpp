// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_PKEY_CTX_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_PKEY_CTX_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/evp.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL EVP_PKEY_CTX object.
 */
class EVP_PKEY_CTX : public OpenSSLObjectWrapper<::EVP_PKEY_CTX>
{
public:
  /**
   * Construct with the input EVP_PKEY_CTX, its EVP_PKEY_CTX_free deleter function, and its EVP_PKEY_CTX_dup copier
   * function.
   *
   * @param evpPkeyCtx The EVP_PKEY_CTX OpenSSL object to wrap.
   */
  explicit EVP_PKEY_CTX(::EVP_PKEY_CTX* evpPkeyCtx)
    : OpenSSLObjectWrapper(evpPkeyCtx, &EVP_PKEY_CTX_free, &EVP_PKEY_CTX_dup)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_PKEY_CTX_H_
