// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ScheduleDeleteTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class ScheduleDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ScheduleId& getTestScheduleId() const { return mTestScheduleId; }

private:
  const ScheduleId mTestScheduleId = ScheduleId(1ULL);
};

//-----
TEST_F(ScheduleDeleteTransactionUnitTests, ConstructScheduleDeleteTransaction)
{
  // Given / When
  ScheduleDeleteTransaction transaction;

  // Then
  EXPECT_FALSE(transaction.getScheduleId().has_value());
}

//-----
TEST_F(ScheduleDeleteTransactionUnitTests, ConstructScheduleDeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ScheduleDeleteTransactionBody>();
  body->set_allocated_scheduleid(getTestScheduleId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_scheduledelete(body.release());

  // When
  const ScheduleDeleteTransaction scheduleDeleteTransaction(txBody);

  // Then
  EXPECT_EQ(scheduleDeleteTransaction.getScheduleId(), getTestScheduleId());
}

//-----
TEST_F(ScheduleDeleteTransactionUnitTests, GetSetScheduleId)
{
  // Given
  ScheduleDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setScheduleId(getTestScheduleId()));

  // Then
  EXPECT_EQ(transaction.getScheduleId(), getTestScheduleId());
}

//-----
TEST_F(ScheduleDeleteTransactionUnitTests, GetSetScheduleIdFrozen)
{
  // Given
  ScheduleDeleteTransaction transaction = ScheduleDeleteTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setScheduleId(getTestScheduleId()), IllegalStateException);
}
