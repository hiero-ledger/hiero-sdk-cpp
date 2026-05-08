// SPDX-License-Identifier: Apache-2.0
#include "StakingInfo.h"
#include "impl/TimestampConverter.h"

#include <chrono>
#include <gtest/gtest.h>
#include <services/basic_types.pb.h>

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

//-----
TEST_F(StakingInfoUnitTests, ToString)
{
  // Given
  StakingInfo stakingInfo;
  stakingInfo.mDeclineRewards = getTestDeclineReward();
  stakingInfo.mStakePeriodStart = getTestStakePeriodStart();
  stakingInfo.mPendingReward = Hbar(getTestPendingReward(), HbarUnit::TINYBAR());
  stakingInfo.mStakedToMe = Hbar(getTestStakedToMe(), HbarUnit::TINYBAR());
  stakingInfo.mStakedNodeId = getTestStakedNodeId();

  // When
  const std::string result = stakingInfo.toString();

  // Then
  EXPECT_NE(result.find("mDeclineRewards"), std::string::npos);
  EXPECT_NE(result.find("mStakePeriodStart"), std::string::npos);
  EXPECT_NE(result.find("mPendingReward"), std::string::npos);
  EXPECT_NE(result.find("mStakedToMe"), std::string::npos);
  EXPECT_NE(result.find(std::to_string(getTestStakedNodeId())), std::string::npos);
}

TEST_F(StakingInfoUnitTests, ToProtobuf)
{
  // Given
  StakingInfo stakingInfo;
  stakingInfo.mDeclineRewards = getTestDeclineReward();
  stakingInfo.mStakePeriodStart = getTestStakePeriodStart();
  stakingInfo.mPendingReward = Hbar(getTestPendingReward(), HbarUnit::TINYBAR());
  stakingInfo.mStakedToMe = Hbar(getTestStakedToMe(), HbarUnit::TINYBAR());
  stakingInfo.mStakedNodeId = getTestStakedNodeId();

  // When
  const std::unique_ptr<proto::StakingInfo> protoStakingInfo = stakingInfo.toProtobuf();

  // Then
  EXPECT_EQ(protoStakingInfo->decline_reward(), getTestDeclineReward());
  ASSERT_TRUE(protoStakingInfo->has_stake_period_start());
  EXPECT_EQ(internal::TimestampConverter::fromProtobuf(protoStakingInfo->stake_period_start()),
            getTestStakePeriodStart());
  EXPECT_EQ(protoStakingInfo->pending_reward(), getTestPendingReward());
  EXPECT_EQ(protoStakingInfo->staked_to_me(), getTestStakedToMe());
  EXPECT_FALSE(protoStakingInfo->has_staked_account_id());
  ASSERT_TRUE(protoStakingInfo->has_staked_node_id());
  EXPECT_EQ(protoStakingInfo->staked_node_id(), getTestStakedNodeId());
}

//-----
TEST_F(StakingInfoUnitTests, OperatorEqual)
{
  // Given
  StakingInfo stakingInfo1;
  StakingInfo stakingInfo2;

  // Two default-constructed instances are equal
  EXPECT_EQ(stakingInfo1, stakingInfo2);

  // Two identically-constructed instances are equal
  stakingInfo1.mDeclineRewards = getTestDeclineReward();
  stakingInfo1.mStakePeriodStart = getTestStakePeriodStart();
  stakingInfo1.mPendingReward = Hbar(getTestPendingReward(), HbarUnit::TINYBAR());
  stakingInfo1.mStakedToMe = Hbar(getTestStakedToMe(), HbarUnit::TINYBAR());
  stakingInfo1.mStakedNodeId = getTestStakedNodeId();

  stakingInfo2 = stakingInfo1;
  EXPECT_EQ(stakingInfo1, stakingInfo2);

  // Instances differing in each field are not equal
  stakingInfo2.mDeclineRewards = !getTestDeclineReward();
  EXPECT_NE(stakingInfo1, stakingInfo2);
  stakingInfo2 = stakingInfo1;

  stakingInfo2.mStakePeriodStart = getTestStakePeriodStart() + std::chrono::seconds(1);
  EXPECT_NE(stakingInfo1, stakingInfo2);
  stakingInfo2 = stakingInfo1;

  stakingInfo2.mPendingReward = Hbar(getTestPendingReward() + 1LL, HbarUnit::TINYBAR());
  EXPECT_NE(stakingInfo1, stakingInfo2);
  stakingInfo2 = stakingInfo1;

  stakingInfo2.mStakedToMe = Hbar(getTestStakedToMe() + 1LL, HbarUnit::TINYBAR());
  EXPECT_NE(stakingInfo1, stakingInfo2);
  stakingInfo2 = stakingInfo1;

  stakingInfo2.mStakedAccountId = AccountId(1ULL);
  EXPECT_NE(stakingInfo1, stakingInfo2);
  stakingInfo2 = stakingInfo1;

  stakingInfo2.mStakedNodeId = getTestStakedNodeId() + 1LL;
  EXPECT_NE(stakingInfo1, stakingInfo2);
}