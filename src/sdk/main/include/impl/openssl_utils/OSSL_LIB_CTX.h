// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OSSL_LIB_CTX_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OSSL_LIB_CTX_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/crypto.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL OSSL_LIB_CTX object.
 */
class OSSL_LIB_CTX : public OpenSSLObjectWrapper<::OSSL_LIB_CTX>
{
public:
  /**
   * No custom copier provided by OpenSSL for OSSL_LIB_CTX.
   */
  ~OSSL_LIB_CTX() override = default;
  OSSL_LIB_CTX(const OSSL_LIB_CTX&) = delete;
  OSSL_LIB_CTX& operator=(const OSSL_LIB_CTX&) = delete;
  OSSL_LIB_CTX(OSSL_LIB_CTX&&) = default;
  OSSL_LIB_CTX& operator=(OSSL_LIB_CTX&&) = default;

  /**
   * Construct with the input OSSL_LIB_CTX and its OSSL_LIB_CTX_free deleter function.
   *
   * @param osslLibCtx The OSSL_LIB_CTX OpenSSL object to wrap.
   */
  explicit OSSL_LIB_CTX(::OSSL_LIB_CTX* osslLibCtx)
    : OpenSSLObjectWrapper(osslLibCtx, &OSSL_LIB_CTX_free)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OSSL_LIB_CTX_H_
