// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HOOK_ENTITY_ID_H_
#define HIERO_SDK_CPP_HOOK_ENTITY_ID_H_

#include "AccountId.h"

#include <memory>
#include <optional>

namespace proto
{
class HookEntityId;
}

namespace Hiero
{
/**
 * The entity that owns a hook.
 */
class HookEntityId
{
public:
  /**
   * Construct a HookEntityId object from a HookEntityId protobuf object.
   *
   * @param proto The HookEntityId protobuf object from which to create a HookEntityId object.
   * @return The constructed HookId object.
   */
  [[nodiscard]] static HookEntityId fromProtobuf(const proto::HookEntityId& proto);

  /**
   * Construct a HookEntityId protobuf object from this HookEntityId object.
   *
   * @return A pointer to the created HookEntityId protobuf object filled with this HookEntityId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::HookEntityId> toProtobuf() const;

  /**
   * Set the ID of the owning entity as an account. This will reset other entity ID values.
   *
   * @param accountId The ID of the owning account.
   * @return A reference to this HookCreationDetails object with the newly-set account ID.
   */
  HookEntityId& setAccountId(const AccountId& accountId);

  /**
   * Get the account ID.
   *
   * @return The account ID.
   */
  [[nodiscard]] inline std::optional<AccountId> getAccountId() const { return mAccountId; }

private:
  /**
   * The account owning the hook.
   */
  std::optional<AccountId> mAccountId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_HOOK_ENTITY_ID_H_