// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_PUBLIC_KEY_IMPL_H_
#define HIERO_SDK_CPP_IMPL_PUBLIC_KEY_IMPL_H_

#include "PublicKey.h"
#include "impl/openssl_utils/EVP_PKEY.h"

namespace Hiero
{
/**
 * The implementation of the PublicKey object.
 */
struct PublicKey::PublicKeyImpl
{
  /**
   * The wrapped OpenSSL key object.
   */
  internal::OpenSSLUtils::EVP_PKEY mKey;
};
} // namespace Hiero

#endif // HIERO_SDK_CPP_IMPL_PUBLIC_KEY_IMPL_H_
