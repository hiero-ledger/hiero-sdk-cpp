// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenDeleteTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const TokenId mTestTokenId = TokenId(1ULL);
};

//-----
TEST_F(TokenDeleteTransactionUnitTests, ConstructTokenDeleteTransaction)
{
  // Given / When
  TokenDeleteTransaction transaction;

  // Then
  EXPECT_FALSE(transaction.getTokenId().has_value());
}

//-----
TEST_F(TokenDeleteTransactionUnitTests, ConstructTokenDeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenDeleteTransactionBody>();
  body->set_allocated_token(getTestTokenId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_tokendeletion(body.release());

  // When
  const TokenDeleteTransaction tokenDeleteTransaction(txBody);

  // Then
  EXPECT_EQ(tokenDeleteTransaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenDeleteTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenDeleteTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenDeleteTransaction transaction = TokenDeleteTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}