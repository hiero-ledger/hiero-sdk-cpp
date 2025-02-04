// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenPauseTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenPauseTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const TokenId mTestTokenId = TokenId(1ULL);
};

//-----
TEST_F(TokenPauseTransactionUnitTests, ConstructTokenPauseTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenPauseTransactionBody>();
  body->set_allocated_token(getTestTokenId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_token_pause(body.release());

  // When
  const TokenPauseTransaction tokenPauseTransaction(txBody);

  // Then
  EXPECT_EQ(tokenPauseTransaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenPauseTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenPauseTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenPauseTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenPauseTransaction transaction = TokenPauseTransaction()
                                        .setNodeAccountIds({ AccountId(1ULL) })
                                        .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}