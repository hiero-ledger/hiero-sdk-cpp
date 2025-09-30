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

  if (proto.has_key())
  {
    lambdaMappingEntry.mKey = internal::Utilities::stringToByteVector(proto.key());
  }

  if (proto.has_preimage())
  {
    lambdaMappingEntry.mPreimage = internal::Utilities::stringToByteVector(proto.preimage());
  }

  lambdaMappingEntry.mValue = internal::Utilities::stringToByteVector(proto.value());
  return lambdaMappingEntry;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntry> LambdaMappingEntry::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::LambdaMappingEntry>();

  if (mKey.has_value())
  {
    proto->set_key(internal::Utilities::byteVectorToString(mKey.value()));
  }

  if (mPreimage.has_value())
  {
    proto->set_preimage(internal::Utilities::byteVectorToString(mPreimage.value()));
  }

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
LambdaMappingEntry& LambdaMappingEntry::setPreimage(const std::vector<std::byte>& preimage)
{
  mPreimage = preimage;
  return *this;
}

//-----
LambdaMappingEntry& LambdaMappingEntry::setValue(const std::vector<std::byte>& value)
{
  mValue = value;
  return *this;
}

} // namespace Hiero
