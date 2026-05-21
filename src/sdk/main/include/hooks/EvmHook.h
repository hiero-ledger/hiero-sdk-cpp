// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EVM_HOOK_H_
#define HIERO_SDK_CPP_EVM_HOOK_H_

#include "hooks/EvmHookSpec.h"
#include "hooks/EvmHookStorageUpdate.h"

#include <vector>

namespace com::hedera::hapi::node::hooks
{
class EvmHook;
}

namespace Hiero
{
/**
 * Class to hold the definition of an EVM hook.
 */
class EvmHook
{
public:
  /**
   * Construct a EvmHook object from a EvmHook protobuf object.
   *
   * @param proto The EvmHook protobuf object from which to create a EvmHook object.
   * @return The constructed EvmHook object.
   */
  [[nodiscard]] static EvmHook fromProtobuf(const com::hedera::hapi::node::hooks::EvmHook& proto);

  /**
   * Construct a EvmHook protobuf object from this EvmHook object.
   *
   * @return A pointer to the created EvmHook protobuf object filled with this EvmHook object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::EvmHook> toProtobuf() const;

  /**
   * Set the EVM hook spec.
   *
   * @param spec The EVM hook spec to set.
   * @return A reference to this EvmHook with the newly-set EVM hook spec.
   */
  EvmHook& setEvmHookSpec(const EvmHookSpec& spec);

  /**
   * Add a storage update to this hook.
   *
   * @param storageUpdate The storage update to add.
   * @return A reference to this EvmHook object with the newly-added storage update.
   */
  EvmHook& addStorageUpdate(const EvmHookStorageUpdate& storageUpdate);

  /**
   * Set the storage updates for this hook.
   *
   * @param storageUpdates The storage updates to set.
   * @return A reference to this EvmHook object with the newly-set storage updates.
   */
  EvmHook& setStorageUpdates(const std::vector<EvmHookStorageUpdate>& storageUpdates);

  /**
   * Clear the storage updates for this hook.
   *
   * @return A reference to this EvmHook object with no storage updates.
   */
  EvmHook& clearStorageUpdates();

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
  [[nodiscard]] inline std::vector<EvmHookStorageUpdate> getStorageUpdates() const { return mStorageUpdates; }

private:
  /**
   * The specification for the hook.
   */
  EvmHookSpec mEvmHookSpec;

  /**
   * The initial storage updates for the EVM hook, if any.
   */
  std::vector<EvmHookStorageUpdate> mStorageUpdates;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EVM_HOOK_H_