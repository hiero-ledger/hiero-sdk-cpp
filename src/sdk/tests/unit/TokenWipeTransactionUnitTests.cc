// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenWipeTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenWipeTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const uint64_t& getTestAmount() const { return mTestAmount; }
  [[nodiscard]] inline const std::vector<uint64_t>& getTestSerialNumbers() const { return mTestSerialNumbers; }

private:
  const TokenId mTestTokenId = TokenId(1ULL, 2ULL, 3ULL);
  const AccountId mTestAccountId = AccountId(4ULL, 5ULL, 6ULL);
  const uint64_t mTestAmount = 7ULL;
  const std::vector<uint64_t> mTestSerialNumbers = { 8ULL, 9ULL, 10ULL };
};

//-----
TEST_F(TokenWipeTransactionUnitTests, ConstructTokenWipeTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenWipeAccountTransactionBody>();
  body->set_allocated_token(getTestTokenId().toProtobuf().release());
  body->set_allocated_account(getTestAccountId().toProtobuf().release());
  body->set_amount(getTestAmount());

  for (const auto& num : getTestSerialNumbers())
  {
    body->add_serialnumbers(static_cast<int64_t>(num));
  }

  proto::TransactionBody txBody;
  txBody.set_allocated_tokenwipe(body.release());

  // When
  const TokenWipeTransaction tokenWipeTransaction(txBody);

  // Then
  EXPECT_EQ(tokenWipeTransaction.getTokenId(), getTestTokenId());
  EXPECT_EQ(tokenWipeTransaction.getAccountId(), getTestAccountId());
  EXPECT_EQ(tokenWipeTransaction.getAmount(), getTestAmount());
  EXPECT_EQ(tokenWipeTransaction.getSerialNumbers(), getTestSerialNumbers());
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenWipeTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenWipeTransaction transaction = TokenWipeTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetAccountId)
{
  // Given
  TokenWipeTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAccountId(getTestAccountId()));

  // Then
  EXPECT_EQ(transaction.getAccountId(), getTestAccountId());
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetAccountIdFrozen)
{
  // Given
  TokenWipeTransaction transaction = TokenWipeTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAccountId(getTestAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetAmount)
{
  // Given
  TokenWipeTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAmount(getTestAmount()));

  // Then
  EXPECT_EQ(transaction.getAmount(), getTestAmount());
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetAmountFrozen)
{
  // Given
  TokenWipeTransaction transaction = TokenWipeTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAmount(getTestAmount()), IllegalStateException);
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetSerialNumbers)
{
  // Given
  TokenWipeTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setSerialNumbers(getTestSerialNumbers()));

  // Then
  EXPECT_EQ(transaction.getSerialNumbers(), getTestSerialNumbers());
}

//-----
TEST_F(TokenWipeTransactionUnitTests, GetSetSerialNumbersFrozen)
{
  // Given
  TokenWipeTransaction transaction = TokenWipeTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setSerialNumbers(getTestSerialNumbers()), IllegalStateException);
}
