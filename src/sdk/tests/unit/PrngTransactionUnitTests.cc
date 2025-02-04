// SPDX-License-Identifier: Apache-2.0
#include "PrngTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class PrngTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline int getTestRange() const { return mTestRange; }

private:
  const int mTestRange = 1;
};

//-----
TEST_F(PrngTransactionUnitTests, ConstructPrngTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::UtilPrngTransactionBody>();
  body->set_range(getTestRange());

  proto::TransactionBody txBody;
  txBody.set_allocated_util_prng(body.release());

  // When
  const PrngTransaction prngTransaction(txBody);

  // Then
  EXPECT_EQ(prngTransaction.getRange(), getTestRange());
}

//-----
TEST_F(PrngTransactionUnitTests, GetSetRange)
{
  // Given
  PrngTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setRange(getTestRange()));

  // Then
  EXPECT_EQ(transaction.getRange(), getTestRange());
}

//-----
TEST_F(PrngTransactionUnitTests, GetSetRangeFrozen)
{
  // Given
  PrngTransaction transaction =
    PrngTransaction().setNodeAccountIds({ AccountId(1ULL) }).setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setRange(getTestRange()), IllegalStateException);
}
