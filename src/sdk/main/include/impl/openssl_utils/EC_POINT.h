// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EC_POINT_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EC_POINT_H_

#include "impl/openssl_utils/OpenSSLObjectWrapper.h"

#include <openssl/ec.h>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * Wrapper class for the OpenSSL EC_POINT object.
 */
class EC_POINT : public OpenSSLObjectWrapper<::EC_POINT>
{
public:
  /**
   * No custom copier provided by OpenSSL for EC_POINT.
   */
  ~EC_POINT() override = default;
  EC_POINT(const EC_POINT&) = delete;
  EC_POINT& operator=(const EC_POINT&) = delete;
  EC_POINT(EC_POINT&&) = default;
  EC_POINT& operator=(EC_POINT&&) = default;

  /**
   * Construct with the input EC_POINT and its EC_POINT_free deleter function.
   *
   * @param ecPoint The EC_POINT OpenSSL object to wrap.
   */
  explicit EC_POINT(::EC_POINT* ecPoint)
    : OpenSSLObjectWrapper(ecPoint, &EC_POINT_free)
  {
  }
};

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_EC_POINT_H_
