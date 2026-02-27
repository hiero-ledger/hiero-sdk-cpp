// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookExtensionPoint.h"

#include <services/hook_types.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<com::hedera::hapi::node::hooks::HookExtensionPoint, HookExtensionPoint>
  gProtobufHookExtensionPointToHookExtensionPoint = {
    {com::hedera::hapi::node::hooks::HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK,
     HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK}
};

//-----
const std::unordered_map<HookExtensionPoint, com::hedera::hapi::node::hooks::HookExtensionPoint>
  gHookExtensionPointToProtobufHookExtensionPoint = {
    {
     HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK,
     com::hedera::hapi::node::hooks::HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK,
     }
};

} // namespace Hiero
