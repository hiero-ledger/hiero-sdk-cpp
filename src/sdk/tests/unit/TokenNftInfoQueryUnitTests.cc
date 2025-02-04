// SPDX-License-Identifier: Apache-2.0
#include "TokenNftInfoQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenNftInfoQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const NftId& getTestNftId() const { return mTestNftId; }

private:
  const NftId mTestNftId = NftId(TokenId(1ULL, 2ULL, 3ULL), 4ULL);
};

//-----
TEST_F(TokenNftInfoQueryUnitTests, GetSetNftId)
{
  // Given
  TokenNftInfoQuery query;

  // When
  query.setNftId(getTestNftId());

  // Then
  EXPECT_EQ(query.getNftId(), getTestNftId());
}
