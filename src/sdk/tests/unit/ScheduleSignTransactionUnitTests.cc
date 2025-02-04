// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ScheduleSignTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class ScheduleSignTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ScheduleId& getTestScheduleId() const { return mTestScheduleId; }

private:
  const ScheduleId mTestScheduleId = ScheduleId(1ULL);
};

//-----
TEST_F(ScheduleSignTransactionUnitTests, ConstructScheduleSignTransaction)
{
  // Given / When
  ScheduleSignTransaction transaction;

  // Then
  EXPECT_FALSE(transaction.getScheduleId().has_value());
}

//-----
TEST_F(ScheduleSignTransactionUnitTests, ConstructScheduleSignTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ScheduleSignTransactionBody>();
  body->set_allocated_scheduleid(getTestScheduleId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_schedulesign(body.release());

  // When
  const ScheduleSignTransaction scheduleSignTransaction(txBody);

  // Then
  EXPECT_EQ(scheduleSignTransaction.getScheduleId(), getTestScheduleId());
}

//-----
TEST_F(ScheduleSignTransactionUnitTests, GetSetScheduleId)
{
  // Given
  ScheduleSignTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setScheduleId(getTestScheduleId()));

  // Then
  EXPECT_EQ(transaction.getScheduleId(), getTestScheduleId());
}

//-----
TEST_F(ScheduleSignTransactionUnitTests, GetSetScheduleIdFrozen)
{
  // Given
  ScheduleSignTransaction transaction = ScheduleSignTransaction()
                                          .setNodeAccountIds({ AccountId(1ULL) })
                                          .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setScheduleId(getTestScheduleId()), IllegalStateException);
}

//-----
TEST_F(ScheduleSignTransactionUnitTests, ClearScheduleId)
{
  // Given
  ScheduleSignTransaction transaction;
  ASSERT_NO_THROW(transaction.setScheduleId(getTestScheduleId()));

  // When
  EXPECT_NO_THROW(transaction.clearScheduleId());

  // Then
  EXPECT_FALSE(transaction.getScheduleId().has_value());
}

//-----
TEST_F(ScheduleSignTransactionUnitTests, ClearScheduleIdFrozen)
{
  // Given
  ScheduleSignTransaction transaction = ScheduleSignTransaction()
                                          .setNodeAccountIds({ AccountId(1ULL) })
                                          .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.clearScheduleId(), IllegalStateException);
}
