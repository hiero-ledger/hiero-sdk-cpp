// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookCreationDetails.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
HookCreationDetails HookCreationDetails::fromProtobuf(const com::hedera::hapi::node::hooks::HookCreationDetails& proto)
{
  HookCreationDetails hookCreationDetails;
  hookCreationDetails.mExtensionPoint = gProtobufHookExtensionPointToHookExtensionPoint.at(proto.extension_point());
  hookCreationDetails.mHookId = proto.hook_id();

  if (proto.has_evm_hook())
  {
    hookCreationDetails.mEvmHook = EvmHook::fromProtobuf(proto.evm_hook());
  }

  if (proto.has_admin_key())
  {
    hookCreationDetails.mAdminKey = Key::fromProtobuf(proto.admin_key());
  }

  return hookCreationDetails;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::HookCreationDetails> HookCreationDetails::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::HookCreationDetails>();
  proto->set_extension_point(gHookExtensionPointToProtobufHookExtensionPoint.at(mExtensionPoint));
  proto->set_hook_id(mHookId);

  if (mEvmHook.has_value())
  {
    proto->set_allocated_evm_hook(mEvmHook->toProtobuf().release());
  }

  if (mAdminKey)
  {
    proto->set_allocated_admin_key(mAdminKey->toProtobufKey().release());
  }

  return proto;
}

//-----
HookCreationDetails& HookCreationDetails::setExtensionPoint(HookExtensionPoint extensionPoint)
{
  mExtensionPoint = extensionPoint;
  return *this;
}

//-----
HookCreationDetails& HookCreationDetails::setHookId(int64_t hookId)
{
  mHookId = hookId;
  return *this;
}

//-----
HookCreationDetails& HookCreationDetails::setEvmHook(const EvmHook& evmHook)
{
  mEvmHook = evmHook;
  return *this;
}

//-----
HookCreationDetails& HookCreationDetails::setAdminKey(const std::shared_ptr<Key>& adminKey)
{
  mAdminKey = adminKey;
  return *this;
}

} // namespace Hiero
