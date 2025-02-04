// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_PRIVATE_KEY_IMPL_H_
#define HIERO_SDK_CPP_IMPL_PRIVATE_KEY_IMPL_H_

#include "PrivateKey.h"
#include "impl/openssl_utils/EVP_PKEY.h"

#include <memory>
#include <vector>

namespace Hiero
{
/**
 * The implementation of the PrivateKey object.
 */
struct PrivateKey::PrivateKeyImpl
{
  /**
   * The wrapped OpenSSL key object.
   */
  internal::OpenSSLUtils::EVP_PKEY mKey;

  /**
   * The chain code. If this is empty, then this PrivateKey will not support derivation.
   */
  std::vector<std::byte> mChainCode;

  /**
   * A pointer to the PublicKey object that corresponds to this PrivateKey.
   */
  std::shared_ptr<PublicKey> mPublicKey = nullptr;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_IMPL_PRIVATE_KEY_IMPL_H_
