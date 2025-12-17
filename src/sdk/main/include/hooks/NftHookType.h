// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NFT_HOOK_TYPE_H_
#define HIERO_SDK_CPP_NFT_HOOK_TYPE_H_

#include <string>
#include <unordered_map>

namespace Hiero
{
/**
 * Enumeration specifying the different types of hooks for NFTs.
 */
enum class NftHookType
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
   * Hook type uninitialized.
   */
  UNINITIALIZED
};

/**
 * Map of NftHookType to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<NftHookType, std::string> gNftHookTypeToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_NFT_HOOK_TYPE_H_