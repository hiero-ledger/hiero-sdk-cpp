// SPDX-License-Identifier: Apache-2.0
#include "AccountUpdateTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "exceptions/IllegalStateException.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"

#include <crypto_update.pb.h>
#include <gtest/gtest.h>
#include <limits>
#include <transaction_body.pb.h>

using namespace Hiero;

class AccountUpdateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPublicKey() const { return mPublicKey; }
  [[nodiscard]] inline bool getTestReceiverSignatureRequired() const { return mReceiverSignatureRequired; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mAutoRenewPeriod;
  }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mExpirationTime;
  }
  [[nodiscard]] inline const std::string& getTestAccountMemo() const { return mAccountMemo; }
  [[nodiscard]] inline int32_t getTestMaximumTokenAssociations() const { return mMaxTokenAssociations; }
  [[nodiscard]] inline const AccountId& getTestStakedAccountId() const { return mStakedAccountId; }
  [[nodiscard]] inline const uint64_t& getTestStakedNodeId() const { return mStakedNodeId; }
  [[nodiscard]] inline bool getTestDeclineStakingReward() const { return mDeclineStakingReward; }

private:
  const AccountId mAccountId = AccountId(1ULL);
  const std::shared_ptr<PublicKey> mPublicKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const bool mReceiverSignatureRequired = true;
  const std::chrono::system_clock::duration mAutoRenewPeriod = std::chrono::hours(2);
  const std::chrono::system_clock::time_point mExpirationTime = std::chrono::system_clock::now();
  const std::string mAccountMemo = "test account memo";
  const int32_t mMaxTokenAssociations = 3;
  const AccountId mStakedAccountId = AccountId(4ULL);
  const uint64_t mStakedNodeId = 5ULL;
  const bool mDeclineStakingReward = true;
};

//-----
TEST_F(AccountUpdateTransactionUnitTests, ConstructAccountUpdateTransaction)
{
  // Given / When
  AccountUpdateTransaction transaction;

  // Then
  EXPECT_EQ(transaction.getAccountId(), AccountId());
  EXPECT_EQ(transaction.getKey(), nullptr);
  EXPECT_FALSE(transaction.getReceiverSignatureRequired().has_value());
  EXPECT_FALSE(transaction.getAutoRenewPeriod().has_value());
  EXPECT_FALSE(transaction.getExpirationTime().has_value());
  EXPECT_FALSE(transaction.getAccountMemo().has_value());
  EXPECT_FALSE(transaction.getMaxAutomaticTokenAssociations().has_value());
  EXPECT_FALSE(transaction.getStakedAccountId().has_value());
  EXPECT_FALSE(transaction.getStakedNodeId().has_value());
  EXPECT_FALSE(transaction.getDeclineStakingReward().has_value());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ConstructAccountUpdateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::CryptoUpdateTransactionBody>();
  body->set_allocated_accountidtoupdate(getTestAccountId().toProtobuf().release());
  body->set_allocated_key(getTestPublicKey()->toProtobufKey().release());
  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->set_allocated_staked_account_id(getTestAccountId().toProtobuf().release());

  auto boolValue = std::make_unique<google::protobuf::BoolValue>();
  boolValue->set_value(getTestReceiverSignatureRequired());
  body->set_allocated_receiversigrequiredwrapper(boolValue.release());

  boolValue = std::make_unique<google::protobuf::BoolValue>();
  boolValue->set_value(getTestDeclineStakingReward());
  body->set_allocated_decline_reward(boolValue.release());

  auto stringValue = std::make_unique<google::protobuf::StringValue>();
  stringValue->set_value(getTestAccountMemo());
  body->set_allocated_memo(stringValue.release());

  auto int32Value = std::make_unique<google::protobuf::Int32Value>();
  int32Value->set_value(getTestMaximumTokenAssociations());
  body->set_allocated_max_automatic_token_associations(int32Value.release());

  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoupdateaccount(body.release());

  // When
  const AccountUpdateTransaction accountUpdateTransaction(txBody);

  // Then
  EXPECT_EQ(accountUpdateTransaction.getAccountId(), getTestAccountId());
  ASSERT_NE(accountUpdateTransaction.getKey(), nullptr);
  EXPECT_EQ(accountUpdateTransaction.getKey()->toBytes(), getTestPublicKey()->toBytes());
  ASSERT_TRUE(accountUpdateTransaction.getReceiverSignatureRequired().has_value());
  EXPECT_EQ(accountUpdateTransaction.getReceiverSignatureRequired(), getTestReceiverSignatureRequired());
  ASSERT_TRUE(accountUpdateTransaction.getAutoRenewPeriod().has_value());
  EXPECT_EQ(accountUpdateTransaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
  ASSERT_TRUE(accountUpdateTransaction.getExpirationTime().has_value());
  EXPECT_EQ(accountUpdateTransaction.getExpirationTime(), getTestExpirationTime());
  ASSERT_TRUE(accountUpdateTransaction.getAccountMemo().has_value());
  EXPECT_EQ(accountUpdateTransaction.getAccountMemo(), getTestAccountMemo());
  ASSERT_TRUE(accountUpdateTransaction.getMaxAutomaticTokenAssociations().has_value());
  EXPECT_EQ(accountUpdateTransaction.getMaxAutomaticTokenAssociations(), getTestMaximumTokenAssociations());
  ASSERT_TRUE(accountUpdateTransaction.getStakedAccountId().has_value());
  EXPECT_EQ(accountUpdateTransaction.getStakedAccountId(), getTestAccountId());
  EXPECT_FALSE(accountUpdateTransaction.getStakedNodeId().has_value());
  ASSERT_TRUE(accountUpdateTransaction.getDeclineStakingReward().has_value());
  EXPECT_EQ(accountUpdateTransaction.getDeclineStakingReward(), getTestDeclineStakingReward());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetAccountId)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAccountId(getTestAccountId()));

  // Then
  EXPECT_EQ(transaction.getAccountId(), getTestAccountId());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetAccountIdFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAccountId(getTestAccountId()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetKey)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setKey(getTestPublicKey()));

  // Then
  EXPECT_EQ(transaction.getKey()->toBytes(), getTestPublicKey()->toBytes());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetKeyFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setKey(getTestPublicKey()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetReceiverSignatureRequired)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setReceiverSignatureRequired(getTestReceiverSignatureRequired()));

  // Then
  EXPECT_TRUE(transaction.getReceiverSignatureRequired());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetReceiverSignatureRequiredFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setReceiverSignatureRequired(getTestReceiverSignatureRequired()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetAutoRenewPeriod)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetAutoRenewPeriodFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetExpirationTime)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setExpirationTime(getTestExpirationTime()));

  // Then
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetExpirationTimeFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetAccountMemo)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAccountMemo(getTestAccountMemo()));

  // Then
  EXPECT_EQ(transaction.getAccountMemo(), getTestAccountMemo());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetAccountMemoFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAccountMemo(getTestAccountMemo()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ClearAccountMemo)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.clearAccountMemo());

  // Then
  EXPECT_TRUE(transaction.getAccountMemo().has_value());
  EXPECT_TRUE(transaction.getAccountMemo().value().empty());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ClearAccountMemoFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearAccountMemo(), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetMaxAutomaticTokenAssociations)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setMaxAutomaticTokenAssociations(getTestMaximumTokenAssociations()));

  // Then
  EXPECT_EQ(transaction.getMaxAutomaticTokenAssociations(), getTestMaximumTokenAssociations());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetMaxAutomaticTokenAssociationsFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setMaxAutomaticTokenAssociations(getTestMaximumTokenAssociations()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetStakedAccountId)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setStakedAccountId(getTestStakedAccountId()));

  // Then
  EXPECT_EQ(transaction.getStakedAccountId(), getTestStakedAccountId());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetStakedAccountIdFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setStakedAccountId(getTestStakedAccountId()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ClearStakedAccountId)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.clearStakedAccountId());

  // Then
  EXPECT_EQ(transaction.getStakedAccountId(), AccountId(0ULL));
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ClearStakedAccountIdFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearStakedAccountId(), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetStakedNodeId)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setStakedNodeId(getTestStakedNodeId()));

  // Then
  EXPECT_EQ(transaction.getStakedNodeId(), getTestStakedNodeId());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetStakedNodeIdFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setStakedNodeId(getTestStakedNodeId()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ClearStakedNodeId)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.clearStakedNodeId());

  // Then
  EXPECT_EQ(transaction.getStakedNodeId(), -1LL);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ClearStakedNodeIdFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearStakedNodeId(), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetStakingRewardPolicy)
{
  // Given
  AccountUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setDeclineStakingReward(getTestDeclineStakingReward()));

  // Then
  EXPECT_EQ(transaction.getDeclineStakingReward(), getTestDeclineStakingReward());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, SetStakingRewardPolicyFrozen)
{
  // Given
  AccountUpdateTransaction transaction = AccountUpdateTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setDeclineStakingReward(getTestDeclineStakingReward()), IllegalStateException);
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ResetStakedAccountId)
{
  // Given
  AccountUpdateTransaction transaction;
  ASSERT_NO_THROW(transaction.setStakedAccountId(getTestStakedAccountId()));

  // When
  EXPECT_NO_THROW(transaction.setStakedNodeId(getTestStakedNodeId()));

  // Then
  EXPECT_FALSE(transaction.getStakedAccountId().has_value());
  EXPECT_TRUE(transaction.getStakedNodeId().has_value());
}

//-----
TEST_F(AccountUpdateTransactionUnitTests, ResetStakedNodeId)
{
  // Given
  AccountUpdateTransaction transaction;
  ASSERT_NO_THROW(transaction.setStakedNodeId(getTestStakedNodeId()));

  // When
  EXPECT_NO_THROW(transaction.setStakedAccountId(getTestStakedAccountId()));

  // Then
  EXPECT_TRUE(transaction.getStakedAccountId().has_value());
  EXPECT_FALSE(transaction.getStakedNodeId().has_value());
}
