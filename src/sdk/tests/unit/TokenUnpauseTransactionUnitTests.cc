// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenUnpauseTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenUnpauseTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const TokenId mTestTokenId = TokenId(1ULL);
};

//-----
TEST_F(TokenUnpauseTransactionUnitTests, ConstructTokenUnpauseTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenUnpauseTransactionBody>();
  body->set_allocated_token(getTestTokenId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_token_unpause(body.release());

  // When
  const TokenUnpauseTransaction tokenUnpauseTransaction(txBody);

  // Then
  EXPECT_EQ(tokenUnpauseTransaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenUnpauseTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenUnpauseTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenUnpauseTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenUnpauseTransaction transaction = TokenUnpauseTransaction()
                                          .setNodeAccountIds({ AccountId(1ULL) })
                                          .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}
