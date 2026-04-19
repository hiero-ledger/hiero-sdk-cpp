// SPDX-License-Identifier: Apache-2.0
#include "hooks/FungibleHookType.h"

#include <services/response_code.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<FungibleHookType, std::string> gFungibleHookTypeToString = {
  {FungibleHookType::PRE_TX_ALLOWANCE_HOOK,       "PRE_TX_ALLOWANCE_HOOK"     },
  { FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK, "PRE_POST_TX_ALLOWANCE_HOOK"},
  { FungibleHookType::UNINITIALIZED,              "UNINITIALIZED"             },
};

} // namespace Hiero