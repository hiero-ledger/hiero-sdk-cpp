// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookEntityId.h"

#include <services/basic_types.pb.h>

namespace Hiero
{
//-----
HookEntityId HookEntityId::fromProtobuf(const proto::HookEntityId& proto)
{
  HookEntityId hookEntityId;

  if (proto.has_account_id())
  {
    hookEntityId.mAccountId = AccountId::fromProtobuf(proto.account_id());
  }

  return hookEntityId;
}

//-----
std::unique_ptr<proto::HookEntityId> HookEntityId::toProtobuf() const
{
  auto proto = std::make_unique<proto::HookEntityId>();

  if (mAccountId.has_value())
  {
    proto->set_allocated_account_id(mAccountId->toProtobuf().release());
  }

  return proto;
}

//-----
HookEntityId& HookEntityId::setAccountId(const AccountId& accountId)
{
  mAccountId = accountId;
  return *this;
}

} // namespace Hiero
