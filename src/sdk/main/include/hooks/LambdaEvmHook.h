// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LAMBDA_EVM_HOOK_H_
#define HIERO_SDK_CPP_LAMBDA_EVM_HOOK_H_

#include "hooks/EvmHookSpec.h"
#include "hooks/LambdaStorageUpdate.h"

#include <vector>

namespace com::hedera::hapi::node::hooks
{
class LambdaEvmHook;
}

namespace Hiero
{
/**
 * Class to hold the definition of a lambda EVM hook.
 */
class LambdaEvmHook
{
public:
  /**
   * Construct a LambdaEvmHook object from a LambdaEvmHook protobuf object.
   *
   * @param proto The LambdaEvmHook protobuf object from which to create a LambdaEvmHook object.
   * @return The constructed LambdaEvmHook object.
   */
  [[nodiscard]] static LambdaEvmHook fromProtobuf(const com::hedera::hapi::node::hooks::LambdaEvmHook& proto);

  /**
   * Construct a LambdaEvmHook protobuf object from this LambdaEvmHook object.
   *
   * @return A pointer to the created LambdaEvmHook protobuf object filled with this LambdaEvmHook object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::LambdaEvmHook> toProtobuf() const;

  /**
   * Set the EVM hook spec.
   *
   * @param spec The EVM hook spec to set.
   * @return A reference to this LambdaEvmHook with the newly-set EVM hook spec.
   */
  LambdaEvmHook& setEvmHookSpec(const EvmHookSpec& spec);

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
   * Get the EVM hook spec for this hook.
   *
   * @return The EVM hook spec for this hook.
   */
  [[nodiscard]] inline EvmHookSpec getEvmHookSpec() const { return mEvmHookSpec; }

  /**
   * Get the storage updates for this hook.
   *
   * @return The storage updates for this hook.
   */
  [[nodiscard]] inline std::vector<LambdaStorageUpdate> getStorageUpdates() const { return mStorageUpdates; }

private:
  /**
   * The specification for the hook.
   */
  EvmHookSpec mEvmHookSpec;

  /**
   * The initial storage updates for the lambda, if any.
   */
  std::vector<LambdaStorageUpdate> mStorageUpdates;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LAMBDA_EVM_HOOK_H_