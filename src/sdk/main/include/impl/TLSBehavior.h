// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_TLS_BEHAVIOR_H_
#define HIERO_SDK_CPP_IMPL_TLS_BEHAVIOR_H_

namespace Hiero::internal
{
/**
 * Enum representing different modes of network communication.
 */
enum class TLSBehavior
{
  /**
   * Communicate only in the clear
   */
  DISABLE,
  /**
   * Require TLS connection
   */
  REQUIRE
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_TLS_BEHAVIOR_H_
