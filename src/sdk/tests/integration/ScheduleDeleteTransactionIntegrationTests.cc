// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleDeleteTransaction.h"
#include "ScheduleId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class ScheduleDeleteTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(ScheduleDeleteTransactionIntegrationTests, ExecuteScheduleDeleteTransaction)
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

  ScheduleId scheduleId;
  ASSERT_NO_THROW(scheduleId = ScheduleCreateTransaction()
                                 .setScheduledTransaction(scheduledTransaction)
                                 .setAdminKey(operatorKey)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mScheduleId.value());

  // When / Then
  EXPECT_NO_THROW(
    const TransactionReceipt txReceipt =
      ScheduleDeleteTransaction().setScheduleId(scheduleId).execute(getTestClient()).getReceipt(getTestClient()));

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
TEST_F(ScheduleDeleteTransactionIntegrationTests, CannotDeleteImmutableSchedule)
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

  ScheduleId scheduleId;
  ASSERT_NO_THROW(scheduleId = ScheduleCreateTransaction()
                                 .setScheduledTransaction(scheduledTransaction)
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mScheduleId.value());

  // When / Then
  EXPECT_THROW(
    const TransactionReceipt txReceipt =
      ScheduleDeleteTransaction().setScheduleId(scheduleId).execute(getTestClient()).getReceipt(getTestClient()),
    ReceiptStatusException); // SCHEDULE_IS_IMMUTABLE
}
