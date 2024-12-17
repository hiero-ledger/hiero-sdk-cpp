// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountUpdateTransaction.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <thread>

#include <gtest/gtest.h>

using namespace Hiero;

class ScheduleCreateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, ExecuteScheduleCreateTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(newKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  const WrappedTransaction scheduledTransaction(
    TransferTransaction().addHbarTransfer(accountId, Hbar(-5LL)).addHbarTransfer(AccountId(2ULL), Hbar(5LL)));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ScheduleCreateTransaction()
                                .setScheduledTransaction(scheduledTransaction)
                                .setAdminKey(operatorKey)
                                .setPayerAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  ASSERT_TRUE(txReceipt.mScheduleId.has_value());

  ScheduleInfo scheduleInfo;
  EXPECT_NO_THROW(scheduleInfo =
                    ScheduleInfoQuery().setScheduleId(txReceipt.mScheduleId.value()).execute(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, CanCreateScheduleCreateTransactionWithScheduleFunction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(newKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TransferTransaction scheduledTransaction;
  ASSERT_NO_THROW(
    scheduledTransaction =
      TransferTransaction().addHbarTransfer(accountId, Hbar(-5LL)).addHbarTransfer(AccountId(2ULL), Hbar(5LL)));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = scheduledTransaction.schedule()
                                .setAdminKey(operatorKey)
                                .setPayerAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  // TODO: ScheduleInfoQuery

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, CannotScheduleTwoIdenticalTransactions)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(newKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  ScheduleCreateTransaction scheduleTransaction =
    ScheduleCreateTransaction()
      .setScheduledTransaction(WrappedTransaction(TransferTransaction()
                                                    .addHbarTransfer(accountId, Hbar(-5LL))
                                                    .addHbarTransfer(AccountId(2ULL), Hbar(5LL))
                                                    .freezeWith(&getTestClient())))
      .setAdminKey(operatorKey)
      .setPayerAccountId(AccountId(2ULL));

  ScheduleCreateTransaction firstScheduleTransaction = scheduleTransaction;
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    firstScheduleTransaction.execute(getTestClient()).getReceipt(getTestClient()));

  // When / Then
  ScheduleCreateTransaction secondScheduleTransaction = scheduleTransaction;
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 secondScheduleTransaction.execute(getTestClient()).getReceipt(getTestClient()),
               ReceiptStatusException); // IDENTICAL_SCHEDULE_ALREADY_CREATED

  // Then
  // TODO: ScheduleInfoQuery

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountDeleteTransaction()
                                                         .setDeleteAccountId(accountId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .freezeWith(&getTestClient())
                                                         .sign(newKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
}

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, CanSignScheduleAndWaitForExpiry)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(newKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // Create the transaction
  const WrappedTransaction scheduledTransaction(
    TransferTransaction().addHbarTransfer(accountId, Hbar(-5LL)).addHbarTransfer(AccountId(2ULL), Hbar(5LL)));

  // When
  // Schedule the transaction
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ScheduleCreateTransaction()
                                .setScheduledTransaction(scheduledTransaction)
                                .setWaitForExpiry(true)
                                .setExpirationTime(std::chrono::system_clock::now() + std::chrono::hours(24))
                                .setAdminKey(operatorKey)
                                .setPayerAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  ASSERT_TRUE(txReceipt.mScheduleId.has_value());
  const ScheduleId scheduleId = txReceipt.mScheduleId.value();

  // Check if transaction is scheduled
  ScheduleInfo scheduleInfo;
  EXPECT_NO_THROW(scheduleInfo =
                    ScheduleInfoQuery().setScheduleId(txReceipt.mScheduleId.value()).execute(getTestClient()));

  // Sign with the key
  EXPECT_NO_THROW(txReceipt = ScheduleSignTransaction()
                                .setScheduleId(scheduleId)
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, CannotScheduleOneYearIntoTheFuture)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(newKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // Create the transaction
  const WrappedTransaction scheduledTransaction(
    TransferTransaction().addHbarTransfer(accountId, Hbar(-5LL)).addHbarTransfer(AccountId(2ULL), Hbar(5LL)));

  // When / Then
  // Schedule the transaction
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = ScheduleCreateTransaction()
                             .setScheduledTransaction(scheduledTransaction)
                             .setExpirationTime(std::chrono::system_clock::now() + std::chrono::hours(8760))
                             .setAdminKey(operatorKey)
                             .setPayerAccountId(AccountId(2ULL))
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, CannotScheduleInThePast)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(newKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // Create the transaction
  const WrappedTransaction scheduledTransaction(
    TransferTransaction().addHbarTransfer(accountId, Hbar(-5LL)).addHbarTransfer(AccountId(2ULL), Hbar(5LL)));

  // When / Then
  // Schedule the transaction
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = ScheduleCreateTransaction()
                             .setScheduledTransaction(scheduledTransaction)
                             .setExpirationTime(std::chrono::system_clock::now() - std::chrono::hours(24))
                             .setAdminKey(operatorKey)
                             .setPayerAccountId(AccountId(2ULL))
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException);

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, CanSignWithMultisigAndUpdateSigningRequirements)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // Generate three ED25519 private keys.
  const std::vector<std::shared_ptr<PrivateKey>> privateKeys = { ED25519PrivateKey::generatePrivateKey(),
                                                                 ED25519PrivateKey::generatePrivateKey(),
                                                                 ED25519PrivateKey::generatePrivateKey() };

  // Create a Keylist from the list of generated private keys that require 2 of the 3 keys to sign.
  auto keys = std::make_shared<KeyList>(KeyList::of({ privateKeys.at(0), privateKeys.at(1), privateKeys.at(2) }));
  keys->setThreshold(2);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(keys)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // Create the transaction
  const WrappedTransaction scheduledTransaction(
    TransferTransaction().addHbarTransfer(accountId, Hbar(-5LL)).addHbarTransfer(AccountId(2ULL), Hbar(5LL)));

  // When
  // Schedule the transaction
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ScheduleCreateTransaction()
                                .setScheduledTransaction(scheduledTransaction)
                                .setExpirationTime(std::chrono::system_clock::now() + std::chrono::hours(1))
                                .setAdminKey(operatorKey)
                                .setPayerAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  ASSERT_TRUE(txReceipt.mScheduleId.has_value());
  const ScheduleId scheduleId = txReceipt.mScheduleId.value();

  // Verify the transaction is not executed
  ScheduleInfo scheduleInfo;
  EXPECT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));

  ASSERT_EQ(scheduleInfo.mExecutionTime.has_value(), false);

  // Sign with one key
  EXPECT_NO_THROW(txReceipt = ScheduleSignTransaction()
                                .setScheduleId(scheduleId)
                                .freezeWith(&getTestClient())
                                .sign(privateKeys[0])
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify the transaction is still not executed
  EXPECT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));

  ASSERT_EQ(scheduleInfo.mExecutionTime.has_value(), false);

  // Update the signing requirements
  ASSERT_NO_THROW(AccountUpdateTransaction()
                    .setAccountId(accountId)
                    .setKey(privateKeys[0]->getPublicKey())
                    .freezeWith(&getTestClient())
                    .sign(privateKeys[0])
                    .sign(privateKeys[1])
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Verify the transaction is still not executed
  EXPECT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));

  ASSERT_EQ(scheduleInfo.mExecutionTime.has_value(), false);

  // Then
  // Sign with one more key
  EXPECT_NO_THROW(txReceipt = ScheduleSignTransaction()
                                .setScheduleId(scheduleId)
                                .freezeWith(&getTestClient())
                                .sign(privateKeys[1])
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify the transaction is executed
  EXPECT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));

  ASSERT_EQ(scheduleInfo.mExecutionTime.has_value(), true);
}

//-----
TEST_F(ScheduleCreateTransactionIntegrationTests, CanExecuteWithShortExpirationTime)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(newKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  const WrappedTransaction scheduledTransaction(
    TransferTransaction().addHbarTransfer(accountId, Hbar(-5LL)).addHbarTransfer(AccountId(2ULL), Hbar(5LL)));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ScheduleCreateTransaction()
                                .setScheduledTransaction(scheduledTransaction)
                                .setWaitForExpiry(true)
                                .setExpirationTime(std::chrono::system_clock::now() + std::chrono::seconds(5))
                                .setAdminKey(operatorKey)
                                .setPayerAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  ASSERT_TRUE(txReceipt.mScheduleId.has_value());
  const ScheduleId scheduleId = txReceipt.mScheduleId.value();

  ScheduleInfo scheduleInfo;
  // Verify the transaction is still not executed
  EXPECT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));

  ASSERT_EQ(scheduleInfo.mExecutionTime.has_value(), false);

  EXPECT_NO_THROW(txReceipt = ScheduleSignTransaction()
                                .setScheduleId(scheduleId)
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Verify the transaction is still not executed
  EXPECT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));

  ASSERT_EQ(scheduleInfo.mExecutionTime.has_value(), false);

  AccountBalance accountBalanceBefore;
  EXPECT_NO_THROW(accountBalanceBefore = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  // Then
  // Sleep for 5 seconds so transaction can execute
  std::this_thread::sleep_for(std::chrono::seconds(6));

  // Verify the transaction is executed
  AccountBalance accountBalanceAfter;
  EXPECT_NO_THROW(accountBalanceAfter = AccountBalanceQuery().setAccountId(accountId).execute(getTestClient()));

  ASSERT_NE(accountBalanceBefore.mBalance.toTinybars(), accountBalanceAfter.mBalance.toTinybars());

  // Clean up
  ASSERT_NO_THROW(txReceipt = AccountDeleteTransaction()
                                .setDeleteAccountId(accountId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}
