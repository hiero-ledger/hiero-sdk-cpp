// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LAMBDA_STORAGE_SLOT_H_
#define HIERO_SDK_CPP_LAMBDA_STORAGE_SLOT_H_

#include <cstddef>
#include <vector>

namespace com::hedera::hapi::node::hooks
{
class LambdaStorageSlot;
}

namespace Hiero
{
/**
 * An explicit storage slot update.
 */
class LambdaStorageSlot
{
public:
  /**
   * Construct a LambdaStorageSlot object from a LambdaStorageSlot protobuf object.
   *
   * @param proto The LambdaStorageSlot protobuf object from which to create a LambdaStorageSlot object.
   * @return The constructed LambdaStorageSlot object.
   */
  [[nodiscard]] static LambdaStorageSlot fromProtobuf(const com::hedera::hapi::node::hooks::LambdaStorageSlot& proto);

  /**
   * Construct a LambdaStorageSlot protobuf object from this LambdaStorageSlot object.
   *
   * @return A pointer to the created LambdaStorageSlot protobuf object filled with this LambdaStorageSlot object's
   *         data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::LambdaStorageSlot> toProtobuf() const;

  /**
   * Set the storage slot key.
   *
   * @param key The storage slot key.
   * @return A reference to this LambdaStorageSlot object with the newly-set mapping entry key.
   */
  LambdaStorageSlot& setKey(const std::vector<std::byte>& key);

  /**
   * Set the value for the mapping entry.
   *
   * @param value The value for the mapping entry.
   * @return A reference to this LambdaStorageSlot object with the newly-set mapping entry value.
   */
  LambdaStorageSlot& setValue(const std::vector<std::byte>& value);

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

#endif // HIERO_SDK_CPP_LAMBDA_STORAGE_SLOT_H_