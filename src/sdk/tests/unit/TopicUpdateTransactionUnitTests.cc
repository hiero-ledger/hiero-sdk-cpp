// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "TopicUpdateTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TopicUpdateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TopicId& getTestTopicId() const { return mTestTopicId; }
  [[nodiscard]] inline const std::string& getTestTopicMemo() const { return mTestTopicMemo; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::shared_ptr<ED25519PrivateKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const std::shared_ptr<ED25519PrivateKey>& getTestSubmitKey() const { return mTestSubmitKey; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mTestAutoRenewPeriod;
  }
  [[nodiscard]] inline const AccountId& getTestAutoRenewAccountId() const { return mTestAutoRenewAccountId; }

private:
  const TopicId mTestTopicId = TopicId(1ULL, 2ULL, 3ULL);
  const std::string mTestTopicMemo = "test topic memo";
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const std::shared_ptr<ED25519PrivateKey> mTestAdminKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ED25519PrivateKey> mTestSubmitKey = ED25519PrivateKey::generatePrivateKey();
  const std::chrono::system_clock::duration mTestAutoRenewPeriod = std::chrono::hours(4);
  const AccountId mTestAutoRenewAccountId = AccountId(5ULL, 6ULL, 7ULL);
};

//-----
TEST_F(TopicUpdateTransactionUnitTests, ConstructTopicUpdateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ConsensusUpdateTopicTransactionBody>();
  body->set_allocated_topicid(getTestTopicId().toProtobuf().release());
  body->mutable_memo()->set_value(getTestTopicMemo());
  body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  body->set_allocated_submitkey(getTestSubmitKey()->toProtobufKey().release());
  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  body->set_allocated_autorenewaccount(getTestAutoRenewAccountId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_consensusupdatetopic(body.release());

  // When
  const TopicUpdateTransaction topicUpdateTransaction(txBody);

  // Then
  EXPECT_EQ(topicUpdateTransaction.getTopicId(), getTestTopicId());
  EXPECT_EQ(topicUpdateTransaction.getMemo(), getTestTopicMemo());
  EXPECT_EQ(topicUpdateTransaction.getExpirationTime(), getTestExpirationTime());
  EXPECT_EQ(topicUpdateTransaction.getAdminKey()->toBytes(), getTestAdminKey()->getPublicKey()->toBytes());
  EXPECT_EQ(topicUpdateTransaction.getSubmitKey()->toBytes(), getTestSubmitKey()->getPublicKey()->toBytes());
  EXPECT_EQ(topicUpdateTransaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
  EXPECT_EQ(topicUpdateTransaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ConstructTopicUpdateTransactionFromWrongTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::CryptoDeleteTransactionBody>();
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodelete(body.release());

  // When / Then
  EXPECT_THROW(const TopicUpdateTransaction topicCreateTransaction(txBody), std::invalid_argument);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetTopicId)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTopicId(getTestTopicId()));

  // Then
  EXPECT_EQ(transaction.getTopicId(), getTestTopicId());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetTopicIdFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTopicId(getTestTopicId()), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetMemo)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setMemo(getTestTopicMemo()));

  // Then
  EXPECT_EQ(transaction.getMemo(), getTestTopicMemo());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetMemoFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setMemo(getTestTopicMemo()), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearMemo)
{
  // Given
  TopicUpdateTransaction transaction;
  ASSERT_NO_THROW(transaction.setMemo(getTestTopicMemo()));

  // When
  EXPECT_NO_THROW(transaction.clearTopicMemo());

  // Then
  EXPECT_TRUE(transaction.getMemo()->empty());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearMemoFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearTopicMemo(), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetExpirationTime)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setExpirationTime(getTestExpirationTime()));

  // Then
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetExpirationTimeFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetAdminKey)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAdminKey(getTestAdminKey()));

  // Then
  EXPECT_EQ(transaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetAdminKeyFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAdminKey(getTestAdminKey()), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearAdminKey)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.clearAdminKey());

  // Then
  EXPECT_NE(transaction.getAdminKey(), nullptr);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearAdminKeyFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearAdminKey(), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetSubmitKey)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setSubmitKey(getTestSubmitKey()));

  // Then
  EXPECT_EQ(transaction.getSubmitKey()->toBytes(), getTestSubmitKey()->toBytes());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetSubmitKeyFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setSubmitKey(getTestSubmitKey()), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearSubmitKey)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.clearSubmitKey());

  // Then
  EXPECT_NE(transaction.getSubmitKey(), nullptr);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearSubmitKeyFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearAdminKey(), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetAutoRenewPeriod)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetAutoRenewPeriodFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetAutoRenewAccountId)
{
  // Given
  TopicUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, GetSetAutoRenewAccountIdFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()), IllegalStateException);
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearAutoRenewAccountId)
{
  // Given
  TopicUpdateTransaction transaction;
  ASSERT_NO_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()));

  // When
  EXPECT_NO_THROW(transaction.clearAutoRenewAccountId());

  // Then
  EXPECT_EQ(transaction.getAutoRenewAccountId(), AccountId());
}

//-----
TEST_F(TopicUpdateTransactionUnitTests, ClearAutoRenewAccountIdFrozen)
{
  // Given
  TopicUpdateTransaction transaction = TopicUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearAutoRenewAccountId(), IllegalStateException);
}
