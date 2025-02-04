// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "Defaults.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TopicDeleteTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TopicDeleteTransactionIntegrationTests, ExecuteTopicDeleteTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt =
                    TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TopicDeleteTransactionIntegrationTests, CannotDeleteImmutableTopic)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TopicId topicId;
  ASSERT_NO_THROW(topicId =
                    TopicCreateTransaction().execute(getTestClient()).getReceipt(getTestClient()).mTopicId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()),
               ReceiptStatusException); // UNAUTHORIZED
}
