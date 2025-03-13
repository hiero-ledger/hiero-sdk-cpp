// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "CustomFixedFee.h"
#include "Defaults.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>

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

//-----
TEST_F(TopicCreateTransactionIntegrationTests, RevenueGeneratingTopicCanCreate)
{
  // Given
  std::vector<std::shared_ptr<Key>> exemptKeys;
  for (int i = 0; i < 2; i++)
  {
    ASSERT_NO_THROW(exemptKeys.push_back(ED25519PrivateKey::generatePrivateKey()));
  }
  const std::string topicMemo = "memo";

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
                              .setMemo(topicMemo)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeExemptKeys(exemptKeys)
                              .setCustomFixedFees({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // Then
  TopicInfo topicInfo;
  EXPECT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicMemo, topicInfo.mMemo);
  EXPECT_EQ(getTestClient().getOperatorPublicKey()->toBytes(), topicInfo.mAdminKey->toBytes());
  ASSERT_EQ(exemptKeys.size(), topicInfo.mFeeExemptKeys.size());
  EXPECT_EQ(std::dynamic_pointer_cast<PrivateKey>(exemptKeys[0])->getPublicKey()->toBytes(),
            topicInfo.mFeeExemptKeys[0]->toBytes());
  EXPECT_EQ(std::dynamic_pointer_cast<PrivateKey>(exemptKeys[1])->getPublicKey()->toBytes(),
            topicInfo.mFeeExemptKeys[1]->toBytes());
  EXPECT_EQ(customFixedFee.toString(), topicInfo.mCustomFixedFees[0].toString());
}

//-----
TEST_F(TopicCreateTransactionIntegrationTests, RevenueGeneratingTopicCannotCreateWithInvalidExemptKey)
{
  // Given
  std::vector<std::shared_ptr<Key>> exemptKeys;
  for (int i = 0; i < 11; i++)
  {
    ASSERT_NO_THROW(exemptKeys.push_back(ED25519PrivateKey::generatePrivateKey()));
  }

  // When / Then
  // Duplicate exempt key
  EXPECT_THROW(TopicCreateTransaction()
                 .setAdminKey(getTestClient().getOperatorPublicKey())
                 .setFeeExemptKeys({ exemptKeys[0], exemptKeys[0] })
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException); // FEE_EXEMPT_KEY_LIST_CONTAINS_DUPLICATED_KEYS

  // Set 11 keys
  EXPECT_THROW(TopicCreateTransaction()
                 .setAdminKey(getTestClient().getOperatorPublicKey())
                 .setFeeExemptKeys(exemptKeys)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // MAX_ENTRIES_FOR_FEE_EXEMPT_KEY_LIST_EXCEEDED
}

//-----
TEST_F(TopicCreateTransactionIntegrationTests, AutoSetAutoRenewAccount)
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
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TopicInfo topicInfo;
  ASSERT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(txReceipt.mTopicId.value()).execute(getTestClient()));

  ASSERT_TRUE(topicInfo.mAutoRenewAccountId.has_value());
  EXPECT_EQ(topicInfo.mAutoRenewAccountId.value(), AccountId(2ULL));
}

//-----
TEST_F(TopicCreateTransactionIntegrationTests, DoesNotAutoSetAutoRenewAccount)
{
  // Given
  const std::string memo = "topic create test memo";
  const std::chrono::system_clock::duration autoRenewPeriod = DEFAULT_AUTO_RENEW_PERIOD + std::chrono::hours(10);

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey)
                                .setInitialBalance(Hbar(5LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TopicCreateTransaction()
                                .setMemo(memo)
                                .setAdminKey(operatorKey)
                                .setSubmitKey(operatorKey)
                                .setAutoRenewPeriod(autoRenewPeriod)
                                .setAutoRenewAccountId(accountId)
                                .freezeWith(&getTestClient())
                                .sign(operatorKey)
                                .sign(accountKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TopicInfo topicInfo;
  ASSERT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(txReceipt.mTopicId.value()).execute(getTestClient()));

  ASSERT_TRUE(topicInfo.mAutoRenewAccountId.has_value());
  EXPECT_EQ(topicInfo.mAutoRenewAccountId.value(), accountId);
}