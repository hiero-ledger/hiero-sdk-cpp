// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LAMBDA_EVM_HOOK_H_
#define HIERO_SDK_CPP_LAMBDA_EVM_HOOK_H_

#include "hooks/LambdaStorageUpdate.h"
#include "hooks/EvmHookSpec.h"

#include <vector>

namespace Hiero
{
/**
 * Class to hold the definition of a lambda EVM hook.
 */
class LambdaEvmHook : public EvmHookSpec
{
public:
  /**
   * Add a storage update to this hook.
   *
   * @param storageUpdate The storage update to add.
   * @return A reference to this LambdaEvmHook object with the newly-added storage update.
   */
  LambdaEvmHook& addStorageUpdate(const LambdaStorageUpdate& storageUpdate);

  /**
   * Set the storage updates for this hook.
   *
   * @param storageUpdates The storage updates to set.
   * @return A reference to this LambdaEvmHook object with the newly-set storage updates.
   */
  LambdaEvmHook& setStorageUpdates(const std::vector<LambdaStorageUpdate>& storageUpdates);

  /**
   * Clear the storage updates for this hook.
   *
   * @return A reference to this LambdaEvmHook object with no storage updates.
   */
  LambdaEvmHook& clearStorageUpdates();

  /**
   * Get the storage updates for this hook.
   *
   * @return The storage updates for this hook.
   */
  [[nodiscard]] inline std::vector<LambdaStorageUpdate> getStorageUpdates() const { return mStorageUpdates; }

private:
  /**
   * The initial storage updates for the lambda, if any.
   */
   std::vector<LambdaStorageUpdate> mStorageUpdates;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LAMBDA_EVM_HOOK_H_