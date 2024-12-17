// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleDeleteTransaction.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class ScheduleInfoQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(ScheduleInfoQueryIntegrationTests, ExecuteScheduleInfoQuery)
{
  // Given
  const std::string memo = "test memo";

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

  ScheduleId scheduleId;
  ASSERT_NO_THROW(scheduleId =
                    ScheduleCreateTransaction()
                      .setScheduledTransaction(WrappedTransaction(TransferTransaction()
                                                                    .addHbarTransfer(accountId, Hbar(-5LL))
                                                                    .addHbarTransfer(AccountId(2ULL), Hbar(5LL))
                                                                    .freezeWith(&getTestClient())))
                      .setScheduleMemo(memo)
                      .setAdminKey(operatorKey)
                      .setPayerAccountId(AccountId(2ULL))
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mScheduleId.value());

  // When
  ScheduleInfo scheduleInfo;
  EXPECT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));

  // Then
  EXPECT_EQ(scheduleInfo.mScheduleId, scheduleId);
  EXPECT_FALSE(scheduleInfo.mDeletionTime.has_value());
  EXPECT_FALSE(scheduleInfo.mExecutionTime.has_value());
  EXPECT_GT(scheduleInfo.mExpirationTime.time_since_epoch().count(),
            std::chrono::system_clock::now().time_since_epoch().count());
  EXPECT_NE(scheduleInfo.mScheduledTransaction.getTransaction<TransferTransaction>(), nullptr);
  EXPECT_EQ(scheduleInfo.mMemo, memo);
  EXPECT_EQ(scheduleInfo.mAdminKey->toBytes(), operatorKey->getPublicKey()->toBytes());
  EXPECT_EQ(scheduleInfo.mSignatories.size(), 1);
  EXPECT_EQ(scheduleInfo.mCreatorAccountId, AccountId(2ULL));
  EXPECT_EQ(scheduleInfo.mPayerAccountId, AccountId(2ULL));

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountDeleteTransaction()
                                                         .setDeleteAccountId(accountId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .freezeWith(&getTestClient())
                                                         .sign(newKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
  ASSERT_NO_THROW(
    const TransactionReceipt txReceipt =
      ScheduleDeleteTransaction().setScheduleId(scheduleId).execute(getTestClient()).getReceipt(getTestClient()));
}
