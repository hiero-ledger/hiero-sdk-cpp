// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHook.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
EvmHook EvmHook::fromProtobuf(const com::hedera::hapi::node::hooks::EvmHook& proto)
{
  EvmHook evmHook;

  if (proto.has_spec())
  {
    evmHook.mEvmHookSpec = EvmHookSpec::fromProtobuf(proto.spec());
  }

  for (int i = 0; i < proto.storage_updates_size(); ++i)
  {
    evmHook.mStorageUpdates.push_back(EvmHookStorageUpdate::fromProtobuf(proto.storage_updates(i)));
  }

  return evmHook;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::EvmHook> EvmHook::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::EvmHook>();

  proto->set_allocated_spec(mEvmHookSpec.toProtobuf().release());

  for (const EvmHookStorageUpdate& update : mStorageUpdates)
  {
    *proto->add_storage_updates() = *update.toProtobuf();
  }

  return proto;
}

//-----
EvmHook& EvmHook::setEvmHookSpec(const EvmHookSpec& spec)
{
  mEvmHookSpec = spec;
  return *this;
}

//-----
EvmHook& EvmHook::addStorageUpdate(const EvmHookStorageUpdate& storageUpdate)
{
  mStorageUpdates.push_back(storageUpdate);
  return *this;
}

//-----
EvmHook& EvmHook::setStorageUpdates(const std::vector<EvmHookStorageUpdate>& storageUpdates)
{
  mStorageUpdates = storageUpdates;
  return *this;
}

//-----
EvmHook& EvmHook::clearStorageUpdates()
{
  mStorageUpdates.clear();
  return *this;
}

} // namespace Hiero
