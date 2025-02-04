// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenGrantKycTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenGrantKycTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const AccountId mTestAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const TokenId mTestTokenId = TokenId(4ULL, 5ULL, 6ULL);
};

//-----
TEST_F(TokenGrantKycTransactionUnitTests, ConstructTokenGrantKycTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenGrantKycTransactionBody>();
  body->set_allocated_account(getTestAccountId().toProtobuf().release());
  body->set_allocated_token(getTestTokenId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_tokengrantkyc(body.release());

  // When
  const TokenGrantKycTransaction tokenGrantKycTransaction(txBody);

  // Then
  EXPECT_EQ(tokenGrantKycTransaction.getAccountId(), getTestAccountId());
  EXPECT_EQ(tokenGrantKycTransaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenGrantKycTransactionUnitTests, GetSetAccountId)
{
  // Given
  TokenGrantKycTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAccountId(getTestAccountId()));

  // Then
  EXPECT_EQ(transaction.getAccountId(), getTestAccountId());
}

//-----
TEST_F(TokenGrantKycTransactionUnitTests, GetSetAccountIdFrozen)
{
  // Given
  TokenGrantKycTransaction transaction = TokenGrantKycTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAccountId(getTestAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenGrantKycTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenGrantKycTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenGrantKycTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenGrantKycTransaction transaction = TokenGrantKycTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}
