// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LAMBDA_MAPPING_ENTRIES_H_
#define HIERO_SDK_CPP_LAMBDA_MAPPING_ENTRIES_H_

#include "hooks/LambdaMappingEntry.h"

#include <cstddef>
#include <vector>

namespace com::hedera::hapi::node::hooks
{
class LambdaMappingEntries;
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
class LambdaMappingEntries
{
public:
  /**
   * Construct a LambdaMappingEntries object from a LambdaMappingEntries protobuf object.
   *
   * @param proto The LambdaMappingEntries protobuf object from which to create a LambdaMappingEntries object.
   * @return The constructed LambdaMappingEntries object.
   */
  [[nodiscard]] static LambdaMappingEntries fromProtobuf(const com::hedera::hapi::node::hooks::LambdaMappingEntries& proto);

  /**
   * Construct a LambdaMappingEntries protobuf object from this LambdaMappingEntries object.
   *
   * @return A pointer to the created LambdaMappingEntries protobuf object filled with this LambdaMappingEntries
   *         object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntries> toProtobuf() const;

  /**
   * Set the Solidity mapping slot.
   *
   * @param mappingSlot The Solidity mapping slot.
   * @return A reference to this LambdaMappingEntries object with the newly-set mapping slot.
   */
  LambdaMappingEntries& setMappingSlot(const std::vector<std::byte>& mappingSlot);

  /**
   * Add a mapping entry.
   *
   * @param entry The mapping entry to add.
   * @return A reference to this LambdaMappingEntries object with the newly-added entry.
   */
  LambdaMappingEntries& addEntry(const LambdaMappingEntry& entry);

  /**
   * Set the mapping entries.
   *
   * @param entries The mapping entries.
   * @return A reference to this LambdaMappingEntries object with the newly-set mapping entries.
   */
  LambdaMappingEntries& setEntries(const std::vector<LambdaMappingEntry>& entries);

  /**
   * Clear the mapping entries.
   *
   * @return A reference to this LambdaMappingEntries object with the cleared mapping entries.
   */
  LambdaMappingEntries& clearEntries();

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
  [[nodiscard]] inline std::vector<LambdaMappingEntry> getEntries() const { return mEntries; }

private:
  /**
   * The slot corresponding to the Solidity mapping.
   */
  std::vector<std::byte> mMappingSlot;

  /**
   * The 32-byte key of the mapping entry.
   */
  std::vector<LambdaMappingEntry> mEntries;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LAMBDA_MAPPING_ENTRIES_H_