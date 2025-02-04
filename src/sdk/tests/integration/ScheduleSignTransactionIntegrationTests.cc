// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleId.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "ScheduleSignTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class ScheduleSignTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(ScheduleSignTransactionIntegrationTests, ExecuteScheduleSignTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  std::shared_ptr<PrivateKey> newKey1;
  std::shared_ptr<PrivateKey> newKey2;
  std::shared_ptr<PrivateKey> newKey3;

  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  ASSERT_NO_THROW(newKey1 = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(newKey2 = ED25519PrivateKey::generatePrivateKey());
  ASSERT_NO_THROW(newKey3 = ED25519PrivateKey::generatePrivateKey());

  const auto keyList = std::make_shared<KeyList>(KeyList::of({ newKey1, newKey2, newKey3 }));

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(keyList)
                                .setInitialBalance(Hbar(10LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  ScheduleId scheduleId;
  ASSERT_NO_THROW(scheduleId = TransferTransaction()
                                 .addHbarTransfer(accountId, Hbar(1LL).negated())
                                 .addHbarTransfer(AccountId(2ULL), Hbar(1LL))
                                 .schedule()
                                 .execute(getTestClient())
                                 .getReceipt(getTestClient())
                                 .mScheduleId.value());

  // When
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = ScheduleSignTransaction()
                                                         .setScheduleId(scheduleId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(newKey1)
                                                         .sign(newKey2)
                                                         .sign(newKey3)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Then
  ScheduleInfo scheduleInfo;
  ASSERT_NO_THROW(scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(getTestClient()));
  EXPECT_TRUE(scheduleInfo.mExecutionTime.has_value());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountDeleteTransaction()
                                                         .setDeleteAccountId(accountId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .freezeWith(&getTestClient())
                                                         .sign(newKey1)
                                                         .sign(newKey2)
                                                         .sign(newKey3)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
}
