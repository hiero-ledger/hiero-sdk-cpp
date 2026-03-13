// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FUNGIBLE_HOOK_TYPE_H_
#define HIERO_SDK_CPP_FUNGIBLE_HOOK_TYPE_H_

#include <string>
#include <unordered_map>

namespace Hiero
{
/**
 * Enumeration specifying the different types of hooks for fungible tokens (including HBAR).
 */
enum class FungibleHookType
{
  /**
   * Execute the allowance hook before the transaction.
   */
  PRE_TX_ALLOWANCE_HOOK,

  /**
   * Execute the allowance hook before and after the transaction.
   */
  PRE_POST_TX_ALLOWANCE_HOOK,

  /**
   * Hook type not set.
   */
  UNINITIALIZED
};

/**
 * Map of FungibleHookType to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<FungibleHookType, std::string> gFungibleHookTypeToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_FUNGIBLE_HOOK_TYPE_H_