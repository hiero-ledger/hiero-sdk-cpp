// SPDX-License-Identifier: Apache-2.0
#include "StakingInfo.h"
#include "impl/TimestampConverter.h"

#include <basic_types.pb.h>
#include <chrono>
#include <gtest/gtest.h>

using namespace Hiero;

class StakingInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline bool getTestDeclineReward() const { return mTestDeclineReward; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestStakePeriodStart() const
  {
    return mTestStakePeriodStart;
  }
  [[nodiscard]] inline const int64_t& getTestPendingReward() const { return mTestPendingReward; }
  [[nodiscard]] inline const int64_t& getTestStakedToMe() const { return mTestStakedToMe; }
  [[nodiscard]] inline const int64_t& getTestStakedNodeId() const { return mTestStakedNodeId; }

private:
  const bool mTestDeclineReward = true;
  const std::chrono::system_clock::time_point mTestStakePeriodStart = std::chrono::system_clock::now();
  const int64_t mTestPendingReward = 1LL;
  const int64_t mTestStakedToMe = 2LL;
  const int64_t mTestStakedNodeId = 3LL;
};

//-----
TEST_F(StakingInfoUnitTests, FromProtobuf)
{
  // Given
  proto::StakingInfo protoStakingInfo;
  protoStakingInfo.set_decline_reward(getTestDeclineReward());
  protoStakingInfo.set_allocated_stake_period_start(
    internal::TimestampConverter::toProtobuf(getTestStakePeriodStart()));
  protoStakingInfo.set_pending_reward(getTestPendingReward());
  protoStakingInfo.set_staked_to_me(getTestStakedToMe());
  protoStakingInfo.set_staked_node_id(getTestStakedNodeId());

  // When
  const StakingInfo stakingInfo = StakingInfo::fromProtobuf(protoStakingInfo);

  // Then
  EXPECT_EQ(stakingInfo.mDeclineRewards, getTestDeclineReward());
  ASSERT_TRUE(stakingInfo.mStakePeriodStart.has_value());
  EXPECT_EQ(*stakingInfo.mStakePeriodStart, getTestStakePeriodStart());
  EXPECT_EQ(stakingInfo.mPendingReward.toTinybars(), getTestPendingReward());
  EXPECT_EQ(stakingInfo.mStakedToMe.toTinybars(), getTestStakedToMe());
  EXPECT_FALSE(stakingInfo.mStakedAccountId.has_value());
  ASSERT_TRUE(stakingInfo.mStakedNodeId.has_value());
  EXPECT_EQ(*stakingInfo.mStakedNodeId, getTestStakedNodeId());
}
