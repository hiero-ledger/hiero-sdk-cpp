// SPDX-License-Identifier: Apache-2.0
#include "TokenAssociation.h"
#include "impl/Utilities.h"

#include <custom_fees.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class TokenAssociationUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const AccountId mTestAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const TokenId mTestTokenId = TokenId(4ULL, 5ULL, 6ULL);
};

//-----
TEST_F(TokenAssociationUnitTests, FromProtobuf)
{
  // Given
  proto::TokenAssociation protoTokenAssociation;
  protoTokenAssociation.set_allocated_account_id(getTestAccountId().toProtobuf().release());
  protoTokenAssociation.set_allocated_token_id(getTestTokenId().toProtobuf().release());

  // When
  const TokenAssociation tokenAssociation = TokenAssociation::fromProtobuf(protoTokenAssociation);

  // Then
  EXPECT_EQ(tokenAssociation.mAccountId, getTestAccountId());
  EXPECT_EQ(tokenAssociation.mTokenId, getTestTokenId());
}

//-----
TEST_F(TokenAssociationUnitTests, ToProtobuf)
{
  // Given
  TokenAssociation tokenAssociation;
  tokenAssociation.mAccountId = getTestAccountId();
  tokenAssociation.mTokenId = getTestTokenId();

  // When
  const std::unique_ptr<proto::TokenAssociation> protoTokenAssociation = tokenAssociation.toProtobuf();

  // Then
  ASSERT_TRUE(protoTokenAssociation->has_account_id());
  EXPECT_EQ(protoTokenAssociation->account_id().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(protoTokenAssociation->account_id().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(protoTokenAssociation->account_id().accountnum(), getTestAccountId().mAccountNum);
  ASSERT_TRUE(protoTokenAssociation->has_token_id());
  EXPECT_EQ(protoTokenAssociation->token_id().shardnum(), getTestTokenId().mShardNum);
  EXPECT_EQ(protoTokenAssociation->token_id().realmnum(), getTestTokenId().mRealmNum);
  EXPECT_EQ(protoTokenAssociation->token_id().tokennum(), getTestTokenId().mTokenNum);
}

//-----
TEST_F(TokenAssociationUnitTests, FromBytes)
{
  // Given
  proto::TokenAssociation protoTokenAssociation;
  protoTokenAssociation.set_allocated_account_id(getTestAccountId().toProtobuf().release());
  protoTokenAssociation.set_allocated_token_id(getTestTokenId().toProtobuf().release());

  // When
  const TokenAssociation tokenAssociation =
    TokenAssociation::fromBytes(internal::Utilities::stringToByteVector(protoTokenAssociation.SerializeAsString()));

  // Then
  EXPECT_EQ(tokenAssociation.mAccountId, getTestAccountId());
  EXPECT_EQ(tokenAssociation.mTokenId, getTestTokenId());
}

//-----
TEST_F(TokenAssociationUnitTests, ToBytes)
{
  // Given
  TokenAssociation tokenAssociation;
  tokenAssociation.mAccountId = getTestAccountId();
  tokenAssociation.mTokenId = getTestTokenId();

  // When
  const std::vector<std::byte> bytes = tokenAssociation.toBytes();

  // Then
  EXPECT_EQ(bytes, internal::Utilities::stringToByteVector(tokenAssociation.toProtobuf()->SerializeAsString()));
}
