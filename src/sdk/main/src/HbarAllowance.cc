// SPDX-License-Identifier: Apache-2.0
#include "HbarAllowance.h"

#include <crypto_approve_allowance.pb.h>

namespace Hiero
{
//-----
HbarAllowance::HbarAllowance(AccountId owner, AccountId spender, const Hbar& amount)
  : mOwnerAccountId(std::move(owner))
  , mSpenderAccountId(std::move(spender))
  , mAmount(amount)
{
}

//-----
HbarAllowance HbarAllowance::fromProtobuf(const proto::CryptoAllowance& proto)
{
  HbarAllowance allowance;

  if (proto.has_owner())
  {
    allowance.mOwnerAccountId = AccountId::fromProtobuf(proto.owner());
  }

  if (proto.has_spender())
  {
    allowance.mSpenderAccountId = AccountId::fromProtobuf(proto.spender());
  }

  allowance.mAmount = Hbar(proto.amount(), HbarUnit::TINYBAR());
  return allowance;
}

//-----
void HbarAllowance::validateChecksums(const Client& client) const
{
  mOwnerAccountId.validateChecksum(client);
  mSpenderAccountId.validateChecksum(client);
}

//-----
std::unique_ptr<proto::CryptoAllowance> HbarAllowance::toProtobuf() const
{
  auto proto = std::make_unique<proto::CryptoAllowance>();
  proto->set_allocated_owner(mOwnerAccountId.toProtobuf().release());
  proto->set_allocated_spender(mSpenderAccountId.toProtobuf().release());
  proto->set_amount(mAmount.toTinybars());
  return proto;
}

} // namespace Hiero
