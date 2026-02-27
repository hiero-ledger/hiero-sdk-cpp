// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EVM_HOOK_STORAGE_SLOT_H_
#define HIERO_SDK_CPP_EVM_HOOK_STORAGE_SLOT_H_

#include <cstddef>
#include <memory>
#include <vector>

namespace com::hedera::hapi::node::hooks
{
class EvmHookStorageSlot;
}

namespace Hiero
{
/**
 * An explicit storage slot update.
 */
class EvmHookStorageSlot
{
public:
  /**
   * Construct a EvmHookStorageSlot object from a EvmHookStorageSlot protobuf object.
   *
   * @param proto The EvmHookStorageSlot protobuf object from which to create a EvmHookStorageSlot object.
   * @return The constructed EvmHookStorageSlot object.
   */
  [[nodiscard]] static EvmHookStorageSlot fromProtobuf(const com::hedera::hapi::node::hooks::EvmHookStorageSlot& proto);

  /**
   * Construct a EvmHookStorageSlot protobuf object from this EvmHookStorageSlot object.
   *
   * @return A pointer to the created EvmHookStorageSlot protobuf object filled with this EvmHookStorageSlot object's
   *         data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookStorageSlot> toProtobuf() const;

  /**
   * Set the storage slot key.
   *
   * @param key The storage slot key.
   * @return A reference to this EvmHookStorageSlot object with the newly-set mapping entry key.
   */
  EvmHookStorageSlot& setKey(const std::vector<std::byte>& key);

  /**
   * Set the value for the mapping entry.
   *
   * @param value The value for the mapping entry.
   * @return A reference to this EvmHookStorageSlot object with the newly-set mapping entry value.
   */
  EvmHookStorageSlot& setValue(const std::vector<std::byte>& value);

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
   * The 32-byte storage slot key.
   */
  std::vector<std::byte> mKey;

  /**
   * The 32-byte storage slot value (leave empty to delete).
   */
  std::vector<std::byte> mValue;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EVM_HOOK_STORAGE_SLOT_H_