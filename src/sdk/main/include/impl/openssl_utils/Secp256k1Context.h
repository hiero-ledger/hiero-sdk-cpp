// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_SECP256K1_CONTEXT_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_SECP256K1_CONTEXT_H_

#include "impl/CryptoObjectWrapper.h"

#include <secp256k1.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the secp256k1_context object.
 */
class Secp256k1Context : public CryptoObjectWrapper<::secp256k1_context>
{
public:
  /**
   * No custom copier provided for secp256k1_context.
   */
  ~Secp256k1Context() override = default;
  Secp256k1Context(const Secp256k1Context&) = delete;
  Secp256k1Context& operator=(const Secp256k1Context&) = delete;
  Secp256k1Context(Secp256k1Context&&) = default;
  Secp256k1Context& operator=(Secp256k1Context&&) = default;

  /**
   * Construct with the input secp256k1_context and its secp256k1_context_destroy deleter function.
   *
   * @param ctx The secp256k1_context object to wrap.
   */
  explicit Secp256k1Context(::secp256k1_context* ctx)
    : CryptoObjectWrapper(ctx, &secp256k1_context_destroy)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_SECP256K1_CONTEXT_H_
