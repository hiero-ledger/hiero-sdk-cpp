// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_BN_CTX_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_BN_CTX_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/bn.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL BN_CTX object.
 */
class BN_CTX : public OpenSSLObjectWrapper<::BN_CTX>
{
public:
  /**
   * No custom copier provided by OpenSSL for BN_CTX, so copying is disallowed.
   */
  ~BN_CTX() override = default;
  BN_CTX(const BN_CTX&) = delete;
  BN_CTX& operator=(const BN_CTX&) = delete;
  BN_CTX(BN_CTX&&) = default;
  BN_CTX& operator=(BN_CTX&&) = default;

  /**
   * Construct with the input BN_CTX and its BN_CTX_free deleter function.
   *
   * @param bnCtx The BN_CTX OpenSSL object to wrap.
   */
  explicit BN_CTX(::BN_CTX* bnCtx)
    : OpenSSLObjectWrapper(bnCtx, &BN_CTX_free)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_BN_CTX_H_