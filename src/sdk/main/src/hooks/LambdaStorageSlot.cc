// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaStorageSlot.h"
#include "impl/Utilities.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
LambdaStorageSlot LambdaStorageSlot::fromProtobuf(const com::hedera::hapi::node::hooks::LambdaStorageSlot& proto)
{
  LambdaStorageSlot lambdaStorageSlot;
  lambdaStorageSlot.mKey = internal::Utilities::stringToByteVector(proto.key());
  lambdaStorageSlot.mValue = internal::Utilities::stringToByteVector(proto.value());
  return lambdaStorageSlot;
}

//-----
std::unique_ptr<com::hedera::hapi::node::hooks::LambdaStorageSlot> LambdaStorageSlot::toProtobuf() const
{
  auto proto = std::make_unique<com::hedera::hapi::node::hooks::LambdaStorageSlot>();
  proto->set_key(internal::Utilities::byteVectorToString(mKey));
  proto->set_value(internal::Utilities::byteVectorToString(mValue));
  return proto;
}

//-----
LambdaStorageSlot& LambdaStorageSlot::setKey(const std::vector<std::byte>& key)
{
  mKey = key;
  return *this;
}

//-----
LambdaStorageSlot& LambdaStorageSlot::setValue(const std::vector<std::byte>& value)
{
  mValue = value;
  return *this;
}

} // namespace Hiero
