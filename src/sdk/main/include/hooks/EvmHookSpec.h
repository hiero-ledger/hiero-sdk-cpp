// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EVM_HOOK_SPEC_H_
#define HIERO_SDK_CPP_EVM_HOOK_SPEC_H_

#include "ContractId.h"

#include <memory>
#include <optional>

namespace com::hedera::hapi::node::hooks
{
class EvmHookSpec;
}

namespace Hiero
{
/**
 * Shared specifications of an EVM hook. May be used for any extension point.
 */
class EvmHookSpec
{
public:
  /**
   * Construct a EvmHookSpec object from a EvmHookSpec protobuf object.
   *
   * @param proto The EvmHookSpec protobuf object from which to create a EvmHookSpec object.
   * @return The constructed EvmHookSpec object.
   */
  [[nodiscard]] static EvmHookSpec fromProtobuf(const com::hedera::hapi::node::hooks::EvmHookSpec& proto);

  /**
   * Construct a EvmHookSpec protobuf object from this EvmHookSpec object.
   *
   * @return A pointer to the created EvmHookSpec protobuf object filled with this EvmHookSpec object's data.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookSpec> toProtobuf() const;

  /**
   * Set the contract that contains the hook EVM bytecode. Resets other bytecode sources.
   *
   * @param contractId The ID of the contract housing the hook EVM bytecode.
   * @return A reference to this EvmHookSpec object with the newly-added contract ID.
   */
  EvmHookSpec& setContractId(const ContractId& contractId);

  /**
   * Get the ID of the hook EVM bytecode contract.
   *
   * @return The ID of the hook EVM bytecode contract.
   */
  [[nodiscard]] inline std::optional<ContractId> getContractId() const { return mContractId; }

private:
  /**
   * The source of the EVM bytecode for the hook.
   */
  std::optional<ContractId> mContractId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EVM_HOOK_SPEC_H_