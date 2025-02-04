// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "NftId.h"
#include "TokenId.h"
#include "TokenRejectTransaction.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class TokenRejectTransactionUnitTests : public ::testing::Test
{
protected:
  TokenRejectTransaction transaction;
};

//-----
TEST_F(TokenRejectTransactionUnitTests, ConstructTokenRejectTransactionFromTransactionBodyProtobuf)
{
  // Given
  proto::TransactionBody transactionBody;
  proto::TokenRejectTransactionBody* body = transactionBody.mutable_tokenreject();

  proto::TokenID* fungibleTokenId = body->add_rejections()->mutable_fungible_token();
  fungibleTokenId->set_shardnum(1);
  fungibleTokenId->set_realmnum(2);
  fungibleTokenId->set_tokennum(3);

  proto::TokenID* nonFungibleTokenId = body->add_rejections()->mutable_nft()->mutable_token_id();
  nonFungibleTokenId->set_shardnum(4);
  nonFungibleTokenId->set_realmnum(5);
  nonFungibleTokenId->set_tokennum(6);

  // When
  TokenRejectTransaction tokenRejectTransaction(transactionBody);

  // Then
  EXPECT_EQ(tokenRejectTransaction.getFts().size(), 1);
  EXPECT_EQ(tokenRejectTransaction.getNfts().size(), 1);
  EXPECT_EQ(tokenRejectTransaction.getFts().at(0), TokenId(1, 2, 3));
  EXPECT_EQ(tokenRejectTransaction.getNfts().at(0), NftId(TokenId(4, 5, 6), 0)); // Assuming serial number 0 by default
}

//-----
TEST_F(TokenRejectTransactionUnitTests, SetOwner)
{
  // Given
  AccountId ownerId(1, 2, 3);

  // When
  transaction.setOwner(ownerId);

  // Then
  EXPECT_TRUE(transaction.getOwner().has_value());
  EXPECT_EQ(transaction.getOwner().value(), ownerId);
}

//-----
TEST_F(TokenRejectTransactionUnitTests, SetFts)
{
  // Given
  std::vector<TokenId> fts = { TokenId(1, 2, 3), TokenId(4, 5, 6) };

  // When
  transaction.setFts(fts);

  // Then
  EXPECT_EQ(transaction.getFts().size(), 2);
  EXPECT_EQ(transaction.getFts().at(0), TokenId(1, 2, 3));
  EXPECT_EQ(transaction.getFts().at(1), TokenId(4, 5, 6));
}

//-----
TEST_F(TokenRejectTransactionUnitTests, SetNfts)
{
  // Given
  std::vector<NftId> nfts = { NftId(TokenId(1, 2, 3), 1), NftId(TokenId(4, 5, 6), 2) };

  // When
  transaction.setNfts(nfts);

  // Then
  EXPECT_EQ(transaction.getNfts().size(), 2);
  EXPECT_EQ(transaction.getNfts().at(0), NftId(TokenId(1, 2, 3), 1));
  EXPECT_EQ(transaction.getNfts().at(1), NftId(TokenId(4, 5, 6), 2));
}
