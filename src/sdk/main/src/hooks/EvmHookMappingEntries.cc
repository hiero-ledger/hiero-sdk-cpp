// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookMappingEntries.h"
#include "hooks/EvmHookMappingEntry.h"
#include "impl/Utilities.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
EvmHookMappingEntries EvmHookMappingEntries::fromProtobuf(
  const com::hedera::hapi::node::hooks::EvmHookMappingEntries& proto)
{
  EvmHookMappingEntries evmHookMappingEntries;
  evmHookMappingEntries.mMappingSlot = internal::Utilities::stringToByteVector(proto.mapping_slot());

  for (int i = 0; i < proto.entries_size(); ++i)
  {
    evmHookMappingEntries.mEntries.push_back(EvmHookMappingEntry::fromProtobuf(proto.entries(i)));
  }

  return evmHookMappingEntries;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookMappingEntries> EvmHookMappingEntries::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::EvmHookMappingEntries>();
  proto->set_mapping_slot(internal::Utilities::byteVectorToString(mMappingSlot));

  for (const EvmHookMappingEntry& entry : mEntries)
  {
    *proto->add_entries() = *entry.toProtobuf();
  }

  return proto;
}

//-----
EvmHookMappingEntries& EvmHookMappingEntries::setMappingSlot(const std::vector<std::byte>& mappingSlot)
{
  mMappingSlot = mappingSlot;
  return *this;
}

//-----
EvmHookMappingEntries& EvmHookMappingEntries::addEntry(const EvmHookMappingEntry& entry)
{
  mEntries.push_back(entry);
  return *this;
}

//-----
EvmHookMappingEntries& EvmHookMappingEntries::setEntries(const std::vector<EvmHookMappingEntry>& entries)
{
  mEntries = entries;
  return *this;
}

//-----
EvmHookMappingEntries& EvmHookMappingEntries::clearEntries()
{
  mEntries.clear();
  return *this;
}

} // namespace Hiero
