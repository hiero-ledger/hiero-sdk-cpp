// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookCall.h"

#include <services/basic_types.pb.h>

namespace Hiero
{
//-----
HookCall HookCall::fromProtobuf(const proto::HookCall& proto)
{
  HookCall hookCall;

  if (proto.has_full_hook_id())
  {
    hookCall.mFullHookId = HookId::fromProtobuf(proto.full_hook_id());
  }

  if (proto.has_hook_id())
  {
    hookCall.mHookId = proto.hook_id();
  }

  if (proto.has_evm_hook_call())
  {
    hookCall.mEvmHookCall = EvmHookCall::fromProtobuf(proto.evm_hook_call());
  }

  return hookCall;
}

//-----
std::unique_ptr<proto::HookCall> HookCall::toProtobuf() const
{
  auto proto = std::make_unique<proto::HookCall>();

  if (mFullHookId.has_value())
  {
    proto->set_allocated_full_hook_id(mFullHookId->toProtobuf().release());
  }

  if (mHookId.has_value())
  {
    proto->set_hook_id(mHookId.value());
  }

  if (mEvmHookCall.has_value())
  {
    proto->set_allocated_evm_hook_call(mEvmHookCall->toProtobuf().release());
  }

  return proto;
}

//-----
HookCall& HookCall::setFullHookId(const HookId& hookId)
{
  mFullHookId = hookId;
  mHookId.reset();
  return *this;
}

//-----
HookCall& HookCall::setHookId(int64_t hookId)
{
  mHookId = hookId;
  mFullHookId.reset();
  return *this;
}

//-----
HookCall& HookCall::setEvmHookCall(const EvmHookCall& evmHookCall)
{
  mEvmHookCall = evmHookCall;
  return *this;
}

} // namespace Hiero
