// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenUnfreezeTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenUnfreezeTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const AccountId mTestAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const TokenId mTestTokenId = TokenId(4ULL, 5ULL, 6ULL);
};

//-----
TEST_F(TokenUnfreezeTransactionUnitTests, ConstructTokenUnfreezeTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenUnfreezeAccountTransactionBody>();
  body->set_allocated_account(getTestAccountId().toProtobuf().release());
  body->set_allocated_token(getTestTokenId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_tokenunfreeze(body.release());

  // When
  const TokenUnfreezeTransaction tokenUnfreezeTransaction(txBody);

  // Then
  EXPECT_EQ(tokenUnfreezeTransaction.getAccountId(), getTestAccountId());
  EXPECT_EQ(tokenUnfreezeTransaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenUnfreezeTransactionUnitTests, GetSetAccountId)
{
  // Given
  TokenUnfreezeTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAccountId(getTestAccountId()));

  // Then
  EXPECT_EQ(transaction.getAccountId(), getTestAccountId());
}

//-----
TEST_F(TokenUnfreezeTransactionUnitTests, GetSetAccountIdFrozen)
{
  // Given
  TokenUnfreezeTransaction transaction = TokenUnfreezeTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAccountId(getTestAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenUnfreezeTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenUnfreezeTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenUnfreezeTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenUnfreezeTransaction transaction = TokenUnfreezeTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}