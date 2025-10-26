// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookCall.h"

#include <services/basic_types.pb.h>

namespace Hiero
{
//-----
std::unique_ptr<proto::HookCall> HookCall::toProtobuf() const
{
  auto proto = std::make_unique<proto::HookCall>();

  if (mHookId != 0LL)
  {
    proto->set_hook_id(mHookId);
  }

  if (mEvmHookCall.has_value())
  {
    proto->set_allocated_evm_hook_call(mEvmHookCall->toProtobuf().release());
  }

  return proto;
}

//-----
HookCall& HookCall::setHookId(int64_t hookId)
{
  mHookId = hookId;
  return *this;
}

//-----
HookCall& HookCall::setEvmHookCall(const EvmHookCall& evmHookCall)
{
  mEvmHookCall = evmHookCall;
  return *this;
}

//-----
void HookCall::applyFromProtobuf(const proto::HookCall& proto)
{
  if (proto.has_hook_id())
  {
    mHookId = proto.hook_id();
  }

  if (proto.has_evm_hook_call())
  {
    mEvmHookCall = EvmHookCall::fromProtobuf(proto.evm_hook_call());
  }
}

} // namespace Hiero
