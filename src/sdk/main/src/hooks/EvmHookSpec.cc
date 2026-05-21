// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookSpec.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
EvmHookSpec EvmHookSpec::fromProtobuf(const com::hedera::hapi::node::hooks::EvmHookSpec& proto)
{
  EvmHookSpec evmHookSpec;

  if (proto.has_contract_id())
  {
    evmHookSpec.mContractId = ContractId::fromProtobuf(proto.contract_id());
  }

  return evmHookSpec;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookSpec> EvmHookSpec::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::EvmHookSpec>();

  if (mContractId.has_value())
  {
    proto->set_allocated_contract_id(mContractId->toProtobuf().release());
  }

  return proto;
}

//-----
EvmHookSpec& EvmHookSpec::setContractId(const ContractId& contractId)
{
  mContractId = contractId;
  return *this;
}

} // namespace Hiero
