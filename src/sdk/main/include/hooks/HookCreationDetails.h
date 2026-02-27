// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HOOK_CREATION_DETAILS_H_
#define HIERO_SDK_CPP_HOOK_CREATION_DETAILS_H_

#include "Key.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/EvmHook.h"

#include <cstdint>
#include <memory>
#include <optional>

namespace com::hedera::hapi::node::hooks
{
class HookCreationDetails;
}

namespace Hiero
{
/**
 * Struct to hold the creation details for a hook.
 */
class HookCreationDetails
{
public:
  /**
   * Construct a HookCreationDetails object from a HookCreationDetails protobuf object.
   *
   * @param proto The HookCreationDetails protobuf object from which to create a HookCreationDetails object.
   * @return The constructed HookCreationDetails object.
   */
  [[nodiscard]] static HookCreationDetails fromProtobuf(
    const com::hedera::hapi::node::hooks::HookCreationDetails& proto);

  /**
   * Construct a HookCreationDetails protobuf object from this HookCreationDetails object.
   *
   * @return A pointer to the created HookCreationDetails protobuf object filled with this HookCreationDetails
   *         object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::HookCreationDetails> toProtobuf() const;

  /**
   * Set the extension point for the hook.
   *
   * @param extensionPoint The extension point of the hook.
   * @return A reference to this HookCreationDetails object with the newly-set extension point.
   */
  HookCreationDetails& setExtensionPoint(HookExtensionPoint extensionPoint);

  /**
   * Set the ID of the hook.
   *
   * @param hookId The ID of the hook.
   * @return A reference to this HookCreationDetails object with the newly-set hook ID.
   */
  HookCreationDetails& setHookId(int64_t hookId);

  /**
   * Set the EVM hook implementation. Resets any other hook implementation.
   *
   * @param evmHook The hook implementation.
   * @return A reference to this HookCreationDetails object with the newly-set EVM hook implementation.
   */
  HookCreationDetails& setEvmHook(const EvmHook& evmHook);

  /**
   * Set the hook admin key.
   *
   * @param adminKey The hook admin key.
   * @return A reference to this HookCreationDetails object with the newly-set admin key.
   */
  HookCreationDetails& setAdminKey(const std::shared_ptr<Key>& adminKey);

  /**
   * Get the extension point of the hook.
   *
   * @return The extension point of the hook.
   */
  [[nodiscard]] inline HookExtensionPoint getExtensionPoint() const { return mExtensionPoint; }

  /**
   * Get the hook implementation.
   *
   * @return The hook implementation.
   */
  [[nodiscard]] inline int64_t getHookId() const { return mHookId; }

  /**
   * Get the ID of the hook.
   *
   * @return The ID of the hook.
   */
  [[nodiscard]] inline std::optional<EvmHook> getEvmHook() const { return mEvmHook; }

  /**
   * Get the hook admin key.
   *
   * @return The hook admin key.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getAdminKey() const { return mAdminKey; }

private:
  /**
   * The extension point for the hook.
   */
  HookExtensionPoint mExtensionPoint;

  /**
   * The ID at which to create the hook.
   */
  int64_t mHookId;

  /**
   * The EVM hook implementation.
   *
   * A hook programmed in EVM bytecode that may access state or interact with external contracts.
   */
  std::optional<EvmHook> mEvmHook;

  /**
   * A key that that can be used to remove or replace the hook.
   */
  std::shared_ptr<Key> mAdminKey;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_HOOK_CREATION_DETAILS_H_