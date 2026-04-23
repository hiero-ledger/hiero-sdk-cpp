// SPDX-License-Identifier: Apache-2.0
#include "TransactionId.h"
#include "TransactionRecordQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TransactionRecordQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const TransactionId& getTestTransactionId() const { return mTestTransactionId; }

private:
  const AccountId mTestAccountId = AccountId(10ULL);
  const TransactionId mTestTransactionId = TransactionId::generate(mTestAccountId);
};

//-----
TEST_F(TransactionRecordQueryUnitTests, SetTransactionId)
{
  // Given
  TransactionRecordQuery query;
  const TransactionId transactionId = TransactionId::generate(getTestAccountId());

  // When
  query.setTransactionId(transactionId);

  // Then
  EXPECT_EQ(query.getTransactionId(), transactionId);
}