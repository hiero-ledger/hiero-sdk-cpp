// SPDX-License-Identifier: Apache-2.0
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "Defaults.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TopicCreateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TopicCreateTransactionIntegrationTests, ExecuteTopicCreateTransaction)
{
  // Given
  const std::string memo = "topic create test memo";
  const std::chrono::system_clock::duration autoRenewPeriod = DEFAULT_AUTO_RENEW_PERIOD + std::chrono::hours(10);

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TopicCreateTransaction()
                                .setMemo(memo)
                                .setAdminKey(operatorKey)
                                .setSubmitKey(operatorKey)
                                .setAutoRenewPeriod(autoRenewPeriod)
                                .setAutoRenewAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TopicInfo topicInfo;
  ASSERT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(txReceipt.mTopicId.value()).execute(getTestClient()));

  EXPECT_EQ(topicInfo.mTopicId, txReceipt.mTopicId.value());
  EXPECT_EQ(topicInfo.mMemo, memo);
  ASSERT_NE(topicInfo.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());
  ASSERT_NE(topicInfo.mSubmitKey, nullptr);
  EXPECT_EQ(topicInfo.mSubmitKey->toBytes(), operatorKey->getPublicKey()->toBytes());
  ASSERT_TRUE(topicInfo.mAutoRenewPeriod.has_value());
  EXPECT_EQ(topicInfo.mAutoRenewPeriod.value(), autoRenewPeriod);
  ASSERT_TRUE(topicInfo.mAutoRenewAccountId.has_value());
  EXPECT_EQ(topicInfo.mAutoRenewAccountId.value(), AccountId(2ULL));

  // Clean up
  ASSERT_NO_THROW(txReceipt = TopicDeleteTransaction()
                                .setTopicId(txReceipt.mTopicId.value())
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(TopicCreateTransactionIntegrationTests, CanCreateTopicWithNoFieldsSet)
{
  // Given / When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TopicCreateTransaction().execute(getTestClient()).getReceipt(getTestClient()));

  // Then
  EXPECT_TRUE(txReceipt.mTopicId.has_value());
}
