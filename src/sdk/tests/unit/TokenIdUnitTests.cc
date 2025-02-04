// SPDX-License-Identifier: Apache-2.0
#include "TokenId.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

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
TEST_F(TokenIdUnitTests, CompareTokenIds)
{
  EXPECT_TRUE(TokenId() == TokenId());
  EXPECT_TRUE(TokenId(getTestTokenNum()) == TokenId(getTestTokenNum()));
  EXPECT_TRUE(TokenId(getTestShardNum(), getTestRealmNum(), getTestTokenNum()) ==
              TokenId(getTestShardNum(), getTestRealmNum(), getTestTokenNum()));
}

//-----
TEST_F(TokenIdUnitTests, ConstructFromString)
{
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testTokenNumStr = std::to_string(getTestTokenNum());

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenId::fromString(testShardNumStr + '.' + testRealmNumStr + '.' + testTokenNumStr));
  EXPECT_EQ(tokenId.mShardNum, getTestShardNum());
  EXPECT_EQ(tokenId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(tokenId.mTokenNum, getTestTokenNum());

  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + testRealmNumStr + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString('.' + testShardNumStr + testRealmNumStr + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + '.' + testRealmNumStr + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + testRealmNumStr + '.' + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + testRealmNumStr + testTokenNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(".." + testShardNumStr + testRealmNumStr + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString('.' + testShardNumStr + testRealmNumStr + '.' + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString('.' + testShardNumStr + testRealmNumStr + testTokenNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + ".." + testRealmNumStr + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + '.' + testRealmNumStr + testTokenNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + testRealmNumStr + ".." + testTokenNumStr),
               std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString(testShardNumStr + testRealmNumStr + '.' + testTokenNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(tokenId =
                 TokenId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + '.' + testTokenNumStr + '.'),
               std::invalid_argument);

  EXPECT_THROW(tokenId = TokenId::fromString("abc"), std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString("o.o.e"), std::invalid_argument);
  EXPECT_THROW(tokenId = TokenId::fromString("0.0.1!"), std::invalid_argument);
}

//-----
TEST_F(TokenIdUnitTests, ProtobufTokenId)
{
  TokenId tokenId(getTestShardNum(), getTestRealmNum(), getTestTokenNum());

  // Serialize shard, realm, token number
  std::unique_ptr<proto::TokenID> protoTokenId = tokenId.toProtobuf();
  EXPECT_EQ(protoTokenId->shardnum(), getTestShardNum());
  EXPECT_EQ(protoTokenId->realmnum(), getTestRealmNum());
  EXPECT_EQ(protoTokenId->tokennum(), getTestTokenNum());

  // Adjust protobuf fields
  const uint64_t adjustment = 3ULL;
  const uint64_t newShard = getTestShardNum() + adjustment;
  const uint64_t newRealm = getTestRealmNum() - adjustment;
  const uint64_t newToken = getTestTokenNum() * adjustment;

  protoTokenId->set_shardnum(static_cast<int64_t>(newShard));
  protoTokenId->set_realmnum(static_cast<int64_t>(newRealm));
  protoTokenId->set_tokennum(static_cast<int64_t>(newToken));

  // Deserialize shard, realm, token number
  tokenId = TokenId::fromProtobuf(*protoTokenId);
  EXPECT_EQ(tokenId.mShardNum, newShard);
  EXPECT_EQ(tokenId.mRealmNum, newRealm);
  EXPECT_EQ(tokenId.mTokenNum, newToken);
}

//-----
TEST_F(TokenIdUnitTests, ToString)
{
  TokenId tokenId;
  EXPECT_EQ(tokenId.toString(), "0.0.0");

  tokenId.mShardNum = getTestShardNum();
  tokenId.mRealmNum = getTestRealmNum();
  tokenId.mTokenNum = getTestTokenNum();
  EXPECT_EQ(tokenId.toString(),
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestTokenNum()));
}
