// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNodeDeleteTransaction.h"

#include <gtest/gtest.h>
#include <services/transaction.pb.h>

using namespace Hiero;

class RegisteredNodeDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  RegisteredNodeDeleteTransaction transaction;
};

//-----
TEST_F(RegisteredNodeDeleteTransactionUnitTests, ConstructRegisteredNodeDeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  proto::TransactionBody transactionBody;
  auto* body = transactionBody.mutable_registerednodedelete();
  body->set_registered_node_id(42ULL);

  // When
  RegisteredNodeDeleteTransaction tx(transactionBody);

  // Then
  EXPECT_EQ(tx.getRegisteredNodeId(), 42ULL);
}

//-----
TEST_F(RegisteredNodeDeleteTransactionUnitTests, SetAndGetRegisteredNodeId)
{
  // Given / When
  transaction.setRegisteredNodeId(99ULL);

  // Then
  EXPECT_EQ(transaction.getRegisteredNodeId(), 99ULL);
}

//-----
TEST_F(RegisteredNodeDeleteTransactionUnitTests, DefaultRegisteredNodeIdIsZero)
{
  EXPECT_EQ(transaction.getRegisteredNodeId(), 0ULL);
}

//-----
TEST_F(RegisteredNodeDeleteTransactionUnitTests, SerializeDeserializeRoundTrip)
{
  // Given / When: manually build proto body and reconstruct
  proto::TransactionBody txBody;
  txBody.mutable_registerednodedelete()->set_registered_node_id(7ULL);

  RegisteredNodeDeleteTransaction tx2(txBody);

  // Then
  EXPECT_EQ(tx2.getRegisteredNodeId(), 7ULL);
}
