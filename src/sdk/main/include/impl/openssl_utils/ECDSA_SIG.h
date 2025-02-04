// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_ECDSA_SIG_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_ECDSA_SIG_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/ecdsa.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL ECDSA_SIG object.
 */
class ECDSA_SIG : public OpenSSLObjectWrapper<::ECDSA_SIG>
{
public:
  /**
   * No custom copier provided by OpenSSL for ECDSA_SIG.
   */
  ~ECDSA_SIG() override = default;
  ECDSA_SIG(const ECDSA_SIG&) = delete;
  ECDSA_SIG& operator=(const ECDSA_SIG&) = delete;
  ECDSA_SIG(ECDSA_SIG&&) = default;
  ECDSA_SIG& operator=(ECDSA_SIG&&) = default;

  /**
   * Construct with the input ECDSA_SIG and its ECDSA_SIG_free deleter function.
   *
   * @param ecdsaSig The ECDSA_SIG OpenSSL object to wrap.
   */
  explicit ECDSA_SIG(::ECDSA_SIG* ecdsaSig)
    : OpenSSLObjectWrapper(ecdsaSig, &ECDSA_SIG_free)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_ECDSA_SIG_H_
