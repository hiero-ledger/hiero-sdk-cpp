// SPDX-License-Identifier: Apache-2.0
#include "hooks/FungibleHookCall.h"

#include <services/basic_types.pb.h>

namespace Hiero
{
//-----
FungibleHookCall FungibleHookCall::fromProtobuf(const proto::HookCall& proto, FungibleHookType hookType)
{
  FungibleHookCall fungibleHookCall;
  fungibleHookCall.applyFromProtobuf(proto);
  fungibleHookCall.mHookType = hookType;
  return fungibleHookCall;
}

//-----
FungibleHookCall& FungibleHookCall::setHookType(FungibleHookType type)
{
  mHookType = type;
  return *this;
}

} // namespace Hiero
