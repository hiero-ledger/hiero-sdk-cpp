// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OSSL_DECODER_CTX_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OSSL_DECODER_CTX_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/decoder.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL OSSL_DECODER_CTX object.
 */
class OSSL_DECODER_CTX : public OpenSSLObjectWrapper<::OSSL_DECODER_CTX>
{
public:
  /**
   * No custom copier provided by OpenSSL for OSSL_LIB_CTX.
   */
  ~OSSL_DECODER_CTX() override = default;
  OSSL_DECODER_CTX(const OSSL_DECODER_CTX&) = delete;
  OSSL_DECODER_CTX& operator=(const OSSL_DECODER_CTX&) = delete;
  OSSL_DECODER_CTX(OSSL_DECODER_CTX&&) = default;
  OSSL_DECODER_CTX& operator=(OSSL_DECODER_CTX&&) = default;

  /**
   * Construct with the input OSSL_DECODER_CTX and its OSSL_DECODER_CTX_free deleter function.
   *
   * @param osslDecoderCtx The OSSL_DECODER_CTX OpenSSL object to wrap.
   */
  explicit OSSL_DECODER_CTX(::OSSL_DECODER_CTX* osslDecoderCtx)
    : OpenSSLObjectWrapper(osslDecoderCtx, &OSSL_DECODER_CTX_free)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OSSL_DECODER_CTX_H_
