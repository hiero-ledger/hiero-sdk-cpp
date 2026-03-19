// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookMappingEntry.h"
#include "impl/Utilities.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
EvmHookMappingEntry EvmHookMappingEntry::fromProtobuf(const com::hedera::hapi::node::hooks::EvmHookMappingEntry& proto)
{
  EvmHookMappingEntry evmHookMappingEntry;

  if (proto.has_key())
  {
    evmHookMappingEntry.mKey = internal::Utilities::stringToByteVector(proto.key());
  }

  if (proto.has_preimage())
  {
    evmHookMappingEntry.mPreimage = internal::Utilities::stringToByteVector(proto.preimage());
  }

  evmHookMappingEntry.mValue = internal::Utilities::stringToByteVector(proto.value());
  return evmHookMappingEntry;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookMappingEntry> EvmHookMappingEntry::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::EvmHookMappingEntry>();

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
EvmHookMappingEntry& EvmHookMappingEntry::setKey(const std::vector<std::byte>& key)
{
  mKey = key;
  mPreimage.reset();
  return *this;
}

//-----
EvmHookMappingEntry& EvmHookMappingEntry::setPreimage(const std::vector<std::byte>& preimage)
{
  mPreimage = preimage;
  mKey.reset();
  return *this;
}

//-----
EvmHookMappingEntry& EvmHookMappingEntry::setValue(const std::vector<std::byte>& value)
{
  mValue = value;
  return *this;
}

} // namespace Hiero
