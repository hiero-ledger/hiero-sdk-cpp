// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HOOK_ID_H_
#define HIERO_SDK_CPP_HOOK_ID_H_

#include "hooks/HookEntityId.h"

#include <cstdint>
#include <memory>

namespace proto
{
class HookId;
}

namespace Hiero
{
/**
 * An ID for a hook.
 */
class HookId
{
public:
  /**
   * Construct a HookId object from a HookId protobuf object.
   *
   * @param proto The HookId protobuf object from which to create a HookId object.
   * @return The constructed HookId object.
   */
  [[nodiscard]] static HookId fromProtobuf(const proto::HookId& proto);

  /**
   * Construct a HookId protobuf object from this HookId object.
   *
   * @return A pointer to the created HookId protobuf object filled with this HookId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::HookId> toProtobuf() const;

  /**
   * Set the ID of the owning entity.
   *
   * @param entityId The ID of the hook-owning entity.
   * @return A reference to this HookCreationDetails object with the newly-set owning entity ID.
   */
  HookId& setEntityId(const HookEntityId& entityId);

  /**
   * Set the ID of the hook.
   *
   * @param hookId The ID of the hook.
   * @return A reference to this HookCreationDetails object with the newly-set hook ID.
   */
  HookId& setHookId(int64_t hookId);

  /**
   * Get the owning entity ID of the hook.
   *
   * @return The owning entity ID of the hook.
   */
  [[nodiscard]] inline HookEntityId getEntityId() const { return mEntityId; }

  /**
   * Get the hook ID.
   *
   * @return The hook ID.
   */
  [[nodiscard]] inline int64_t getHookId() const { return mHookId; }

private:
  /**
   * The hook's owning entity ID.
   */
  HookEntityId mEntityId;

  /**
   * The ID for the hook.
   */
  int64_t mHookId = 0LL;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_HOOK_ID_H_