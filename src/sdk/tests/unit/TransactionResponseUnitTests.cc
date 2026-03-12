// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecordQuery.h"
#include "TransactionResponse.h"

#include <gtest/gtest.h>
#include <services/transaction_response.pb.h>

using namespace Hiero;

class TransactionResponseUnitTests : public ::testing::Test
{
};

//-----
TEST_F(TransactionResponseUnitTests, ContructTransactionResponse)
{
  // Given
  const AccountId accountId(1ULL);
  const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  const TransactionId transactionId = TransactionId::withValidStart(accountId, now);
  const std::vector<std::byte> hash = { std::byte(0x00), std::byte(0x01), std::byte(0x02) };
  const std::vector<AccountId> nodeAccountIds = { AccountId(3ULL), AccountId(4ULL) };

  // When
  const TransactionResponse transactionResponse(accountId, transactionId, hash, nodeAccountIds);

  // Then
  EXPECT_EQ(transactionResponse.mNodeId, accountId);
  EXPECT_EQ(transactionResponse.mTransactionHash, hash);
  EXPECT_EQ(transactionResponse.mTransactionId, transactionId);
  EXPECT_EQ(transactionResponse.mTransactionNodeAccountIds, nodeAccountIds);
}

//-----
TEST_F(TransactionResponseUnitTests, GetReceiptQueryNullClientPinsToSubmittingNode)
{
  // Given
  const AccountId submittingNode(0ULL, 0ULL, 3ULL);
  const TransactionId txId = TransactionId::withValidStart(AccountId(1ULL), std::chrono::system_clock::now());
  const TransactionResponse response(submittingNode, txId, {}, { submittingNode, AccountId(0ULL, 0ULL, 4ULL) });

  // When / Then: no client → pinned to submitting node only
  EXPECT_EQ(response.getReceiptQuery(nullptr).getNodeAccountIds(), (std::vector<AccountId>{ submittingNode }));
}

//-----
TEST_F(TransactionResponseUnitTests, GetReceiptQueryFailoverBuildsNodeListFromTransactionNodes)
{
  // Given: transaction nodes 3, 5, 4 (unsorted); submitting node is 3
  const AccountId submittingNode(0ULL, 0ULL, 3ULL);
  const AccountId node4(0ULL, 0ULL, 4ULL);
  const AccountId node5(0ULL, 0ULL, 5ULL);
  const TransactionId txId = TransactionId::withValidStart(AccountId(1ULL), std::chrono::system_clock::now());
  const TransactionResponse response(submittingNode, txId, {}, { submittingNode, node5, node4 });

  Client client;
  client.setAllowReceiptNodeFailover(true);

  // When / Then: submitting node first then sorting remaining by acc. number
  const std::vector<AccountId> expected = { submittingNode, node4, node5 };
  EXPECT_EQ(response.getReceiptQuery(&client).getNodeAccountIds(), expected);
}

//-----
TEST_F(TransactionResponseUnitTests, SingleFlagGovernsReceiptAndRecordQueryNodeList)
{
  // Given
  const AccountId submittingNode(0ULL, 0ULL, 3ULL);
  const AccountId node4(0ULL, 0ULL, 4ULL);
  const TransactionId txId = TransactionId::withValidStart(AccountId(1ULL), std::chrono::system_clock::now());
  const TransactionResponse response(submittingNode, txId, {}, { submittingNode, node4 });

  Client client;
  client.setAllowReceiptNodeFailover(true);

  // When / Then: same flag, same node list for both query types
  EXPECT_EQ(response.getReceiptQuery(&client).getNodeAccountIds(),
            response.getRecordQuery(&client).getNodeAccountIds());
}
