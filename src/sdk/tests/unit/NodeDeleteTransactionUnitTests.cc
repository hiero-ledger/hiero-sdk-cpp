// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "NodeDeleteTransaction.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class NodeDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  NodeDeleteTransaction transaction;
};

//-----
TEST_F(NodeDeleteTransactionUnitTests, ConstructNodeDeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  proto::TransactionBody transactionBody;
  aproto::NodeDeleteTransactionBody* body = transactionBody.mutable_nodedelete();

  body->set_node_id(1);

  // When
  NodeDeleteTransaction nodeDeleteTransaction(transactionBody);

  // Then
  ASSERT_EQ(nodeDeleteTransaction.getNodeId(), 1);
}

//-----
TEST_F(NodeDeleteTransactionUnitTests, SetAndGetNodeId)
{
  // Given
  uint64_t nodeId = 3;

  // When
  transaction.setNodeId(nodeId);

  // Then
  ASSERT_EQ(transaction.getNodeId(), nodeId);
}