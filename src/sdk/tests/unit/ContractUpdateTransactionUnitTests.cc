// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ContractId.h"
#include "ContractUpdateTransaction.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "exceptions/IllegalStateException.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"

#include <contract_update.pb.h>
#include <gtest/gtest.h>
#include <limits>
#include <transaction_body.pb.h>

using namespace Hiero;

class ContractUpdateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mTestAutoRenewPeriod;
  }
  [[nodiscard]] inline const std::string& getTestContractMemo() const { return mTestContractMemo; }
  [[nodiscard]] inline int32_t getTestMaximumAutomaticTokenAssociations() const
  {
    return mTestMaximumAutomaticTokenAssociations;
  }
  [[nodiscard]] inline const AccountId& getTestAutoRenewAccountId() const { return mTestAutoRenewAccountId; }
  [[nodiscard]] inline const AccountId& getTestStakedAccountId() const { return mTestStakedAccountId; }
  [[nodiscard]] inline const uint64_t& getTestStakedNodeId() const { return mTestStakedNodeId; }
  [[nodiscard]] inline bool getTestDeclineStakingReward() const { return mTestDeclineStakingReward; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const std::shared_ptr<PublicKey> mTestAdminKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::chrono::system_clock::duration mTestAutoRenewPeriod = std::chrono::hours(2);
  const std::string mTestContractMemo = "test contract memo";
  const int32_t mTestMaximumAutomaticTokenAssociations = 3;
  const AccountId mTestAutoRenewAccountId = AccountId(4ULL);
  const AccountId mTestStakedAccountId = AccountId(5ULL);
  const uint64_t mTestStakedNodeId = 6ULL;
  const bool mTestDeclineStakingReward = true;
};

//-----
TEST_F(ContractUpdateTransactionUnitTests, ConstructContractUpdateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ContractUpdateTransactionBody>();
  body->set_allocated_contractid(getTestContractId().toProtobuf().release());
  body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  body->set_allocated_auto_renew_account_id(getTestAutoRenewAccountId().toProtobuf().release());
  body->set_allocated_staked_account_id(getTestStakedAccountId().toProtobuf().release());

  auto stringValue = std::make_unique<google::protobuf::StringValue>();
  stringValue->set_value(getTestContractMemo());
  body->set_allocated_memowrapper(stringValue.release());

  auto int32Value = std::make_unique<google::protobuf::Int32Value>();
  int32Value->set_value(getTestMaximumAutomaticTokenAssociations());
  body->set_allocated_max_automatic_token_associations(int32Value.release());

  auto boolValue = std::make_unique<google::protobuf::BoolValue>();
  boolValue->set_value(getTestDeclineStakingReward());
  body->set_allocated_decline_reward(boolValue.release());

  proto::TransactionBody txBody;
  txBody.set_allocated_contractupdateinstance(body.release());

  // When
  const ContractUpdateTransaction contractUpdateTransaction(txBody);

  // Then
  EXPECT_EQ(contractUpdateTransaction.getContractId(), getTestContractId());
  ASSERT_TRUE(contractUpdateTransaction.getExpirationTime().has_value());
  EXPECT_EQ(contractUpdateTransaction.getExpirationTime(), getTestExpirationTime());
  ASSERT_NE(contractUpdateTransaction.getAdminKey(), nullptr);
  EXPECT_EQ(contractUpdateTransaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
  ASSERT_TRUE(contractUpdateTransaction.getAutoRenewPeriod().has_value());
  EXPECT_EQ(contractUpdateTransaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
  ASSERT_TRUE(contractUpdateTransaction.getContractMemo().has_value());
  EXPECT_EQ(contractUpdateTransaction.getContractMemo(), getTestContractMemo());
  ASSERT_TRUE(contractUpdateTransaction.getMaxAutomaticTokenAssociations().has_value());
  EXPECT_EQ(contractUpdateTransaction.getMaxAutomaticTokenAssociations(), getTestMaximumAutomaticTokenAssociations());
  ASSERT_TRUE(contractUpdateTransaction.getAutoRenewAccountId().has_value());
  EXPECT_EQ(contractUpdateTransaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
  ASSERT_TRUE(contractUpdateTransaction.getStakedAccountId().has_value());
  EXPECT_EQ(contractUpdateTransaction.getStakedAccountId(), getTestStakedAccountId());
  EXPECT_FALSE(contractUpdateTransaction.getStakedNodeId().has_value());
  ASSERT_TRUE(contractUpdateTransaction.getDeclineStakingReward().has_value());
  EXPECT_EQ(contractUpdateTransaction.getDeclineStakingReward(), getTestDeclineStakingReward());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetContractId)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setContractId(getTestContractId()));

  // Then
  EXPECT_EQ(transaction.getContractId(), getTestContractId());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetContractIdFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContractId(getTestContractId()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetExpirationTime)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setExpirationTime(getTestExpirationTime()));

  // Then
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetExpirationTimeFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetAdminKey)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAdminKey(getTestAdminKey()));

  // Then
  EXPECT_EQ(transaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetAdminKeyFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAdminKey(getTestAdminKey()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetAutoRenewPeriod)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetAutoRenewPeriodFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetContractMemo)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setContractMemo(getTestContractMemo()));

  // Then
  EXPECT_EQ(transaction.getContractMemo(), getTestContractMemo());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetContractMemoTooLarge)
{
  // Given
  ContractUpdateTransaction transaction;

  // When / Then
  EXPECT_THROW(transaction.setContractMemo(std::string(101, 'a')), std::length_error);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetContractMemoFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContractMemo(getTestContractMemo()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetMaxAutomaticTokenAssociations)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setMaxAutomaticTokenAssociations(getTestMaximumAutomaticTokenAssociations()));

  // Then
  EXPECT_EQ(transaction.getMaxAutomaticTokenAssociations(), getTestMaximumAutomaticTokenAssociations());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetMaxAutomaticTokenAssociationsFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setMaxAutomaticTokenAssociations(getTestMaximumAutomaticTokenAssociations()),
               IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetAutoRenewAccountId)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetAutoRenewAccountIdFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetStakedAccountId)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setStakedAccountId(getTestStakedAccountId()));

  // Then
  EXPECT_EQ(transaction.getStakedAccountId(), getTestStakedAccountId());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetStakedAccountIdFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setStakedAccountId(getTestStakedAccountId()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetStakedNodeId)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setStakedNodeId(getTestStakedNodeId()));

  // Then
  EXPECT_EQ(transaction.getStakedNodeId(), getTestStakedNodeId());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetStakedNodeIdFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setStakedNodeId(getTestStakedNodeId()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, GetSetStakingRewardPolicy)
{
  // Given
  ContractUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setDeclineStakingReward(getTestDeclineStakingReward()));

  // Then
  EXPECT_EQ(transaction.getDeclineStakingReward(), getTestDeclineStakingReward());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, SetStakingRewardPolicyFrozen)
{
  // Given
  ContractUpdateTransaction transaction = ContractUpdateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setDeclineStakingReward(getTestDeclineStakingReward()), IllegalStateException);
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, ResetStakedAccountId)
{
  // Given
  ContractUpdateTransaction transaction;
  ASSERT_NO_THROW(transaction.setStakedAccountId(getTestStakedAccountId()));

  // When
  EXPECT_NO_THROW(transaction.setStakedNodeId(getTestStakedNodeId()));

  // Then
  EXPECT_FALSE(transaction.getStakedAccountId().has_value());
  EXPECT_TRUE(transaction.getStakedNodeId().has_value());
}

//-----
TEST_F(ContractUpdateTransactionUnitTests, ResetStakedNodeId)
{
  // Given
  ContractUpdateTransaction transaction;
  ASSERT_NO_THROW(transaction.setStakedNodeId(getTestStakedNodeId()));

  // When
  EXPECT_NO_THROW(transaction.setStakedAccountId(getTestStakedAccountId()));

  // Then
  EXPECT_TRUE(transaction.getStakedAccountId().has_value());
  EXPECT_FALSE(transaction.getStakedNodeId().has_value());
}
