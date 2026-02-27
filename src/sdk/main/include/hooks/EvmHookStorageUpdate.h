// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EVM_HOOK_STORAGE_UPDATE_H_
#define HIERO_SDK_CPP_EVM_HOOK_STORAGE_UPDATE_H_

#include "hooks/EvmHookMappingEntries.h"
#include "hooks/EvmHookStorageSlot.h"

#include <memory>
#include <optional>

namespace com::hedera::hapi::node::hooks
{
class EvmHookStorageUpdate;
}

namespace Hiero
{
/**
 * Specifies a key/value pair in the storage of an EVM hook, either by the explicit storage slot contents; or by a
 * combination of a Solidity mapping's slot key and the key into that mapping.
 */
class EvmHookStorageUpdate
{
public:
  /**
   * Construct a EvmHookStorageUpdate object from a EvmHookStorageUpdate protobuf object.
   *
   * @param proto The EvmHookStorageUpdate protobuf object from which to create a EvmHookStorageUpdate object.
   * @return The constructed HookId object.
   */
  [[nodiscard]] static EvmHookStorageUpdate fromProtobuf(
    const com::hedera::hapi::node::hooks::EvmHookStorageUpdate& proto);

  /**
   * Construct a EvmHookStorageUpdate protobuf object from this HookId object.
   *
   * @return A pointer to the created EvmHookStorageUpdate protobuf object filled with this EvmHookStorageUpdate
   *         object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookStorageUpdate> toProtobuf() const;

  /**
   * Set an update for an explicit storage slot. It also resets any value for a slot update using a Solidity mapping
   * entry.
   *
   * @param storageSlot The explicit storage slot.
   * @return A reference to this EvmHookStorageUpdate object with the newly-set explicit storage slot update.
   */
  EvmHookStorageUpdate& setStorageSlot(const EvmHookStorageSlot& storageSlot);

  /**
   * Set an update for Solidity mapped entries. It also resets any value for an explicit storage update.
   *
   * @param mappingEntries The update for Solidity mapped entries.
   * @return A reference to this EvmHookStorageUpdate object with the newly-set Solidity mapped storage update.
   */
  EvmHookStorageUpdate& setMappingEntries(const EvmHookMappingEntries& mappingEntries);

  /**
   * Get the explicit storage slot update.
   *
   * @return The explicit storage slot update.
   */
  [[nodiscard]] inline std::optional<EvmHookStorageSlot> getStorageSlot() const { return mStorageSlot; }

  /**
   * Get the Solidity mapped entry update.
   *
   * @return The Solidity mapped entry update.
   */
  [[nodiscard]] inline std::optional<EvmHookMappingEntries> getMappingEntries() const { return mMappingEntries; }

private:
  /**
   * An explicit storage slot update.
   */
  std::optional<EvmHookStorageSlot> mStorageSlot;

  /**
   * An implicit storage slot update specified as Solidity mapping entries.
   */
  std::optional<EvmHookMappingEntries> mMappingEntries;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EVM_HOOK_STORAGE_UPDATE_H_