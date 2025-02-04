// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TopicMessageSubmitTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TopicMessageSubmitTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TopicId& getTestTopicId() const { return mTestTopicId; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestMessage() const { return mTestMessage; }

private:
  const TopicId mTestTopicId = TopicId(1ULL, 2ULL, 3ULL);
  const std::vector<std::byte> mTestMessage = { std::byte(0x04), std::byte(0x05), std::byte(0x06) };
};

//-----
TEST_F(TopicMessageSubmitTransactionUnitTests, ConstructTopicMessageSubmitTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ConsensusSubmitMessageTransactionBody>();
  body->set_allocated_topicid(getTestTopicId().toProtobuf().release());
  body->set_message(internal::Utilities::byteVectorToString(getTestMessage()));

  proto::TransactionBody txBody;
  txBody.set_allocated_consensussubmitmessage(body.release());

  // When
  const TopicMessageSubmitTransaction topicMessageSubmitTransaction(txBody);

  // Then
  EXPECT_EQ(topicMessageSubmitTransaction.getTopicId(), getTestTopicId());
}

//-----
TEST_F(TopicMessageSubmitTransactionUnitTests, GetSetTopicId)
{
  // Given
  TopicMessageSubmitTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTopicId(getTestTopicId()));

  // Then
  EXPECT_EQ(transaction.getTopicId(), getTestTopicId());
}

//-----
TEST_F(TopicMessageSubmitTransactionUnitTests, GetSetTopicIdFrozen)
{
  // Given
  TopicMessageSubmitTransaction transaction = TopicMessageSubmitTransaction()
                                                .setNodeAccountIds({ AccountId(1ULL) })
                                                .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTopicId(getTestTopicId()), IllegalStateException);
}

//-----
TEST_F(TopicMessageSubmitTransactionUnitTests, GetSetMessage)
{
  // Given
  TopicMessageSubmitTransaction transactionWithBytes;
  TopicMessageSubmitTransaction transactionWithStr;

  // When
  EXPECT_NO_THROW(transactionWithBytes.setMessage(getTestMessage()));
  EXPECT_NO_THROW(transactionWithStr.setMessage(internal::Utilities::byteVectorToString(getTestMessage())));

  // Then
  EXPECT_EQ(transactionWithBytes.getMessage(), getTestMessage());
  EXPECT_EQ(transactionWithStr.getMessage(), getTestMessage());
}

//-----
TEST_F(TopicMessageSubmitTransactionUnitTests, GetSetMessageFrozen)
{
  // Given
  TopicMessageSubmitTransaction transactionWithBytes = TopicMessageSubmitTransaction()
                                                         .setNodeAccountIds({ AccountId(1ULL) })
                                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  TopicMessageSubmitTransaction transactionWithStr = TopicMessageSubmitTransaction()
                                                       .setNodeAccountIds({ AccountId(1ULL) })
                                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transactionWithBytes.freeze());
  ASSERT_NO_THROW(transactionWithStr.freeze());

  // When / Then
  EXPECT_THROW(transactionWithBytes.setMessage(getTestMessage()), IllegalStateException);
  EXPECT_THROW(transactionWithStr.setMessage(internal::Utilities::byteVectorToString(getTestMessage())),
               IllegalStateException);
}
