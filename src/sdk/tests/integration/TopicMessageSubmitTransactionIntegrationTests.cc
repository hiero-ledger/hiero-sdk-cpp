// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "CustomFeeLimit.h"
#include "ED25519PrivateKey.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace Hiero;

class TopicMessageSubmitTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::string& getTestMemo() const { return mMemo; }

private:
  const std::string mMemo = "[e2e::TopicCreateTransaction]";
};

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, ExecuteTopicMessageSubmitTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(
    txResponses =
      TopicMessageSubmitTransaction().setTopicId(topicId).setMessage("Hello from HCS!").executeAll(getTestClient()));

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getReceipt(getTestClient()).mStatus, Status::SUCCESS);
  }

  TopicInfo topicInfo;
  ASSERT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo.mTopicId, topicId);
  EXPECT_EQ(topicInfo.mSequenceNumber, 1);

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, ExecuteLargeTopicMessageSubmitTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  const std::string testMemo = getTestMemo();
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .setMemo(testMemo)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  TopicInfo topicInfo;
  EXPECT_NO_THROW(topicInfo = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  ASSERT_EQ(topicInfo.mTopicId, topicId);
  ASSERT_EQ(topicInfo.mMemo, testMemo);
  ASSERT_EQ(topicInfo.mSequenceNumber, 0);
  ASSERT_NE(topicInfo.mAdminKey, nullptr);
  ASSERT_EQ(topicInfo.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // When
  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(
    txResponses =
      TopicMessageSubmitTransaction().setTopicId(topicId).setMessage(getTestBigContents()).executeAll(getTestClient()));

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getReceipt(getTestClient()).mStatus, Status::SUCCESS);
  }

  TopicInfo topicInfo2;
  ASSERT_NO_THROW(topicInfo2 = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo2.mTopicId, topicId);
  EXPECT_EQ(topicInfo2.mMemo, testMemo);
  EXPECT_EQ(topicInfo2.mSequenceNumber, 14);
  ASSERT_NE(topicInfo2.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo2.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, CannotSubmitTopicMessageWithoutTopicId)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  const std::string testMemo = getTestMemo();
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .setMemo(testMemo)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  std::vector<TransactionResponse> txResponses;
  ASSERT_THROW(
    txResponses =
      TopicMessageSubmitTransaction().setMessage(getTestBigContents()).setMaxChunks(15).executeAll(getTestClient()),
    PrecheckStatusException);

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getValidateStatus(), true);
    EXPECT_THROW(const TransactionReceipt txReceipt = resp.getReceipt(getTestClient()), ReceiptStatusException);
  }

  TopicInfo topicInfo2;
  ASSERT_NO_THROW(topicInfo2 = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo2.mTopicId, topicId);
  EXPECT_EQ(topicInfo2.mMemo, testMemo);
  EXPECT_EQ(topicInfo2.mSequenceNumber, 0);
  ASSERT_NE(topicInfo2.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo2.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, CanSubmitTopicMessageWithoutMessage)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  const std::string testMemo = getTestMemo();
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .setMemo(testMemo)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  std::vector<TransactionResponse> txResponses;
  EXPECT_NO_THROW(txResponses = TopicMessageSubmitTransaction().setTopicId(topicId).executeAll(getTestClient()));

  // Then
  for (const auto& resp : txResponses)
  {
    EXPECT_EQ(resp.getReceipt(getTestClient()).mStatus, Status::SUCCESS);
  }

  TopicInfo topicInfo2;
  ASSERT_NO_THROW(topicInfo2 = TopicInfoQuery().setTopicId(topicId).execute(getTestClient()));

  EXPECT_EQ(topicInfo2.mTopicId, topicId);
  EXPECT_EQ(topicInfo2.mMemo, testMemo);
  ASSERT_NE(topicInfo2.mAdminKey, nullptr);
  EXPECT_EQ(topicInfo2.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCanChargeHbarsWithLimit)
{
  // Given
  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  CustomFeeLimit limit;
  ASSERT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(CustomFixedFee().setAmount(feeAmount)));

  // When
  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .addCustomFeeLimit(limit)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountInfo.mBalance.toTinybars(), initialBalance.toTinybars() - feeAmount);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, DISABLED_RevenueGeneratingTopicCanChargeHbarsWithoutLimit)
{
  // Given
  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountInfo.mBalance.toTinybars(), initialBalance.toTinybars() - feeAmount);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, DISABLED_RevenueGeneratingTopicCanChargeTokensWithLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFeeLimit limit;
  EXPECT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId)));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .addCustomFeeLimit(limit)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountBalance.mTokens[tokenId], accountTokenBalance);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCanChargeTokensWithoutLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(accountBalance.mTokens[tokenId], accountTokenBalance);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicDoesNotChargeHbarsFeeExemptKeys)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addFeeExemptKey(accountKey)
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_LT(initialBalance.toTinybars() - accountInfo.mBalance.toTinybars(), feeAmount);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicDoesNotChargeTokensFeeExemptKeys)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);
  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addFeeExemptKey(accountKey)
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountBalance.mTokens[tokenId], accountTokenBalance);
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCannotChargeHbarsWithLowerLimit)
{
  // Given
  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(
                    getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  Hbar initialBalance = Hbar(3LL);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setInitialBalance(initialBalance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  CustomFeeLimit limit;
  ASSERT_NO_THROW(limit =
                    CustomFeeLimit().setPayerId(accountId).addCustomFee(CustomFixedFee().setAmount(feeAmount - 1)));

  // When / Then
  // Submit a message to the revenue generating topic with lower fee limit than the actual fee required
  setTestClientOperator(accountId, accountKey);
  EXPECT_THROW(TopicMessageSubmitTransaction()
                 .setTopicId(topicId)
                 .setMessage("message")
                 .addCustomFeeLimit(limit)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException); // DUPLICATE_ACCOUNT_ID_IN_MAX_CUSTOM_FEE_LIST
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCannotChargeTokensWithLowerLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(2).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFeeLimit limit;
  EXPECT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId)));

  setTestClientOperator(accountId, accountKey);
  EXPECT_THROW(TopicMessageSubmitTransaction()
                 .setTopicId(topicId)
                 .addCustomFeeLimit(limit)
                 .setMessage("message")
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException); // DUPLICATE_ACCOUNT_ID_IN_MAX_CUSTOM_FEE_LIST
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicCannotChargeTokensWithInvalidCustomFeeLimit)
{
  // Given
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 1;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -accountTokenBalance)
                    .addTokenTransfer(tokenId, accountId, accountTokenBalance)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFeeLimit limit;
  EXPECT_NO_THROW(limit = CustomFeeLimit().setPayerId(accountId).addCustomFee(
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(TokenId(0, 0, 0))));

  setTestClientOperator(accountId, accountKey);
  EXPECT_THROW(TopicMessageSubmitTransaction()
                 .setTopicId(topicId)
                 .addCustomFeeLimit(limit)
                 .setMessage("message")
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException); // DUPLICATE_ACCOUNT_ID_IN_MAX_CUSTOM_FEE_LIST
}

//-----
TEST_F(TopicMessageSubmitTransactionIntegrationTests, RevenueGeneratingTopicDoesNotChargeTreasuries)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  uint64_t accountTokenBalance = 10;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey->getPublicKey())
                                .setMaxAutomaticTokenAssociations(-1)
                                .setInitialBalance(Hbar(2LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(10)
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setTreasuryAccountId(accountId)
                              .freezeWith(&getTestClient())
                              .sign(accountKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(getTestClient().getOperatorAccountId().value())
                    .setTokenIds({ tokenId })
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  CustomFixedFee customFixedFee;
  ASSERT_NO_THROW(customFixedFee =
                    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(
                      getTestClient().getOperatorAccountId().value()));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                              .addCustomFixedFee({ customFixedFee })
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When
  EXPECT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, accountId, -1ULL)
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), 1ULL)
                    .freezeWith(&getTestClient())
                    .sign(accountKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  setTestClientOperator(accountId, accountKey);
  EXPECT_NO_THROW(TopicMessageSubmitTransaction()
                    .setTopicId(topicId)
                    .setMessage("message")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  setDefaultTestClientOperator();
  AccountBalance accountBalance;
  EXPECT_NO_THROW(accountBalance = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountBalance.mTokens[tokenId], accountTokenBalance - 1); // -1 as 1 was sent to the operator account
}