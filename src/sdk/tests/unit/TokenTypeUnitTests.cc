// SPDX-License-Identifier: Apache-2.0
#include "TokenType.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class TokenTypeUnitTests : public ::testing::Test
{
};

//-----
TEST_F(TokenTypeUnitTests, ProtobufTokenTypeToTokenType)
{
  // Given / When / Then
  ASSERT_TRUE(gProtobufTokenTypeToTokenType.find(proto::TokenType::FUNGIBLE_COMMON) !=
              gProtobufTokenTypeToTokenType.end());
  ASSERT_TRUE(gProtobufTokenTypeToTokenType.find(proto::TokenType::NON_FUNGIBLE_UNIQUE) !=
              gProtobufTokenTypeToTokenType.end());
  EXPECT_EQ(TokenType::FUNGIBLE_COMMON, gProtobufTokenTypeToTokenType.at(proto::TokenType::FUNGIBLE_COMMON));
  EXPECT_EQ(TokenType::NON_FUNGIBLE_UNIQUE, gProtobufTokenTypeToTokenType.at(proto::TokenType::NON_FUNGIBLE_UNIQUE));
}

//-----
TEST_F(TokenTypeUnitTests, TokenTypeToString)
{
  // Given / When / Then
  ASSERT_TRUE(gTokenTypeToString.find(TokenType::FUNGIBLE_COMMON) != gTokenTypeToString.end());
  ASSERT_TRUE(gTokenTypeToString.find(TokenType::NON_FUNGIBLE_UNIQUE) != gTokenTypeToString.end());
  EXPECT_STRCASEEQ(gTokenTypeToString.at(TokenType::FUNGIBLE_COMMON), "FUNGIBLE_COMMON");
  EXPECT_STRCASEEQ(gTokenTypeToString.at(TokenType::NON_FUNGIBLE_UNIQUE), "NON_FUNGIBLE_UNIQUE");
}
