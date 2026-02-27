// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HOOK_EXTENSION_POINT_H_
#define HIERO_SDK_CPP_HOOK_EXTENSION_POINT_H_

#include <unordered_map>

namespace com::hedera::hapi::node::hooks
{
enum HookExtensionPoint : int;
}

namespace Hiero
{
/**
 * Enumeration specifying the different extension points for a hook.
 */
enum class HookExtensionPoint
{
  /**
   * Hook for account allowance.
   */
  ACCOUNT_ALLOWANCE_HOOK,
};

/**
 * Map of protobuf HookExtensionPoint to the corresponding HookExtensionPoint.
 */
extern const std::unordered_map<com::hedera::hapi::node::hooks::HookExtensionPoint, HookExtensionPoint>
  gProtobufHookExtensionPointToHookExtensionPoint;

/**
 * Map of HookExtensionPoint to its corresponding HookExtensionPoint protobuf.
 */
extern const std::unordered_map<HookExtensionPoint, com::hedera::hapi::node::hooks::HookExtensionPoint>
  gHookExtensionPointToProtobufHookExtensionPoint;

} // namespace Hiero

#endif // HIERO_SDK_CPP_HOOK_EXTENSION_POINT_H_