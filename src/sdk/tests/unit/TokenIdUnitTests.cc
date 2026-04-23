// SPDX-License-Identifier: Apache-2.0
#include "TokenId.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>

using namespace Hiero;

class TokenIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const uint64_t& getTestShardNum() const { return mShardNum; }
  [[nodiscard]] inline const uint64_t& getTestRealmNum() const { return mRealmNum; }
  [[nodiscard]] inline const uint64_t& getTestTokenNum() const { return mTokenNum; }

private:
  const uint64_t mShardNum = 8ULL;
  const uint64_t mRealmNum = 90ULL;
  const uint64_t mTokenNum = 1000ULL;
};

//-----
// Constructor Tests
//-----

//-----
TEST_F(TokenIdUnitTests, ConstructWithTokenNum)
{
  // Given / When
  const TokenId tokenId(getTestTokenNum());

  // Then
  EXPECT_EQ(tokenId.mShardNum, 0ULL);
  EXPECT_EQ(tokenId.mRealmNum, 0ULL);
  EXPECT_EQ(tokenId.mTokenNum, getTestTokenNum());
}

//-----
TEST_F(TokenIdUnitTests, ConstructWithShardRealmTokenNum)
{
  // Given / When
  const TokenId tokenId(getTestShardNum(), getTestRealmNum(), getTestTokenNum());

  // Then
  EXPECT_EQ(tokenId.mShardNum, getTestShardNum());
  EXPECT_EQ(tokenId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(tokenId.mTokenNum, getTestTokenNum());
}

//-----
// Comparison Tests
//-----

//-----
TEST_F(TokenIdUnitTests, CompareTokenIds)
{
  // Given / When / Then
  EXPECT_TRUE(TokenId() == TokenId());
  EXPECT_TRUE(TokenId(getTestTokenNum()) == TokenId(getTestTokenNum()));
  EXPECT_TRUE(TokenId(getTestShardNum(), getTestRealmNum(), getTestTokenNum()) ==
              TokenId(getTestShardNum(), getTestRealmNum(), getTestTokenNum()));
}

//-----
// FromString Tests
//-----

//-----
TEST_F(TokenIdUnitTests, FromStringWithValidShardRealmNum)
{
  // Given
  const std::string validId = std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
                              std::to_string(getTestTokenNum());

  // When
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenId::fromString(validId));

  // Then
  EXPECT_EQ(tokenId.mShardNum, getTestShardNum());
  EXPECT_EQ(tokenId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(tokenId.mTokenNum, getTestTokenNum());
}

//-----
TEST_F(TokenIdUnitTests, FromStringThrowsWithMissingDelimiters)
{
  // Given
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testTokenNumStr = std::to_string(getTestTokenNum());

  // When / Then
  EXPECT_THROW(TokenId::fromString(testShardNumStr + testRealmNumStr + testTokenNumStr), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(testShardNumStr + '.' + testRealmNumStr + testTokenNumStr), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(testShardNumStr + testRealmNumStr + '.' + testTokenNumStr), std::invalid_argument);
}

//-----
TEST_F(TokenIdUnitTests, FromStringThrowsWithExtraDelimiters)
{
  // Given
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testTokenNumStr = std::to_string(getTestTokenNum());

  // When / Then
  EXPECT_THROW(TokenId::fromString('.' + testShardNumStr + testRealmNumStr + testTokenNumStr), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(testShardNumStr + testRealmNumStr + testTokenNumStr + '.'), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(".." + testShardNumStr + testRealmNumStr + testTokenNumStr), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(TokenId::fromString('.' + testShardNumStr + testRealmNumStr + '.' + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(TokenId::fromString('.' + testShardNumStr + testRealmNumStr + testTokenNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(testShardNumStr + ".." + testRealmNumStr + testTokenNumStr), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(testShardNumStr + '.' + testRealmNumStr + testTokenNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(testShardNumStr + testRealmNumStr + ".." + testTokenNumStr), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString(testShardNumStr + testRealmNumStr + '.' + testTokenNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(
    TokenId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + '.' + testTokenNumStr + '.'),
    std::invalid_argument);
}

//-----
TEST_F(TokenIdUnitTests, FromStringThrowsWithNonNumericInput)
{
  // Given / When / Then
  EXPECT_THROW(TokenId::fromString("abc"), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString("o.o.e"), std::invalid_argument);
  EXPECT_THROW(TokenId::fromString("0.0.1!"), std::invalid_argument);
}

//-----
// Protobuf Serialization Tests
//-----

//-----
TEST_F(TokenIdUnitTests, ProtobufSerializeTokenId)
{
  // Given
  TokenId tokenId(getTestShardNum(), getTestRealmNum(), getTestTokenNum());

  // When
  std::unique_ptr<proto::TokenID> protoTokenId = tokenId.toProtobuf();

  // Then
  EXPECT_EQ(protoTokenId->shardnum(), getTestShardNum());
  EXPECT_EQ(protoTokenId->realmnum(), getTestRealmNum());
  EXPECT_EQ(protoTokenId->tokennum(), getTestTokenNum());
}

//-----
TEST_F(TokenIdUnitTests, ProtobufDeserializeTokenId)
{
  // Given
  const uint64_t adjustment = 3ULL;
  const uint64_t newShard = getTestShardNum() + adjustment;
  const uint64_t newRealm = getTestRealmNum() - adjustment;
  const uint64_t newToken = getTestTokenNum() * adjustment;

  proto::TokenID protoTokenId;
  protoTokenId.set_shardnum(static_cast<int64_t>(newShard));
  protoTokenId.set_realmnum(static_cast<int64_t>(newRealm));
  protoTokenId.set_tokennum(static_cast<int64_t>(newToken));

  // When
  const TokenId tokenId = TokenId::fromProtobuf(protoTokenId);

  // Then
  EXPECT_EQ(tokenId.mShardNum, newShard);
  EXPECT_EQ(tokenId.mRealmNum, newRealm);
  EXPECT_EQ(tokenId.mTokenNum, newToken);
}

//-----
// ToString Tests
//-----

//-----
TEST_F(TokenIdUnitTests, ToStringDefaultTokenId)
{
  // Given
  const TokenId tokenId;

  // When / Then
  EXPECT_EQ(tokenId.toString(), "0.0.0");
}

//-----
TEST_F(TokenIdUnitTests, ToStringWithShardRealmTokenNum)
{
  // Given
  TokenId tokenId;
  tokenId.mShardNum = getTestShardNum();
  tokenId.mRealmNum = getTestRealmNum();
  tokenId.mTokenNum = getTestTokenNum();

  // When / Then
  EXPECT_EQ(tokenId.toString(),
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestTokenNum()));
}
