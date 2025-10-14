// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaMappingEntries.h"
#include "hooks/LambdaMappingEntry.h"
#include "impl/Utilities.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
LambdaMappingEntries LambdaMappingEntries::fromProtobuf(
  const com::hedera::hapi::node::hooks::LambdaMappingEntries& proto)
{
  LambdaMappingEntries lambdaMappingEntries;
  lambdaMappingEntries.mMappingSlot = internal::Utilities::stringToByteVector(proto.mapping_slot());

  for (int i = 0; i < proto.entries_size(); ++i)
  {
    lambdaMappingEntries.mEntries.push_back(LambdaMappingEntry::fromProtobuf(proto.entries(i)));
  }

  return lambdaMappingEntries;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntries> LambdaMappingEntries::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::LambdaMappingEntries>();
  proto->set_mapping_slot(internal::Utilities::byteVectorToString(mMappingSlot));

  for (const LambdaMappingEntry& entry : mEntries)
  {
    *proto->add_entries() = *entry.toProtobuf();
  }

  return proto;
}

//-----
LambdaMappingEntries& LambdaMappingEntries::setMappingSlot(const std::vector<std::byte>& mappingSlot)
{
  mMappingSlot = mappingSlot;
  return *this;
}

//-----
LambdaMappingEntries& LambdaMappingEntries::addEntry(const LambdaMappingEntry& entry)
{
  mEntries.push_back(entry);
  return *this;
}

//-----
LambdaMappingEntries& LambdaMappingEntries::setEntries(const std::vector<LambdaMappingEntry>& entries)
{
  mEntries = entries;
  return *this;
}

//-----
LambdaMappingEntries& LambdaMappingEntries::clearEntries()
{
  mEntries.clear();
  return *this;
}

} // namespace Hiero
