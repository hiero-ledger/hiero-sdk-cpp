// SPDX-License-Identifier: Apache-2.0
#include "hooks/NftHookCall.h"

#include <services/basic_types.pb.h>

namespace Hiero
{
//-----
NftHookCall NftHookCall::fromProtobuf(const proto::HookCall& proto, NftHookType hookType)
{
  NftHookCall nftHookCall;
  nftHookCall.applyFromProtobuf(proto);
  nftHookCall.mHookType = hookType;
  return nftHookCall;
}

//-----
NftHookCall& NftHookCall::setHookType(NftHookType type)
{
  mHookType = type;
  return *this;
}

} // namespace Hiero
