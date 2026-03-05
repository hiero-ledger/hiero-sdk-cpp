// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookStorageUpdate.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
EvmHookStorageUpdate EvmHookStorageUpdate::fromProtobuf(const com::hedera::hapi::node::hooks::EvmHookStorageUpdate& proto)
{
  EvmHookStorageUpdate evmHookStorageUpdate;

  if (proto.has_storage_slot())
  {
    evmHookStorageUpdate.mStorageSlot = EvmHookStorageSlot::fromProtobuf(proto.storage_slot());
  }

  if (proto.has_mapping_entries())
  {
    evmHookStorageUpdate.mMappingEntries = EvmHookMappingEntries::fromProtobuf(proto.mapping_entries());
  }

  return evmHookStorageUpdate;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookStorageUpdate> EvmHookStorageUpdate::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::EvmHookStorageUpdate>();

  if (mStorageSlot.has_value())
  {
    proto->set_allocated_storage_slot(mStorageSlot->toProtobuf().release());
  }

  if (mMappingEntries.has_value())
  {
    proto->set_allocated_mapping_entries(mMappingEntries->toProtobuf().release());
  }

  return proto;
}

//-----
EvmHookStorageUpdate& EvmHookStorageUpdate::setStorageSlot(const EvmHookStorageSlot& storageSlot)
{
  mStorageSlot = storageSlot;
  mMappingEntries.reset();
  return *this;
}

//-----
EvmHookStorageUpdate& EvmHookStorageUpdate::setMappingEntries(const EvmHookMappingEntries& mappingEntries)
{
  mMappingEntries = mappingEntries;
  mStorageSlot.reset();
  return *this;
}

} // namespace Hiero
