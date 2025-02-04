// SPDX-License-Identifier: Apache-2.0
#include "PendingAirdropRecord.h"
#include "PendingAirdropId.h"

#include <basic_types.pb.h>
#include <transaction_record.pb.h>

namespace Hiero
{
//-----
PendingAirdropRecord::PendingAirdropRecord(PendingAirdropId pendingAirdropId, uint64_t amount)
  : mPendingAirdropId(pendingAirdropId)
  , mAmount(amount)
{
}

//-----
PendingAirdropRecord PendingAirdropRecord::fromProtobuf(const proto::PendingAirdropRecord& proto)
{
  PendingAirdropRecord pendingAirdropRecord;

  pendingAirdropRecord.mPendingAirdropId = PendingAirdropId::fromProtobuf(proto.pending_airdrop_id());
  pendingAirdropRecord.mAmount = proto.pending_airdrop_value().amount();

  return pendingAirdropRecord;
}

//-----
std::unique_ptr<proto::PendingAirdropRecord> PendingAirdropRecord::toProtobuf() const
{
  auto proto = std::make_unique<proto::PendingAirdropRecord>();

  proto->set_allocated_pending_airdrop_id(mPendingAirdropId.toProtobuf().release());

  auto value = std::make_unique<proto::PendingAirdropValue>();
  value->set_amount(mAmount);

  proto->set_allocated_pending_airdrop_value(value.release());

  return proto;
}

//-----
std::string PendingAirdropRecord::toString() const
{
  return mPendingAirdropId.toString() + " " + std::to_string(mAmount);
}

} // namespace Hiero