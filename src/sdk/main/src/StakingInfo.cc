// SPDX-License-Identifier: Apache-2.0
#include "StakingInfo.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
StakingInfo StakingInfo::fromProtobuf(const proto::StakingInfo& proto)
{
  StakingInfo stakingInfo;
  stakingInfo.mDeclineRewards = proto.decline_reward();

  if (proto.has_stake_period_start())
  {
    stakingInfo.mStakePeriodStart = internal::TimestampConverter::fromProtobuf(proto.stake_period_start());
  }

  stakingInfo.mPendingReward = Hbar(proto.pending_reward(), HbarUnit::TINYBAR());
  stakingInfo.mStakedToMe = Hbar(proto.staked_to_me(), HbarUnit::TINYBAR());

  if (proto.has_staked_account_id())
  {
    stakingInfo.mStakedAccountId = AccountId::fromProtobuf(proto.staked_account_id());
  }

  if (proto.has_staked_node_id())
  {
    stakingInfo.mStakedNodeId = static_cast<uint64_t>(proto.staked_node_id());
  }

  return stakingInfo;
}

//-----
StakingInfo StakingInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::StakingInfo proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::StakingInfo> StakingInfo::toProtobuf() const
{
  auto proto = std::make_unique<proto::StakingInfo>();
  proto->set_decline_reward(mDeclineRewards);

  if (mStakePeriodStart.has_value())
  {
    proto->set_allocated_stake_period_start(internal::TimestampConverter::toProtobuf(mStakePeriodStart.value()));
  }

  proto->set_pending_reward(mPendingReward.toTinybars());
  proto->set_staked_to_me(mStakedToMe.toTinybars());

  if (mStakedAccountId.has_value())
  {
    proto->set_allocated_staked_account_id(mStakedAccountId->toProtobuf().release());
  }
  else if (mStakedNodeId.has_value())
  {
    proto->set_staked_node_id(static_cast<int64_t>(mStakedNodeId.value()));
  }

  return proto;
}

//-----
std::vector<std::byte> StakingInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string StakingInfo::toString() const
{
  nlohmann::json json;
  json["mDeclineRewards"] = mDeclineRewards;

  if (mStakePeriodStart.has_value())
  {
    json["mStakePeriodStart"] = internal::TimestampConverter::toString(mStakePeriodStart.value());
  }

  json["mPendingReward"] = mPendingReward.toString();
  json["mStakedToMe"] = mStakedToMe.toString();

  if (mStakedAccountId.has_value())
  {
    json["mStakedAccountId"] = mStakedAccountId->toString();
  }
  else if (mStakedNodeId.has_value())
  {
    json["mStakedNodeId"] = mStakedNodeId.value();
  }

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const StakingInfo& info)
{
  os << info.toString();
  return os;
}

} // namespace Hiero
