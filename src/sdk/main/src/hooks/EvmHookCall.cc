// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookCall.h"
#include "impl/Utilities.h"

#include <services/basic_types.pb.h>

namespace Hiero
{
//-----
EvmHookCall EvmHookCall::fromProtobuf(const proto::EvmHookCall& proto)
{
  EvmHookCall evmHookCall;
  evmHookCall.mData = internal::Utilities::stringToByteVector(proto.data());
  evmHookCall.mGasLimit = proto.gas_limit();
  return evmHookCall;
}

//-----
std::unique_ptr<proto::EvmHookCall> EvmHookCall::toProtobuf() const
{
  auto proto = std::make_unique<proto::EvmHookCall>();
  proto->set_data(internal::Utilities::byteVectorToString(mData));
  proto->set_gas_limit(mGasLimit);
  return proto;
}

//-----
EvmHookCall& EvmHookCall::setData(const std::vector<std::byte>& callData)
{
  mData = callData;
  return *this;
}

//-----
EvmHookCall& EvmHookCall::setGasLimit(uint64_t gasLimit)
{
  mGasLimit = gasLimit;
  return *this;
}

} // namespace Hiero
