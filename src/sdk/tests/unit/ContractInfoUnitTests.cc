// SPDX-License-Identifier: Apache-2.0
#include "ContractInfo.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <contract_get_info.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class ContractInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const std::string& getTestContractAccountId() const { return mTestContractAccountId; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mTestAutoRenewPeriod;
  }
  [[nodiscard]] inline const uint64_t& getTestStorage() const { return mTestStorage; }
  [[nodiscard]] inline const std::string& getTestMemo() const { return mTestMemo; }
  [[nodiscard]] inline const Hbar& getTestBalance() const { return mTestBalance; }
  [[nodiscard]] inline bool getTestIsDeleted() const { return mTestIsDeleted; }
  [[nodiscard]] inline const LedgerId& getTestLedgerId() const { return mTestLedgerId; }
  [[nodiscard]] inline const AccountId& getTestAutoRenewAccountId() const { return mTestAutoRenewAccountId; }
  [[nodiscard]] inline int32_t getTestMaxAutomaticTokenAssociations() const { return mMaxAutomaticTokenAssociations; }
  [[nodiscard]] inline bool getTestDeclineReward() const { return mTestDeclineReward; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestStakePeriodStart() const
  {
    return mTestStakePeriodStart;
  }
  [[nodiscard]] inline const Hbar& getTestPendingReward() const { return mTestPendingReward; }
  [[nodiscard]] inline const Hbar& getTestStakedToMe() const { return mTestStakedToMe; }
  [[nodiscard]] inline const AccountId& getTestStakedAccountId() const { return mTestStakedAccountId; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const AccountId mTestAccountId = AccountId(2ULL);
  const std::string mTestContractAccountId = "ContractAccountId";
  const std::shared_ptr<PublicKey> mTestAdminKey = PublicKey::fromStringDer(
    "302A300506032B6570032100D75A980182B10AB7D54BFED3C964073A0EE172f3DAA62325AF021A68F707511A");
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const std::chrono::system_clock::duration mTestAutoRenewPeriod = std::chrono::hours(3);
  const uint64_t mTestStorage = 40000ULL;
  const std::string mTestMemo = "test memo";
  const Hbar mTestBalance = Hbar(5LL);
  const bool mTestIsDeleted = true;
  const LedgerId mTestLedgerId = LedgerId({ std::byte(0x06), std::byte(0x07), std::byte(0x08) });
  const AccountId mTestAutoRenewAccountId = AccountId(9ULL);
  const int32_t mMaxAutomaticTokenAssociations = 10;
  const bool mTestDeclineReward = true;
  const std::chrono::system_clock::time_point mTestStakePeriodStart = std::chrono::system_clock::now();
  const Hbar mTestPendingReward = Hbar(11LL);
  const Hbar mTestStakedToMe = Hbar(12LL);
  const AccountId mTestStakedAccountId = AccountId(13ULL);
};

//-----
TEST_F(ContractInfoUnitTests, FromProtobuf)
{
  // Given
  proto::ContractGetInfoResponse_ContractInfo protoContractInfo;
  protoContractInfo.set_allocated_contractid(getTestContractId().toProtobuf().release());
  protoContractInfo.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  protoContractInfo.set_contractaccountid(getTestContractAccountId());
  protoContractInfo.set_deleted(getTestIsDeleted());
  protoContractInfo.set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  protoContractInfo.set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  protoContractInfo.set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  protoContractInfo.set_storage(static_cast<int64_t>(getTestStorage()));
  protoContractInfo.set_memo(getTestMemo());
  protoContractInfo.set_balance(static_cast<uint64_t>(getTestBalance().toTinybars()));
  protoContractInfo.set_deleted(getTestIsDeleted());
  protoContractInfo.set_ledger_id(internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));
  protoContractInfo.set_allocated_auto_renew_account_id(getTestAutoRenewAccountId().toProtobuf().release());
  protoContractInfo.set_max_automatic_token_associations(getTestMaxAutomaticTokenAssociations());

  protoContractInfo.mutable_staking_info()->set_decline_reward(getTestDeclineReward());
  protoContractInfo.mutable_staking_info()->set_allocated_stake_period_start(
    internal::TimestampConverter::toProtobuf(getTestStakePeriodStart()));
  protoContractInfo.mutable_staking_info()->set_pending_reward(getTestPendingReward().toTinybars());
  protoContractInfo.mutable_staking_info()->set_staked_to_me(getTestStakedToMe().toTinybars());
  protoContractInfo.mutable_staking_info()->set_allocated_staked_account_id(
    getTestStakedAccountId().toProtobuf().release());

  // When
  const ContractInfo contractInfo = ContractInfo::fromProtobuf(protoContractInfo);

  // Then
  EXPECT_EQ(contractInfo.mContractId, getTestContractId());
  EXPECT_EQ(contractInfo.mAccountId, getTestAccountId());
  EXPECT_EQ(contractInfo.mContractAccountId, getTestContractAccountId());
  EXPECT_EQ(contractInfo.mAdminKey->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(contractInfo.mExpirationTime, getTestExpirationTime());
  EXPECT_EQ(contractInfo.mAutoRenewPeriod, getTestAutoRenewPeriod());
  EXPECT_EQ(contractInfo.mStorage, getTestStorage());
  EXPECT_EQ(contractInfo.mMemo, getTestMemo());
  EXPECT_EQ(contractInfo.mBalance, getTestBalance());
  EXPECT_EQ(contractInfo.mIsDeleted, getTestIsDeleted());
  EXPECT_EQ(contractInfo.mLedgerId.toBytes(), getTestLedgerId().toBytes());
  ASSERT_TRUE(contractInfo.mAutoRenewAccountId.has_value());
  EXPECT_EQ(contractInfo.mAutoRenewAccountId, getTestAutoRenewAccountId());
  EXPECT_EQ(contractInfo.mMaxAutomaticTokenAssociations, getTestMaxAutomaticTokenAssociations());

  EXPECT_EQ(contractInfo.mStakingInfo.mDeclineRewards, getTestDeclineReward());
  EXPECT_EQ(contractInfo.mStakingInfo.mStakePeriodStart, getTestStakePeriodStart());
  EXPECT_EQ(contractInfo.mStakingInfo.mPendingReward, getTestPendingReward());
  EXPECT_EQ(contractInfo.mStakingInfo.mStakedToMe, getTestStakedToMe());
  ASSERT_TRUE(contractInfo.mStakingInfo.mStakedAccountId.has_value());
  EXPECT_EQ(contractInfo.mStakingInfo.mStakedAccountId, getTestStakedAccountId());
  EXPECT_FALSE(contractInfo.mStakingInfo.mStakedNodeId.has_value());
}
