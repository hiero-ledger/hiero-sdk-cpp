// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenDissociateTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenDissociateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const std::vector<TokenId>& getTestTokenIds() const { return mTestTokenIds; }

private:
  const AccountId mTestAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const std::vector<TokenId> mTestTokenIds = { TokenId(4ULL, 5ULL, 6ULL),
                                               TokenId(7ULL, 8ULL, 9ULL),
                                               TokenId(10ULL, 11ULL, 12ULL) };
};

//-----
TEST_F(TokenDissociateTransactionUnitTests, ConstructTokenDissociateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenDissociateTransactionBody>();
  body->set_allocated_account(getTestAccountId().toProtobuf().release());

  for (const auto& token : getTestTokenIds())
  {
    *body->add_tokens() = *token.toProtobuf();
  }

  proto::TransactionBody txBody;
  txBody.set_allocated_tokendissociate(body.release());

  // When
  const TokenDissociateTransaction tokenDissociateTransaction(txBody);

  // Then
  EXPECT_EQ(tokenDissociateTransaction.getAccountId(), getTestAccountId());
  EXPECT_EQ(tokenDissociateTransaction.getTokenIds().size(), getTestTokenIds().size());
}

//-----
TEST_F(TokenDissociateTransactionUnitTests, GetSetAccountId)
{
  // Given
  TokenDissociateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAccountId(getTestAccountId()));

  // Then
  EXPECT_EQ(transaction.getAccountId(), getTestAccountId());
}

//-----
TEST_F(TokenDissociateTransactionUnitTests, GetSetAccountIdFrozen)
{
  // Given
  TokenDissociateTransaction transaction = TokenDissociateTransaction()
                                             .setNodeAccountIds({ AccountId(1ULL) })
                                             .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAccountId(getTestAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenDissociateTransactionUnitTests, GetSetTokenIds)
{
  // Given
  TokenDissociateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenIds(getTestTokenIds()));

  // Then
  EXPECT_EQ(transaction.getTokenIds(), getTestTokenIds());
}

//-----
TEST_F(TokenDissociateTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenDissociateTransaction transaction = TokenDissociateTransaction()
                                             .setNodeAccountIds({ AccountId(1ULL) })
                                             .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenIds(getTestTokenIds()), IllegalStateException);
}
