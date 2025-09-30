// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EVM_HOOK_CALL_H_
#define HIERO_SDK_CPP_EVM_HOOK_CALL_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace proto
{
class EvmHookCall;
}

namespace Hiero
{
/**
 * Specifies the details of a call to an EVM hook.
 */
class EvmHookCall
{
public:
  /**
   * Construct a EvmHookCall object from a EvmHookCall protobuf object.
   *
   * @param proto The EvmHookCall protobuf object from which to create a EvmHookCall object.
   * @return The constructed EvmHookCall object.
   */
  [[nodiscard]] static EvmHookCall fromProtobuf(const proto::EvmHookCall& proto);

  /**
   * Construct a EvmHookCall protobuf object from this EvmHookCall object.
   *
   * @return A pointer to the created EvmHookCall protobuf object filled with this EvmHookCall object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::EvmHookCall> toProtobuf() const;

  /**
   * Set the call data to pass to the hook.
   *
   * @param callData The call data to pass to the hook.
   * @return A reference to this EvmHookCall object with the newly-added call data.
   */
  EvmHookCall& setData(const std::vector<std::byte>& callData);

  /**
   * Set the gas limit for the hook.
   *
   * @param gasLimit The gas limit for the hook.
   * @return A reference to this EvmHookCall object with the newly-added gas limit.
   */
  EvmHookCall& setGasLimit(uint64_t gasLimit);

  /**
   * Get the call data.
   *
   * @return The call data.
   */
  [[nodiscard]] inline std::vector<std::byte> getData() const { return mData; }

  /**
   * Get the gas limit.
   *
   * @return The gas limit.
   */
  [[nodiscard]] inline uint64_t getGasLimit() const { return mGasLimit; }

private:
  /**
   * The call data to pass to the hook.
   */
  std::vector<std::byte> mData;

  /**
   * The gas limit to use.
   */
  uint64_t mGasLimit;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EVM_HOOK_CALL_H_