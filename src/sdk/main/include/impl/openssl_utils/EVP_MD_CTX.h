// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_MD_CTX_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_MD_CTX_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/evp.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL EVP_MD_CTX object.
 */
class EVP_MD_CTX : public OpenSSLObjectWrapper<::EVP_MD_CTX>
{
public:
  /**
   * No custom copier provided by OpenSSL for EVP_MD_CTX.
   */
  ~EVP_MD_CTX() override = default;
  EVP_MD_CTX(const EVP_MD_CTX&) = delete;
  EVP_MD_CTX& operator=(const EVP_MD_CTX&) = delete;
  EVP_MD_CTX(EVP_MD_CTX&&) = default;
  EVP_MD_CTX& operator=(EVP_MD_CTX&&) = default;

  /**
   * Construct with the input EVP_MD_CTX and its EVP_MD_CTX_free deleter function.
   *
   * @param evpMdCtx The EVP_MD_CTX OpenSSL object to wrap.
   */
  explicit EVP_MD_CTX(::EVP_MD_CTX* evpMdCtx)
    : OpenSSLObjectWrapper(evpMdCtx, &EVP_MD_CTX_free)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EVP_MD_CTX_H_
