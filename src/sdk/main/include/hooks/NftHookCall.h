// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NFT_HOOK_CALL_H_
#define HIERO_SDK_CPP_NFT_HOOK_CALL_H_

#include "hooks/HookCall.h"
#include "hooks/NftHookType.h"

namespace Hiero
{
/**
 * Specifies a call to a hook from within a transaction that interacts with NFTs.
 */
class NftHookCall : public HookCall
{
public:
  /**
   * Construct a NftHookCall object from a HookCall protobuf object and a hook type.
   *
   * @param proto    The HookCall protobuf object from which to create a NftHookCall object.
   * @param hookType The type of hook.
   * @return The constructed NftHookCall object.
   */
  [[nodiscard]] static NftHookCall fromProtobuf(const proto::HookCall& proto, NftHookType hookType);

  /**
   * Set the type of the fungible hook to call.
   *
   * @param type The type of the fungible hook to call.
   * @return A reference to this NftHookCall object with the newly-set fungible hook type.
   */
  NftHookCall& setHookType(NftHookType type);

  /**
   * Get the type of the fungible hook to call.
   *
   * @return The type of the fungible hook to call.
   */
  [[nodiscard]] inline NftHookType getHookType() const { return mHookType; }

private:
  /**
   * The type of the fungible hook to call.
   */
  NftHookType mHookType = NftHookType::UNINITIALIZED;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_NFT_HOOK_CALL_H_