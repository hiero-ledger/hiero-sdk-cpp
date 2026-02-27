// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_HOOK_CALL_H_
#define HIERO_SDK_CPP_HOOK_CALL_H_

#include "hooks/EvmHookCall.h"

#include <cstdint>
#include <optional>

namespace proto
{
class HookCall;
}

namespace Hiero
{
/**
 * Specifies a call to a hook from within a transaction.
 *
 * Often the hook's entity is implied by the nature of the call site. For example, when using an account allowance hook
 * inside a crypto transfer, the hook's entity is necessarily the account whose authorization is required.
 *
 * For future extension points where the hook owner is not forced by the context, we include the option to fully
 * specify the hook id for the call.
 */
class HookCall
{
public:
  /**
   * Construct a HookCall protobuf object from this HookCall object.
   *
   * @return A pointer to the created HookCall protobuf object filled with this HookCall object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::HookCall> toProtobuf() const;

  /**
   * Set the ID of the hook to call. Resets the full hook ID if already set.
   *
   * @param hookId The ID of the hook to call.
   * @return A reference to this EvmHookCall object with the newly-added hook ID.
   */
  HookCall& setHookId(int64_t hookId);

  /**
   * Set the EVM hook specification. Resets any other set hook specification.
   *
   * @param evmHookCall The specification of the EVM hook call.
   * @return A reference to this EvmHookCall object with the newly-set EVM hook specification.
   */
  HookCall& setEvmHookCall(const EvmHookCall& evmHookCall);

  /**
   * Get the ID of the hook.
   *
   * @return The ID of the hook.
   */
  [[nodiscard]] inline int64_t getHookId() const { return mHookId; }

  /**
   * Get the hook EVM call specification.
   *
   * @return The hook EVM call specification.
   */
  [[nodiscard]] inline std::optional<EvmHookCall> getEvmHookCall() const { return mEvmHookCall; }

protected:
  /**
   * Grab the values from a HookCall protobuf object and apply them to this HookCall object.
   *
   * @param proto The HookCall protobuf object from which to create a HookCall object.
   * @return The constructed HookCall object.
   */
  void applyFromProtobuf(const proto::HookCall& proto);

private:
  /**
   * The ID of the hook to call.
   */
  int64_t mHookId = 0LL;

  /**
   * The specification of how to call an EVM hook.
   */
  std::optional<EvmHookCall> mEvmHookCall;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_HOOK_CALL_H_