// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ScheduleCreateTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/TimestampConverter.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class ScheduleCreateTransactionTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mTestSchedulableTransactionBody.set_memo("test memo");
    mTestSchedulableTransactionBody.set_transactionfee(1ULL);
    mTestSchedulableTransactionBody.set_allocated_cryptoapproveallowance(
      new proto::CryptoApproveAllowanceTransactionBody);
  }

  [[nodiscard]] inline const proto::SchedulableTransactionBody& getTestSchedulableTransactionBody() const
  {
    return mTestSchedulableTransactionBody;
  }
  [[nodiscard]] inline const std::string& getTestMemo() const { return mTestMemo; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const AccountId& getTestPayerAccountId() const { return mTestPayerAccountId; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline bool getTestWaitForExpiry() const { return mTestWaitForExpiry; }

private:
  proto::SchedulableTransactionBody mTestSchedulableTransactionBody;
  const std::string mTestMemo = "my test memo";
  const std::shared_ptr<PublicKey> mTestAdminKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const AccountId mTestPayerAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const bool mTestWaitForExpiry = true;
};

//-----
TEST_F(ScheduleCreateTransactionTests, ConstructScheduleCreateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ScheduleCreateTransactionBody>();
  *body->mutable_scheduledtransactionbody() = getTestSchedulableTransactionBody();
  body->set_memo(getTestMemo());
  body->set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  body->set_allocated_payeraccountid(getTestPayerAccountId().toProtobuf().release());
  body->set_allocated_expiration_time(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->set_wait_for_expiry(getTestWaitForExpiry());

  proto::TransactionBody txBody;
  txBody.set_allocated_schedulecreate(body.release());

  // When
  const ScheduleCreateTransaction scheduleCreateTransaction(txBody);

  // Then
  EXPECT_EQ(scheduleCreateTransaction.getScheduledTransaction().getTransactionType(),
            TransactionType::ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION);
  EXPECT_EQ(scheduleCreateTransaction.getScheduleMemo(), getTestMemo());
  EXPECT_EQ(scheduleCreateTransaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(scheduleCreateTransaction.getPayerAccountId(), getTestPayerAccountId());
  EXPECT_EQ(scheduleCreateTransaction.getExpirationTime(), getTestExpirationTime());
  EXPECT_EQ(scheduleCreateTransaction.isWaitForExpiry(), getTestWaitForExpiry());
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetScheduledTransaction)
{
  // Given
  ScheduleCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setScheduledTransaction(WrappedTransaction(AccountAllowanceApproveTransaction())));

  // Then
  EXPECT_EQ(transaction.getScheduledTransaction().getTransactionType(),
            TransactionType::ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION);
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetScheduledTransactionFrozen)
{
  // Given
  ScheduleCreateTransaction transaction = ScheduleCreateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setScheduledTransaction(WrappedTransaction(AccountAllowanceApproveTransaction())),
               IllegalStateException);
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetMemo)
{
  // Given
  ScheduleCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setScheduleMemo(getTestMemo()));

  // Then
  EXPECT_EQ(transaction.getScheduleMemo(), getTestMemo());
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetMemoFrozen)
{
  // Given
  ScheduleCreateTransaction transaction = ScheduleCreateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setScheduleMemo(getTestMemo()), IllegalStateException);
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetAdminKey)
{
  // Given
  ScheduleCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAdminKey(getTestAdminKey()));

  // Then
  EXPECT_EQ(transaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetAdminKeyFrozen)
{
  // Given
  ScheduleCreateTransaction transaction = ScheduleCreateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAdminKey(getTestAdminKey()), IllegalStateException);
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetPayerAccountId)
{
  // Given
  ScheduleCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setPayerAccountId(getTestPayerAccountId()));

  // Then
  EXPECT_EQ(transaction.getPayerAccountId(), getTestPayerAccountId());
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetPayerAccountIdFrozen)
{
  // Given
  ScheduleCreateTransaction transaction = ScheduleCreateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setPayerAccountId(getTestPayerAccountId()), IllegalStateException);
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetExpirationTime)
{
  // Given
  ScheduleCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setExpirationTime(getTestExpirationTime()));

  // Then
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetExpirationTimeFrozen)
{
  // Given
  ScheduleCreateTransaction transaction = ScheduleCreateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetWaitForExpiry)
{
  // Given
  ScheduleCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setWaitForExpiry(getTestWaitForExpiry()));

  // Then
  EXPECT_EQ(transaction.isWaitForExpiry(), getTestWaitForExpiry());
}

//-----
TEST_F(ScheduleCreateTransactionTests, GetSetWaitForExpiryFrozen)
{
  // Given
  ScheduleCreateTransaction transaction = ScheduleCreateTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setWaitForExpiry(getTestWaitForExpiry()), IllegalStateException);
}
