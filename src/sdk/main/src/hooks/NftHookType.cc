// SPDX-License-Identifier: Apache-2.0
#include "hooks/NftHookType.h"

#include <services/response_code.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<NftHookType, std::string> gNftHookTypeToString = {
  {NftHookType::PRE_HOOK,       "PRE_HOOK"     },
  { NftHookType::PRE_POST_HOOK, "PRE_POST_HOOK"},
  { NftHookType::UNINITIALIZED, "UNINITIALIZED"},
};

} // namespace Hiero
