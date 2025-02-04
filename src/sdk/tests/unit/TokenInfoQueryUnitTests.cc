// SPDX-License-Identifier: Apache-2.0
#include "TokenInfoQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenInfoQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const TokenId mTestTokenId = TokenId(1ULL);
};

//-----
TEST_F(TokenInfoQueryUnitTests, GetSetTokenId)
{
  // Given
  TokenInfoQuery query;

  // When
  query.setTokenId(getTestTokenId());

  // Then
  EXPECT_EQ(query.getTokenId(), getTestTokenId());
}
