// SPDX-License-Identifier: Apache-2.0
#include "TransactionResponse.h"

#include <gtest/gtest.h>
#include <transaction_response.pb.h>

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

  // When
  const TransactionResponse transactionResponse(accountId, transactionId, hash);

  // Then
  EXPECT_EQ(transactionResponse.mNodeId, accountId);
  EXPECT_EQ(transactionResponse.mTransactionHash, hash);
  EXPECT_EQ(transactionResponse.mTransactionId, transactionId);
}
