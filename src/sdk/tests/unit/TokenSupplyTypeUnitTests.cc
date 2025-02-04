// SPDX-License-Identifier: Apache-2.0
#include "TokenSupplyType.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class TokenSupplyTypeUnitTests : public ::testing::Test
{
};

//-----
TEST_F(TokenSupplyTypeUnitTests, ProtobufTokenSupplyTypeToTokenSupplyType)
{
  // Given / When / Then
  ASSERT_TRUE(gProtobufTokenSupplyTypeToTokenSupplyType.find(proto::TokenSupplyType::INFINITE) !=
              gProtobufTokenSupplyTypeToTokenSupplyType.end());
  ASSERT_TRUE(gProtobufTokenSupplyTypeToTokenSupplyType.find(proto::TokenSupplyType::FINITE) !=
              gProtobufTokenSupplyTypeToTokenSupplyType.end());
  EXPECT_EQ(TokenSupplyType::INFINITE, gProtobufTokenSupplyTypeToTokenSupplyType.at(proto::TokenSupplyType::INFINITE));
  EXPECT_EQ(TokenSupplyType::FINITE, gProtobufTokenSupplyTypeToTokenSupplyType.at(proto::TokenSupplyType::FINITE));
}

//-----
TEST_F(TokenSupplyTypeUnitTests, TokenSupplyTypeToString)
{
  // Given / When / Then
  ASSERT_TRUE(gTokenSupplyTypeToString.find(TokenSupplyType::INFINITE) != gTokenSupplyTypeToString.end());
  ASSERT_TRUE(gTokenSupplyTypeToString.find(TokenSupplyType::FINITE) != gTokenSupplyTypeToString.end());
  EXPECT_STRCASEEQ(gTokenSupplyTypeToString.at(TokenSupplyType::INFINITE), "INFINITE");
  EXPECT_STRCASEEQ(gTokenSupplyTypeToString.at(TokenSupplyType::FINITE), "FINITE");
}
