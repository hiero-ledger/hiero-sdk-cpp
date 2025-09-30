// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaMappingEntry.h"
#include "impl/Utilities.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
LambdaMappingEntry LambdaMappingEntry::fromProtobuf(const com::hedera::hapi::node::hooks::LambdaMappingEntry& proto)
{
  LambdaMappingEntry lambdaMappingEntry;
  lambdaMappingEntry.mMappingSlot = proto.
  lambdaMappingEntry.mKey = internal::Utilities::stringToByteVector(proto.key());
  lambdaMappingEntry.mValue = internal::Utilities::stringToByteVector(proto.value());
  return lambdaMappingEntry;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntry> LambdaMappingEntry::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::LambdaMappingEntry>();
  proto->set_key(internal::Utilities::byteVectorToString(mKey));
  proto->set_value(internal::Utilities::byteVectorToString(mValue));
  return proto;
}

//-----
LambdaMappingEntry& LambdaMappingEntry::setKey(const std::vector<std::byte>& key)
{
  mKey = key;
  return *this;
}

//-----
LambdaMappingEntry& LambdaMappingEntry::setValue(const std::vector<std::byte>& value)
{
  mValue = value;
  return *this;
}

} // namespace Hiero
