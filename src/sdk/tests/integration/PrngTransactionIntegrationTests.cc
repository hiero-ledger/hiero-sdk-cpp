// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "PrngTransaction.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"

#include <gtest/gtest.h>

using namespace Hiero;

class PrngTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(PrngTransactionIntegrationTests, ExecutePrngTransactionNoRange)
{
  // Given / When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = PrngTransaction().execute(getTestClient()));

  // Then
  TransactionRecord txRecord;
  ASSERT_NO_THROW(txRecord = txResponse.getRecord(getTestClient()));

  EXPECT_FALSE(txRecord.mPrngBytes.empty());
  EXPECT_FALSE(txRecord.mPrngNumber.has_value());
}

//-----
TEST_F(PrngTransactionIntegrationTests, ExecutePrngTransactionRange)
{
  // Given
  const int range = 100;

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = PrngTransaction().setRange(range).execute(getTestClient()));

  // Then
  TransactionRecord txRecord;
  ASSERT_NO_THROW(txRecord = txResponse.getRecord(getTestClient()));

  EXPECT_TRUE(txRecord.mPrngBytes.empty());
  EXPECT_TRUE(txRecord.mPrngNumber.has_value());
  EXPECT_GE(txRecord.mPrngNumber.value(), 0);
  EXPECT_LE(txRecord.mPrngNumber.value(), range);
}
