// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LAMBDA_MAPPING_ENTRY_H_
#define HIERO_SDK_CPP_LAMBDA_MAPPING_ENTRY_H_

#include <cstddef>
#include <vector>

namespace com::hedera::hapi::node::hooks
{
class LambdaMappingEntry;
}

namespace Hiero
{
/**
 * An implicit storage slot specified as a Solidity mapping entry.
 */
class LambdaMappingEntry
{
public:
  /**
   * Construct a LambdaMappingEntry object from a LambdaMappingEntry protobuf object.
   *
   * @param proto The LambdaMappingEntry protobuf object from which to create a LambdaMappingEntry object.
   * @return The constructed LambdaMappingEntry object.
   */
  [[nodiscard]] static LambdaMappingEntry fromProtobuf(const com::hedera::hapi::node::hooks::LambdaMappingEntry& proto);

  /**
   * Construct a LambdaMappingEntry protobuf object from this LambdaMappingEntry object.
   *
   * @return A pointer to the created LambdaMappingEntry protobuf object filled with this LambdaMappingEntry
   *         object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntry> toProtobuf() const;

  /**
   * Set the Solidity mapping slot.
   *
   * @param mappingSlot The Solidity mapping slot.
   * @return A reference to this LambdaMappingEntry object with the newly-set mapping slot.
   */
  LambdaMappingEntry& setMappingSlot(const std::vector<std::byte>& mappingSlot);

  /**
   * Set the key for the mapping entry.
   *
   * @param key The key for the mapping entry.
   * @return A reference to this LambdaMappingEntry object with the newly-set mapping entry key.
   */
  LambdaMappingEntry& setKey(const std::vector<std::byte>& key);

  /**
   * Set the value for the mapping entry.
   *
   * @param value The value for the mapping entry.
   * @return A reference to this LambdaMappingEntry object with the newly-set mapping entry value.
   */
  LambdaMappingEntry& setValue(const std::vector<std::byte>& value);

  /**
   * Get the Solidity mapping slot.
   *
   * @return The Solidity mapping slot.
   */
  [[nodiscard]] inline std::vector<std::byte> getMappingSlot() const { return mMappingSlot; }

  /**
   * Get the Solidity mapping entry key.
   *
   * @return The Solidity mapping entry key.
   */
  [[nodiscard]] inline std::vector<std::byte> getKey() const { return mKey; }

  /**
   * Get the Solidity mapping entry value.
   *
   * @return The Solidity mapping entry value.
   */
  [[nodiscard]] inline std::vector<std::byte> getValue() const { return mValue; }

private:
  /**
   * The slot corresponding to the Solidity mapping.
   */
  std::vector<std::byte> mMappingSlot;

  /**
   * The 32-byte key of the mapping entry.
   */
  std::vector<std::byte> mKey;

  /**
   * The 32-byte value of the mapping entry (leave empty to delete).
   */
  std::vector<std::byte> mValue;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LAMBDA_MAPPING_ENTRY_H_