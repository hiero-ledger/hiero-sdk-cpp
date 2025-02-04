// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "AccountInfo.h"
#include "Hbar.h"
#include "LedgerId.h"
#include "PublicKey.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <chrono>
#include <crypto_get_info.pb.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace Hiero;

class AccountInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const std::string& getTestContractAccountId() const { return mTestContractAccountId; }
  [[nodiscard]] inline bool getTestIsDeleted() const { return mTestIsDeleted; }
  [[nodiscard]] inline const Hbar& getTestProxyReceived() const { return mTestProxyReceived; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPublicKey() const { return mTestPublicKey; }
  [[nodiscard]] inline const Hbar& getTestBalance() const { return mTestBalance; }
  [[nodiscard]] inline bool getTestReceiverSignatureRequired() const { return mTestReceiverSignatureRequired; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mTestAutoRenewPeriod;
  }
  [[nodiscard]] inline const std::string& getTestMemo() const { return mTestMemo; }
  [[nodiscard]] inline const uint64_t& getTestOwnedNfts() const { return mTestOwnedNfts; }
  [[nodiscard]] inline int32_t getTestMaxAutomaticTokenAssociations() const { return mMaxAutomaticTokenAssociations; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPublicKeyAlias() const { return mTestPublicKeyAlias; }
  [[nodiscard]] inline const LedgerId& getTestLedgerId() const { return mTestLedgerId; }
  [[nodiscard]] inline bool getTestDeclineReward() const { return mTestDeclineReward; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestStakePeriodStart() const
  {
    return mTestStakePeriodStart;
  }
  [[nodiscard]] inline const Hbar& getTestPendingReward() const { return mTestPendingReward; }
  [[nodiscard]] inline const Hbar& getTestStakedToMe() const { return mTestStakedToMe; }
  [[nodiscard]] inline const AccountId& getTestStakedAccountId() const { return mTestStakedAccountId; }

private:
  const AccountId mTestAccountId = AccountId(1ULL);
  const std::string mTestContractAccountId = "ContractAccountId";
  const bool mTestIsDeleted = true;
  const Hbar mTestProxyReceived = Hbar(2LL);
  const std::shared_ptr<PublicKey> mTestPublicKey = PublicKey::fromStringDer(
    "302A300506032B6570032100d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a");
  const Hbar mTestBalance = Hbar(3LL);
  const bool mTestReceiverSignatureRequired = true;
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const std::chrono::system_clock::duration mTestAutoRenewPeriod = std::chrono::hours(4);
  const std::string mTestMemo = "test memo";
  const uint64_t mTestOwnedNfts = 5ULL;
  const int32_t mMaxAutomaticTokenAssociations = 6;
  const std::shared_ptr<PublicKey> mTestPublicKeyAlias = PublicKey::fromStringDer(
    "302A300506032B6570032100fc51cd8e6218a1a38da47ed00230f0580816ed13ba3303ac5deb911548908025");
  const LedgerId mTestLedgerId = LedgerId({ std::byte(0x07), std::byte(0x08), std::byte(0x09) });
  const bool mTestDeclineReward = true;
  const std::chrono::system_clock::time_point mTestStakePeriodStart = std::chrono::system_clock::now();
  const Hbar mTestPendingReward = Hbar(10LL);
  const Hbar mTestStakedToMe = Hbar(11LL);
  const AccountId mTestStakedAccountId = AccountId(12ULL);
};

//-----
TEST_F(AccountInfoUnitTests, FromProtobuf)
{
  // Given
  proto::CryptoGetInfoResponse_AccountInfo protoAccountInfo;
  protoAccountInfo.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  protoAccountInfo.set_allocated_contractaccountid(new std::string(getTestContractAccountId()));
  protoAccountInfo.set_deleted(getTestIsDeleted());
  protoAccountInfo.set_proxyreceived(getTestProxyReceived().toTinybars());
  protoAccountInfo.set_allocated_key(getTestPublicKey()->toProtobufKey().release());
  protoAccountInfo.set_balance(static_cast<uint64_t>(getTestBalance().toTinybars()));
  protoAccountInfo.set_receiversigrequired(getTestReceiverSignatureRequired());
  protoAccountInfo.set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  protoAccountInfo.set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  protoAccountInfo.set_memo(getTestMemo());
  protoAccountInfo.set_ownednfts(static_cast<int64_t>(getTestOwnedNfts()));
  protoAccountInfo.set_max_automatic_token_associations(getTestMaxAutomaticTokenAssociations());
  protoAccountInfo.set_alias(getTestPublicKeyAlias()->toProtobufKey()->SerializeAsString());
  protoAccountInfo.set_ledger_id(internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));

  protoAccountInfo.mutable_staking_info()->set_decline_reward(getTestDeclineReward());
  protoAccountInfo.mutable_staking_info()->set_allocated_stake_period_start(
    internal::TimestampConverter::toProtobuf(getTestStakePeriodStart()));
  protoAccountInfo.mutable_staking_info()->set_pending_reward(getTestPendingReward().toTinybars());
  protoAccountInfo.mutable_staking_info()->set_staked_to_me(getTestStakedToMe().toTinybars());
  protoAccountInfo.mutable_staking_info()->set_allocated_staked_account_id(
    getTestStakedAccountId().toProtobuf().release());

  // When
  const AccountInfo accountInfo = AccountInfo::fromProtobuf(protoAccountInfo);

  // Then
  EXPECT_EQ(accountInfo.mAccountId, getTestAccountId());
  EXPECT_EQ(accountInfo.mContractAccountId, getTestContractAccountId());
  EXPECT_EQ(accountInfo.mIsDeleted, getTestIsDeleted());
  EXPECT_EQ(accountInfo.mProxyReceived, getTestProxyReceived());
  EXPECT_EQ(accountInfo.mKey->toBytes(), getTestPublicKey()->toBytes());
  EXPECT_EQ(accountInfo.mBalance, getTestBalance());
  EXPECT_EQ(accountInfo.mReceiverSignatureRequired, getTestReceiverSignatureRequired());
  EXPECT_EQ(accountInfo.mExpirationTime, getTestExpirationTime());
  EXPECT_EQ(accountInfo.mAutoRenewPeriod, getTestAutoRenewPeriod());
  EXPECT_EQ(accountInfo.mMemo, getTestMemo());
  EXPECT_EQ(accountInfo.mOwnedNfts, getTestOwnedNfts());
  EXPECT_EQ(accountInfo.mMaxAutomaticTokenAssociations, getTestMaxAutomaticTokenAssociations());
  EXPECT_EQ(accountInfo.mPublicKeyAlias->toBytesDer(), getTestPublicKeyAlias()->toBytesDer());
  EXPECT_EQ(accountInfo.mLedgerId.toBytes(), getTestLedgerId().toBytes());
  EXPECT_EQ(accountInfo.mStakingInfo.mDeclineRewards, getTestDeclineReward());
  EXPECT_EQ(accountInfo.mStakingInfo.mStakePeriodStart, getTestStakePeriodStart());
  EXPECT_EQ(accountInfo.mStakingInfo.mPendingReward, getTestPendingReward());
  EXPECT_EQ(accountInfo.mStakingInfo.mStakedToMe, getTestStakedToMe());
  ASSERT_TRUE(accountInfo.mStakingInfo.mStakedAccountId.has_value());
  EXPECT_EQ(accountInfo.mStakingInfo.mStakedAccountId, getTestStakedAccountId());
  EXPECT_FALSE(accountInfo.mStakingInfo.mStakedNodeId.has_value());
}
