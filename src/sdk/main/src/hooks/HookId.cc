// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookId.h"

#include <services/basic_types.pb.h>

namespace Hiero
{
//-----
HookId HookId::fromProtobuf(const proto::HookId& proto)
{
  HookId hookId;
  hookId.mHookId = proto.hook_id();
  hookId.mEntityId = HookEntityId::fromProtobuf(proto.entity_id());
  return hookId;
}

//-----
std::unique_ptr<proto::HookId> HookId::toProtobuf() const
{
  auto proto = std::make_unique<proto::HookId>();
  proto->set_allocated_entity_id(mEntityId.toProtobuf().release());
  proto->set_hook_id(mHookId);
  return proto;
}

//-----
HookId& HookId::setEntityId(const HookEntityId& entityId)
{
  mEntityId = entityId;
  return *this;
}

//-----
HookId& HookId::setHookId(int64_t hookId)
{
  mHookId = hookId;
  return *this;
}

} // namespace Hiero
