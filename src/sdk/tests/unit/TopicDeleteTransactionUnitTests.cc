// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TopicDeleteTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TopicDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TopicId& getTestTopicId() const { return mTestTopicId; }

private:
  const TopicId mTestTopicId = TopicId(1ULL);
};

//-----
TEST_F(TopicDeleteTransactionUnitTests, ConstructTopicDeleteTransaction)
{
  // Given / When
  TopicDeleteTransaction transaction;

  // Then
  EXPECT_FALSE(transaction.getTopicId().has_value());
}

//-----
TEST_F(TopicDeleteTransactionUnitTests, ConstructTopicDeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ConsensusDeleteTopicTransactionBody>();
  body->set_allocated_topicid(getTestTopicId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_consensusdeletetopic(body.release());

  // When
  const TopicDeleteTransaction topicDeleteTransaction(txBody);

  // Then
  EXPECT_EQ(topicDeleteTransaction.getTopicId(), getTestTopicId());
}

//-----
TEST_F(TopicDeleteTransactionUnitTests, GetSetTopicId)
{
  // Given
  TopicDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTopicId(getTestTopicId()));

  // Then
  EXPECT_EQ(transaction.getTopicId(), getTestTopicId());
}

//-----
TEST_F(TopicDeleteTransactionUnitTests, GetSetTopicIdFrozen)
{
  // Given
  TopicDeleteTransaction transaction = TopicDeleteTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTopicId(getTestTopicId()), IllegalStateException);
}
