// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookStorageSlot.h"
#include "impl/Utilities.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
EvmHookStorageSlot EvmHookStorageSlot::fromProtobuf(const com::hedera::hapi::node::hooks::EvmHookStorageSlot& proto)
{
  EvmHookStorageSlot evmHookStorageSlot;
  evmHookStorageSlot.mKey = internal::Utilities::stringToByteVector(proto.key());
  evmHookStorageSlot.mValue = internal::Utilities::stringToByteVector(proto.value());
  return evmHookStorageSlot;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookStorageSlot> EvmHookStorageSlot::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::EvmHookStorageSlot>();
  proto->set_key(internal::Utilities::byteVectorToString(mKey));
  proto->set_value(internal::Utilities::byteVectorToString(mValue));
  return proto;
}

//-----
EvmHookStorageSlot& EvmHookStorageSlot::setKey(const std::vector<std::byte>& key)
{
  mKey = key;
  return *this;
}

//-----
EvmHookStorageSlot& EvmHookStorageSlot::setValue(const std::vector<std::byte>& value)
{
  mValue = value;
  return *this;
}

} // namespace Hiero
