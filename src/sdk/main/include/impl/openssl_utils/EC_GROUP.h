// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EC_GROUP_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EC_GROUP_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/ec.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL EC_GROUP object.
 */
class EC_GROUP : public OpenSSLObjectWrapper<::EC_GROUP>
{
public:
  /**
   * Construct with the input EC_GROUP, its EC_GROUP_free deleter, and its EC_GROUP_dup copier.
   *
   * @param ecGroup The EC_GROUP OpenSSL object to wrap.
   */
  explicit EC_GROUP(::EC_GROUP* ecGroup)
    : OpenSSLObjectWrapper(ecGroup, &EC_GROUP_free, &EC_GROUP_dup)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EC_GROUP_H_