// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TokenMintTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class TokenMintTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }
  [[nodiscard]] inline const uint64_t& getTestAmount() const { return mTestAmount; }
  [[nodiscard]] inline const std::vector<std::vector<std::byte>>& getTestMetadataList() const
  {
    return mTestMetadataList;
  }

private:
  const TokenId mTestTokenId = TokenId(1ULL, 2ULL, 3ULL);
  const uint64_t mTestAmount = 4ULL;
  const std::vector<std::vector<std::byte>> mTestMetadataList = {
    {std::byte(0x05),  std::byte(0x06), std::byte(0x07)},
    { std::byte(0x08), std::byte(0x09), std::byte(0x0A)},
    { std::byte(0x0B), std::byte(0x0C), std::byte(0x0D)}
  };
};

//-----
TEST_F(TokenMintTransactionUnitTests, ConstructTokenMintTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenMintTransactionBody>();
  body->set_allocated_token(getTestTokenId().toProtobuf().release());
  body->set_amount(getTestAmount());

  for (const auto& metadata : getTestMetadataList())
  {
    body->add_metadata(internal::Utilities::byteVectorToString(metadata));
  }

  proto::TransactionBody txBody;
  txBody.set_allocated_tokenmint(body.release());

  // When
  const TokenMintTransaction tokenMintTransaction(txBody);

  // Then
  EXPECT_EQ(tokenMintTransaction.getTokenId(), getTestTokenId());
  EXPECT_EQ(tokenMintTransaction.getAmount(), getTestAmount());
  EXPECT_EQ(tokenMintTransaction.getMetadata(), getTestMetadataList());
}

//-----
TEST_F(TokenMintTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenMintTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenMintTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenMintTransaction transaction = TokenMintTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}

//-----
TEST_F(TokenMintTransactionUnitTests, GetSetAmount)
{
  // Given
  TokenMintTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAmount(getTestAmount()));

  // Then
  EXPECT_EQ(transaction.getAmount(), getTestAmount());
}

//-----
TEST_F(TokenMintTransactionUnitTests, GetSetAmountFrozen)
{
  // Given
  TokenMintTransaction transaction = TokenMintTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAmount(getTestAmount()), IllegalStateException);
}

//-----
TEST_F(TokenMintTransactionUnitTests, GetSetMetadata)
{
  // Given
  TokenMintTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setMetadata(getTestMetadataList()));

  // Then
  EXPECT_EQ(transaction.getMetadata(), getTestMetadataList());
}

//-----
TEST_F(TokenMintTransactionUnitTests, GetSetMetadataFrozen)
{
  // Given
  TokenMintTransaction transaction = TokenMintTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setMetadata(getTestMetadataList()), IllegalStateException);
}

//-----
TEST_F(TokenMintTransactionUnitTests, AddMetadata)
{
  // Given
  TokenMintTransaction tokenMintTransaction;
  const std::vector<std::byte> metadata = { std::byte(0x0E), std::byte(0x0F) };

  // When
  EXPECT_NO_THROW(tokenMintTransaction.addMetadata(metadata));

  // Then
  EXPECT_EQ(tokenMintTransaction.getMetadata(), std::vector<std::vector<std::byte>>{ metadata });
}

//-----
TEST_F(TokenMintTransactionUnitTests, AddMetadataFrozen)
{
  // Given
  TokenMintTransaction transaction = TokenMintTransaction()
                                       .setNodeAccountIds({ AccountId(1ULL) })
                                       .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  const std::vector<std::byte> metadata = { std::byte(0x0E), std::byte(0x0F) };
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.addMetadata(metadata), IllegalStateException);
}
