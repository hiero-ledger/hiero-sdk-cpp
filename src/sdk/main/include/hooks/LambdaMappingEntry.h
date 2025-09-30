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
   * Set the key for the mapping entry. Resets the preimage if set.
   *
   * @param key The key for the mapping entry.
   * @return A reference to this LambdaMappingEntry object with the newly-set mapping entry key.
   */
  LambdaMappingEntry& setKey(const std::vector<std::byte>& key);

  /**
   * Set the preimage. Resets the key if set.
   *
   * @param preimage The preimage..
   * @return A reference to this LambdaMappingEntry object with the newly-set preimage.
   */
  LambdaMappingEntry& setPreimage(const std::vector<std::byte>& preimage);

  /**
   * Set the value for the mapping entry.
   *
   * @param value The value for the mapping entry.
   * @return A reference to this LambdaMappingEntry object with the newly-set mapping entry value.
   */
  LambdaMappingEntry& setValue(const std::vector<std::byte>& value);

  /**
   * Get the Solidity mapping entry key.
   *
   * @return The Solidity mapping entry key.
   */
  [[nodiscard]] inline std::optional<std::vector<std::byte>> getKey() const { return mKey; }

  /**
   * Get the preimage.
   *
   * @return The preimage.
   */
  [[nodiscard]] inline std::optional<std::vector<std::byte>> getPreimage() const { return mPreimage; }

  /**
   * Get the Solidity mapping entry value.
   *
   * @return The Solidity mapping entry value.
   */
  [[nodiscard]] inline std::vector<std::byte> getValue() const { return mValue; }

private:
  /**
   * The explicit bytes of the mapping entry. Must use a minimal byte representation; may not exceed 32 bytes in length.
   */
  std::optional<std::vector<std::byte>> mKey;

  /**
   * The bytes that are the preimage of the Keccak256 hash that forms the mapping key. May be longer or shorter than 32
   * bytes and may have leading zeros, since Solidity supports variable-length keys in mappings.
   */
  std::optional<std::vector<std::byte>> mPreimage;

  /**
   * The 32-byte value of the mapping entry (leave empty to delete).
   */
  std::vector<std::byte> mValue;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LAMBDA_MAPPING_ENTRY_H_