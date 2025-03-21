// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Defaults.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TopicUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TopicUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TopicUpdateTransactionIntegrationTests, ExecuteTopicUpdateTransaction)
{
  // Given
  const std::string newMemo = "new topic create test memo";
  const std::chrono::system_clock::duration newAutoRenewPeriod = DEFAULT_AUTO_RENEW_PERIOD + std::chrono::hours(10);

  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TopicUpdateTransaction()
                                .setTopicId(topicId)
                                .setMemo(newMemo)
                                .setAdminKey(newKey)
                                .setSubmitKey(newKey)
                                .setAutoRenewPeriod(newAutoRenewPeriod)
                                .setAutoRenewAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TopicInfo topicInfo;
  ASSERT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo.mTopicId, topicId);
  EXPECT_EQ(topicInfo.mMemo, newMemo);
  ASSERT_NE(topicInfo.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo.mAdminKey->toBytes(), newKey->getPublicKey()->toBytes());
  ASSERT_NE(topicInfo.mSubmitKey, nullptr);
  EXPECT_EQ(topicInfo.mSubmitKey->toBytes(), newKey->getPublicKey()->toBytes());
  ASSERT_TRUE(topicInfo.mAutoRenewPeriod.has_value());
  EXPECT_EQ(topicInfo.mAutoRenewPeriod.value(), newAutoRenewPeriod);
  ASSERT_TRUE(topicInfo.mAutoRenewAccountId.has_value());
  EXPECT_EQ(topicInfo.mAutoRenewAccountId.value(), AccountId(2ULL));

  // Clean up
  ASSERT_NO_THROW(txReceipt = TopicDeleteTransaction()
                                .setTopicId(topicId)
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(TopicUpdateTransactionIntegrationTests, RevenueGeneratingTopicCannotUpdateFeeScheduleKey)
{
  // Given
  std::shared_ptr<PrivateKey> feeScheduleKey;
  ASSERT_NO_THROW(feeScheduleKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TopicId topicId;
  EXPECT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // Then
  // Update the revenue generating topic with new fee schedule key
  EXPECT_THROW(TopicUpdateTransaction()
                 .setTopicId(topicId)
                 .setFeeScheduleKey(feeScheduleKey->getPublicKey())
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // FEE_SCHEDULE_KEY_CANNOT_BE_UPDATED
}

//-----
TEST_F(TopicUpdateTransactionIntegrationTests, RevenueGeneratingTopicCannotUpdateCustomFees)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(2).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  // When
  TopicId topicId;
  EXPECT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // Then
  // Update the revenue generating topic with new custom fees
  EXPECT_THROW(TopicUpdateTransaction()
                 .setTopicId(topicId)
                 .setCustomFixedFees({ customFixedFee })
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // FEE_SCHEDULE_KEY_NOT_SET
}
