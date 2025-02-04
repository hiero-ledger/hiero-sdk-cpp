// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_MD_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_MD_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/evp.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL EVP_MD object.
 */
class EVP_MD : public OpenSSLObjectWrapper<::EVP_MD>
{
public:
  /**
   * No custom copier provided by OpenSSL for EVP_MD.
   */
  ~EVP_MD() override = default;
  EVP_MD(const EVP_MD&) = delete;
  EVP_MD& operator=(const EVP_MD&) = delete;
  EVP_MD(EVP_MD&&) = default;
  EVP_MD& operator=(EVP_MD&&) = default;

  /**
   * Construct with the input EVP_MD and its EVP_MD_free deleter function.
   *
   * @param evpMd The EVP_MD OpenSSL object to wrap.
   */
  explicit EVP_MD(::EVP_MD* evpMd)
    : OpenSSLObjectWrapper(evpMd, &EVP_MD_free)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_MD_H_
