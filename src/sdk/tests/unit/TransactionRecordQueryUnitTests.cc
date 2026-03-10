// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
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

TEST_F(TransactionRecordQueryUnitTests, SetTransactionId)
{
  TransactionRecordQuery query;
  const TransactionId transactionId = TransactionId::generate(getTestAccountId());
  query.setTransactionId(transactionId);
  EXPECT_EQ(query.getTransactionId(), transactionId);
}

//-----
TEST_F(TransactionRecordQueryUnitTests, SetSubmittingNodeIdPinsToThatNode)
{
  // Given
  const AccountId submittingNode(3ULL);
  const AccountId operatorId(1ULL);
  const TransactionId transactionId = TransactionId::withValidStart(operatorId, std::chrono::system_clock::now());

  // When
  TransactionRecordQuery query;
  query.setTransactionId(transactionId);
  query.setSubmittingNodeId(submittingNode);

  // Then - before onExecute()
  query.setNodeAccountIds({ submittingNode });
  ASSERT_EQ(query.getNodeAccountIds().size(), 1U);
  EXPECT_EQ(query.getNodeAccountIds().at(0), submittingNode);
}
