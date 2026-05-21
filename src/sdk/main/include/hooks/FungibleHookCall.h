// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FUNGIBLE_HOOK_CALL_H_
#define HIERO_SDK_CPP_FUNGIBLE_HOOK_CALL_H_

#include "hooks/FungibleHookType.h"
#include "hooks/HookCall.h"

namespace proto
{
class HookCall;
}

namespace Hiero
{
/**
 * Specifies a call to a hook from within a transaction that interacts with fungible tokens (HBAR included).
 */
class FungibleHookCall : public HookCall
{
public:
  /**
   * Construct a FungibleHookCall object from a HookCall protobuf object and a hook type.
   *
   * @param proto    The HookCall protobuf object from which to create a FungibleHookCall object.
   * @param hookType The type of hook.
   * @return The constructed FungibleHookCall object.
   */
  [[nodiscard]] static FungibleHookCall fromProtobuf(const proto::HookCall& proto, FungibleHookType hookType);

  /**
   * Set the type of the fungible hook to call.
   *
   * @param type The type of the fungible hook to call.
   * @return A reference to this FungibleHookCall object with the newly-set fungible hook type.
   */
  FungibleHookCall& setHookType(FungibleHookType type);

  /**
   * Get the type of the fungible hook to call.
   *
   * @return The type of the fungible hook to call.
   */
  [[nodiscard]] inline FungibleHookType getHookType() const { return mHookType; }

private:
  /**
   * The type of the fungible hook to call.
   */
  FungibleHookType mHookType = FungibleHookType::UNINITIALIZED;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FUNGIBLE_HOOK_CALL_H_