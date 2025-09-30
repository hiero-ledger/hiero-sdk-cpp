// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LAMBDA_STORAGE_UPDATE_H_
#define HIERO_SDK_CPP_LAMBDA_STORAGE_UPDATE_H_

#include "hooks/LambdaMappingEntries.h"
#include "hooks/LambdaStorageSlot.h"

#include <optional>

namespace com::hedera::hapi::node::hooks
{
class LambdaStorageUpdate;
}

namespace Hiero
{
/**
 * Specifies a key/value pair in the storage of a lambda, either by the explicit storage slot contents; or by a
 * combination of a Solidity mapping's slot key and the key into that mapping.
 */
class LambdaStorageUpdate
{
public:
  /**
   * Construct a LambdaStorageUpdate object from a LambdaStorageUpdate protobuf object.
   *
   * @param proto The LambdaStorageUpdate protobuf object from which to create a LambdaStorageUpdate object.
   * @return The constructed HookId object.
   */
  [[nodiscard]] static LambdaStorageUpdate fromProtobuf(const com::hedera::hapi::node::hooks::LambdaStorageUpdate& proto);

  /**
   * Construct a LambdaStorageUpdate protobuf object from this HookId object.
   *
   * @return A pointer to the created LambdaStorageUpdate protobuf object filled with this LambdaStorageUpdate
   *         object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::LambdaStorageUpdate> toProtobuf() const;

  /**
   * Set an update for an explicit storage slot. It also resets any value for a slot update using a Solidity mapping
   * entry.
   *
   * @param storageSlot The explicit storage slot.
   * @return A reference to this LambdaStorageUpdate object with the newly-set explicit storage slot update.
   */
  LambdaStorageUpdate& setStorageSlot(const LambdaStorageSlot& storageSlot);

  /**
   * Set an update for Solidity mapped entries. It also resets any value for an explicit storage update.
   *
   * @param mappingEntries The update for Solidity mapped entries.
   * @return A reference to this LambdaStorageUpdate object with the newly-set Solidity mapped storage update.
   */
  LambdaStorageUpdate& setMappingEntries(const LambdaMappingEntries& mappingEntries);

  /**
   * Get the explicit storage slot update.
   *
   * @return The explicit storage slot update.
   */
  [[nodiscard]] inline std::optional<LambdaStorageSlot> getStorageSlot() const { return mStorageSlot; }

  /**
   * Get the Solidity mapped entry update.
   *
   * @return The Solidity mapped entry update.
   */
  [[nodiscard]] inline std::optional<LambdaMappingEntries> getMappingEntries() const { return mMappingEntries; }

private:
  /**
   * An explicit storage slot update.
   */
  std::optional<LambdaStorageSlot> mStorageSlot;

  /**
   * An implicit storage slot update specified as Solidity mapping entries.
   */
  std::optional<LambdaMappingEntries> mMappingEntries;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LAMBDA_STORAGE_UPDATE_H_