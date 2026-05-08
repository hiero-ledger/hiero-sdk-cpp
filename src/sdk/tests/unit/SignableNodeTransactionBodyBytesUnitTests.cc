// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "BaseUnitTest.h"
#include "SignableNodeTransactionBodyBytes.h"
#include "Transaction.h"
#include "TransactionId.h"
#include "TransferTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class SignableNodeTransactionBodyBytesUnitTests : public BaseUnitTest
{
};

//-----
TEST_F(SignableNodeTransactionBodyBytesUnitTests, UnfrozenTransactionThrows)
{
  // Given
  TransferTransaction tx;

  // When / Then
  EXPECT_THROW(tx.getSignableNodeBodyBytesList(), IllegalStateException);
}

//-----
TEST_F(SignableNodeTransactionBodyBytesUnitTests, FrozenTransactionReturnsSizeEqualToNodeIds)
{
  // Given
  const AccountId nodeAccountId1 = AccountId(0ULL, 0ULL, 3ULL);
  const AccountId nodeAccountId2 = AccountId(0ULL, 0ULL, 4ULL);
  const AccountId nodeAccountId3 = AccountId(0ULL, 0ULL, 5ULL);

  TransferTransaction tx;
  tx.setTransactionId(getTestTransactionIdMock());
  tx.setNodeAccountIds({ nodeAccountId1, nodeAccountId2, nodeAccountId3 });
  tx.freeze();

  // When
  const auto signableList = tx.getSignableNodeBodyBytesList();

  // Then
  EXPECT_EQ(signableList.size(), 3ULL);
}

//-----
TEST_F(SignableNodeTransactionBodyBytesUnitTests, BodyBytesMatchNodeAccountIds)
{
  // Given
  const AccountId nodeAccountId1 = AccountId(0ULL, 0ULL, 3ULL);
  const AccountId nodeAccountId2 = AccountId(0ULL, 0ULL, 4ULL);

  TransferTransaction tx;
  tx.setTransactionId(getTestTransactionIdMock());
  tx.setNodeAccountIds({ nodeAccountId1, nodeAccountId2 });
  tx.freeze();

  // When
  const auto signableList = tx.getSignableNodeBodyBytesList();

  // Then
  ASSERT_EQ(signableList.size(), 2ULL);
  EXPECT_EQ(signableList[0].mNodeAccountId, nodeAccountId1);
  EXPECT_EQ(signableList[1].mNodeAccountId, nodeAccountId2);
}

//-----
TEST_F(SignableNodeTransactionBodyBytesUnitTests, TransactionIdsAreConsistent)
{
  // Given
  const AccountId nodeAccountId = AccountId(0ULL, 0ULL, 3ULL);
  const TransactionId txId = getTestTransactionIdMock();

  TransferTransaction tx;
  tx.setTransactionId(txId);
  tx.setNodeAccountIds({ nodeAccountId });
  tx.freeze();

  // When
  const auto signableList = tx.getSignableNodeBodyBytesList();

  // Then
  ASSERT_EQ(signableList.size(), 1ULL);
  EXPECT_EQ(signableList[0].mTransactionId, txId);
}

//-----
TEST_F(SignableNodeTransactionBodyBytesUnitTests, BodyBytesAreNotEmpty)
{
  // Given
  const AccountId nodeAccountId1 = AccountId(0ULL, 0ULL, 3ULL);
  const AccountId nodeAccountId2 = AccountId(0ULL, 0ULL, 4ULL);

  TransferTransaction tx;
  tx.setTransactionId(getTestTransactionIdMock());
  tx.setNodeAccountIds({ nodeAccountId1, nodeAccountId2 });
  tx.freeze();

  // When
  const auto signableList = tx.getSignableNodeBodyBytesList();

  // Then
  ASSERT_EQ(signableList.size(), 2ULL);
  EXPECT_FALSE(signableList[0].mSignableTransactionBodyBytes.empty());
  EXPECT_FALSE(signableList[1].mSignableTransactionBodyBytes.empty());
}

//-----
TEST_F(SignableNodeTransactionBodyBytesUnitTests, SingleNodeReturnsOneEntry)
{
  // Given
  const AccountId nodeAccountId = AccountId(0ULL, 0ULL, 3ULL);

  TransferTransaction tx;
  tx.setTransactionId(getTestTransactionIdMock());
  tx.setNodeAccountIds({ nodeAccountId });
  tx.freeze();

  // When
  const auto signableList = tx.getSignableNodeBodyBytesList();

  // Then
  ASSERT_EQ(signableList.size(), 1ULL);
  EXPECT_EQ(signableList[0].mNodeAccountId, nodeAccountId);
}

//-----
TEST_F(SignableNodeTransactionBodyBytesUnitTests, DifferentTransactionTypesWork)
{
  // Given - AccountCreateTransaction (a different transaction type)
  const AccountId nodeAccountId = AccountId(0ULL, 0ULL, 3ULL);

  AccountCreateTransaction tx;
  tx.setTransactionId(getTestTransactionIdMock());
  tx.setNodeAccountIds({ nodeAccountId });
  tx.freeze();

  // When
  const auto signableList = tx.getSignableNodeBodyBytesList();

  // Then
  ASSERT_EQ(signableList.size(), 1ULL);
  EXPECT_EQ(signableList[0].mNodeAccountId, nodeAccountId);
  EXPECT_FALSE(signableList[0].mSignableTransactionBodyBytes.empty());
}
