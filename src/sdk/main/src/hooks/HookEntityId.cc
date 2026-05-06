// SPDX-License-Identifier: Apache-2.0
#include "hooks/HookEntityId.h"
#include "Client.h"

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
  else if (proto.has_contract_id())
  {
    hookEntityId.mContractId = ContractId::fromProtobuf(proto.contract_id());
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
  else if (mContractId.has_value())
  {
    proto->set_allocated_contract_id(mContractId->toProtobuf().release());
  }

  return proto;
}

//-----
void HookEntityId::validateChecksums(const Client& client) const
{
  if (mAccountId.has_value())
  {
    mAccountId->validateChecksum(client);
  }
  else if (mContractId.has_value())
  {
    mContractId->validateChecksum(client);
  }
}

//-----
HookEntityId& HookEntityId::setAccountId(const AccountId& accountId)
{
  mAccountId = accountId;
  mContractId.reset();
  return *this;
}

//-----
HookEntityId& HookEntityId::setContractId(const ContractId& contractId)
{
  mContractId = contractId;
  mAccountId.reset();
  return *this;
}

} // namespace Hiero
