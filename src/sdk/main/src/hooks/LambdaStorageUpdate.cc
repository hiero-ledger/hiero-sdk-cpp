// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaStorageUpdate.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
LambdaStorageUpdate LambdaStorageUpdate::fromProtobuf(const com::hedera::hapi::node::hooks::LambdaStorageUpdate& proto)
{
  LambdaStorageUpdate lambdaStorageUpdate;

  if (proto.has_storage_slot())
  {
    lambdaStorageUpdate.mStorageSlot = LambdaStorageSlot::fromProtobuf(proto.storage_slot());
  }

  if (proto.has_mapping_entries())
  {
    lambdaStorageUpdate.mMappingEntries = LambdaMappingEntries::fromProtobuf(proto.mapping_entries());
  }

  return lambdaStorageUpdate;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::LambdaStorageUpdate> LambdaStorageUpdate::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::LambdaStorageUpdate>();

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
LambdaStorageUpdate& LambdaStorageUpdate::setStorageSlot(const LambdaStorageSlot& storageSlot)
{
  mStorageSlot = storageSlot;
  mMappingEntries.reset();
  return *this;
}

//-----
LambdaStorageUpdate& LambdaStorageUpdate::setMappingEntries(const LambdaMappingEntries& mappingEntries)
{
  mMappingEntries = mappingEntries;
  mStorageSlot.reset();
  return *this;
}

} // namespace Hiero
