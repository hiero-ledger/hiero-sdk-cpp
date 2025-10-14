// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HOOK_TYPE_H_
#define HIERO_SDK_CPP_HOOK_TYPE_H_

#include <unordered_map>

namespace Hiero
{
/**
 * Enumeration specifying the different types of hooks.
 */
enum class HookType
{
  /**
   * Execute the hook before the transaction.
   */
  PRE_HOOK,

  /**
   * Execute the hook before and after the transaction.
   */
  PRE_POST_HOOK,

  /**
   * Execute the hook for the sender before the transaction.
   */
  PRE_HOOK_SENDER,

  /**
   * Execute the hook for the sender before and after the transaction.
   */
  PRE_POST_HOOK_SENDER,

  /**
   * Execute the hook for the receiver before the transaction.
   */
  PRE_HOOK_RECEIVER,

  /**
   * Execute the hook for the receiver before and after the transaction.
   */
  PRE_POST_HOOK_RECEIVER
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

#endif // HIERO_SDK_CPP_HOOK_TYPE_H_