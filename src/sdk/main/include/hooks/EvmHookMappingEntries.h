// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EVM_HOOK_MAPPING_ENTRIES_H_
#define HIERO_SDK_CPP_EVM_HOOK_MAPPING_ENTRIES_H_

#include "hooks/EvmHookMappingEntry.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace com::hedera::hapi::node::hooks
{
class EvmHookMappingEntries;
}

namespace Hiero
{
/**
 * Specifies storage slot updates via indirection into a Solidity mapping.
 *
 * Concretely, if the Solidity mapping is itself at slot `mapping_slot`, then the * storage slot for key `key` in the
 * mapping is defined by the relationship `key_storage_slot = keccak256(abi.encodePacked(mapping_slot, key))`.
 *
 * This message lets a metaprotocol be specified in terms of changes to a Solidity mapping's entries. If only raw slots
 * could be updated, then a block stream consumer following the metaprotocol would have to invert the Keccak256 hash to
 * determine which mapping entry was being updated, which is not possible.
 */
class EvmHookMappingEntries
{
public:
  /**
   * Construct a EvmHookMappingEntries object from a EvmHookMappingEntries protobuf object.
   *
   * @param proto The EvmHookMappingEntries protobuf object from which to create a EvmHookMappingEntries object.
   * @return The constructed EvmHookMappingEntries object.
   */
  [[nodiscard]] static EvmHookMappingEntries fromProtobuf(
    const com::hedera::hapi::node::hooks::EvmHookMappingEntries& proto);

  /**
   * Construct a EvmHookMappingEntries protobuf object from this EvmHookMappingEntries object.
   *
   * @return A pointer to the created EvmHookMappingEntries protobuf object filled with this EvmHookMappingEntries
   *         object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookMappingEntries> toProtobuf() const;

  /**
   * Set the Solidity mapping slot.
   *
   * @param mappingSlot The Solidity mapping slot.
   * @return A reference to this EvmHookMappingEntries object with the newly-set mapping slot.
   */
  EvmHookMappingEntries& setMappingSlot(const std::vector<std::byte>& mappingSlot);

  /**
   * Add a mapping entry.
   *
   * @param entry The mapping entry to add.
   * @return A reference to this EvmHookMappingEntries object with the newly-added entry.
   */
  EvmHookMappingEntries& addEntry(const EvmHookMappingEntry& entry);

  /**
   * Set the mapping entries.
   *
   * @param entries The mapping entries.
   * @return A reference to this EvmHookMappingEntries object with the newly-set mapping entries.
   */
  EvmHookMappingEntries& setEntries(const std::vector<EvmHookMappingEntry>& entries);

  /**
   * Clear the mapping entries.
   *
   * @return A reference to this EvmHookMappingEntries object with the cleared mapping entries.
   */
  EvmHookMappingEntries& clearEntries();

  /**
   * Get the Solidity mapping slot.
   *
   * @return The Solidity mapping slot.
   */
  [[nodiscard]] inline std::vector<std::byte> getMappingSlot() const { return mMappingSlot; }

  /**
   * Get the Solidity mapping entries.
   *
   * @return The Solidity mapping entries.
   */
  [[nodiscard]] inline std::vector<EvmHookMappingEntry> getEntries() const { return mEntries; }

private:
  /**
   * The slot corresponding to the Solidity mapping.
   */
  std::vector<std::byte> mMappingSlot;

  /**
   * The 32-byte key of the mapping entry.
   */
  std::vector<EvmHookMappingEntry> mEntries;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EVM_HOOK_MAPPING_ENTRIES_H_