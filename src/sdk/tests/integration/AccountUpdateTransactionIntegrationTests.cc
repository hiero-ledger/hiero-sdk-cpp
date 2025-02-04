// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "AccountUpdateTransaction.h"
#include "BaseIntegrationTest.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, ExecuteAccountUpdateTransaction)
{
  // Given
  const std::shared_ptr<PrivateKey> initialPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> newPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const bool newReceiverSignatureRequired = true;
  const std::chrono::system_clock::duration newAutoRenewPeriod = std::chrono::seconds(8000000);
  const std::chrono::system_clock::time_point newExpirationTime =
    std::chrono::system_clock::now() + std::chrono::seconds(3000000);
  const std::string newAccountMemo = "New Account Memo!";
  const int32_t newMaxAutomaticTokenAssociations = 100;
  const uint64_t newStakedNodeId = 0ULL;
  const bool newDeclineStakingRewards = true;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(initialPrivateKey->getPublicKey())
                                .setAutoRenewPeriod(std::chrono::seconds(2592000))
                                .setAccountMemo("test account memo")
                                .setMaxAutomaticTokenAssociations(10)
                                .setStakedAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountUpdateTransaction()
                                 .setAccountId(accountId)
                                 .setKey(newPrivateKey->getPublicKey())
                                 .setReceiverSignatureRequired(newReceiverSignatureRequired)
                                 .setAutoRenewPeriod(newAutoRenewPeriod)
                                 .setExpirationTime(newExpirationTime)
                                 .setAccountMemo(newAccountMemo)
                                 .setMaxAutomaticTokenAssociations(newMaxAutomaticTokenAssociations)
                                 .setStakedNodeId(newStakedNodeId)
                                 .setDeclineStakingReward(newDeclineStakingRewards)
                                 .freezeWith(&getTestClient())
                                 .sign(initialPrivateKey)
                                 .sign(newPrivateKey)
                                 .execute(getTestClient()));

  // Then
  AccountInfo accountInfo;
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = txResponse.getReceipt(getTestClient()));
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_FALSE(accountInfo.mIsDeleted);
  EXPECT_EQ(accountInfo.mKey->toBytes(), newPrivateKey->getPublicKey()->toBytes());
  EXPECT_EQ(accountInfo.mReceiverSignatureRequired, newReceiverSignatureRequired);
  // Can't do direct time comparison due to system clock precision limitations
  EXPECT_LE(accountInfo.mExpirationTime, newExpirationTime);
  EXPECT_EQ(accountInfo.mAutoRenewPeriod, newAutoRenewPeriod);
  EXPECT_EQ(accountInfo.mMemo, newAccountMemo);
  EXPECT_EQ(accountInfo.mMaxAutomaticTokenAssociations, newMaxAutomaticTokenAssociations);
  ASSERT_TRUE(accountInfo.mStakingInfo.mStakedNodeId.has_value());
  EXPECT_EQ(accountInfo.mStakingInfo.mStakedNodeId, newStakedNodeId);
  EXPECT_EQ(accountInfo.mStakingInfo.mDeclineRewards, newDeclineStakingRewards);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(newPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotUpdateAccountWithoutSignature)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(
    const TransactionReceipt txReceipt =
      AccountUpdateTransaction().setAccountId(accountId).execute(getTestClient()).getReceipt(getTestClient()),
    ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotUpdateAccountWithoutAccountId)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               PrecheckStatusException); // ACCOUNT_ID_DOES_NOT_EXIST

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, ExecuteWithOnlyAccountId)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, InvalidAutoRenewPeriod)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::chrono::system_clock::duration invalidAutoRenewPeriod = std::chrono::seconds(777600000);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .setAccountId(accountId)
                                                      .setAutoRenewPeriod(invalidAutoRenewPeriod)
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // AUTORENEW_DURATION_NOT_IN_RANGE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests,
       CannotUpdateMaxAutomaticTokenAssociationsToLowerThanCurrentlyAssociated)
{
  // Given
  const int64_t amount = 10LL;

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(accountKey)
                                .setMaxAutomaticTokenAssociations(1)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -amount)
                    .addTokenTransfer(tokenId, accountId, amount)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When / Then
  ASSERT_THROW(AccountUpdateTransaction()
                 .setAccountId(accountId)
                 .setMaxAutomaticTokenAssociations(0)
                 .freezeWith(&getTestClient())
                 .sign(accountKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // EXISTING_AUTOMATIC_ASSOCIATIONS_EXCEED_GIVEN_LIMIT
}
