// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaEvmHook.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
LambdaEvmHook LambdaEvmHook::fromProtobuf(const com::hedera::hapi::node::hooks::LambdaEvmHook& proto)
{
  LambdaEvmHook lambdaEvmHook;

  for (int i = 0; i < proto.storage_updates_size(); ++i)
  {
    lambdaEvmHook.mStorageUpdates.push_back(LambdaStorageUpdate::fromProtobuf(proto.storage_updates(i)));
  }

  return lambdaEvmHook;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::LambdaEvmHook> LambdaEvmHook::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::LambdaEvmHook>();

  for (const LambdaStorageUpdate& update : mStorageUpdates)
  {
    *proto->add_storage_updates() = *update.toProtobuf();
  }

  return proto;
}

//-----
LambdaEvmHook& LambdaEvmHook::addStorageUpdate(const LambdaStorageUpdate& storageUpdate)
{
  mStorageUpdates.push_back(storageUpdate);
  return *this;
}

//-----
LambdaEvmHook& LambdaEvmHook::setStorageUpdates(const std::vector<LambdaStorageUpdate>& storageUpdates)
{
  mStorageUpdates = storageUpdates;
  return *this;
}

//-----
LambdaEvmHook& LambdaEvmHook::clearStorageUpdates()
{
  mStorageUpdates.clear();
  return *this;
}

} // namespace Hiero
