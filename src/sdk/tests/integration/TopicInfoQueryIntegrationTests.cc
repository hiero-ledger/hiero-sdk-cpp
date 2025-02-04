// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "CustomFixedFee.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TopicInfoQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TopicInfoQueryIntegrationTests, ExecuteTopicInfoQuery)
{
  // Given
  const std::string memo = "test memo";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2200);

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setMemo(memo)
                              .setAdminKey(operatorKey)
                              .setSubmitKey(operatorKey)
                              .setAutoRenewPeriod(autoRenewPeriod)
                              .setAutoRenewAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  TopicInfo topicInfo;
  EXPECT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  // Then
  EXPECT_EQ(topicInfo.mTopicId, topicId);
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
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}
