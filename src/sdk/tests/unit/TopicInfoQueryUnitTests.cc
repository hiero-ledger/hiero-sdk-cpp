// SPDX-License-Identifier: Apache-2.0
#include "TopicInfoQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TopicInfoQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TopicId& getTestTopicId() const { return mTestTopicId; }

private:
  const TopicId mTestTopicId = TopicId(1ULL);
};

//-----
TEST_F(TopicInfoQueryUnitTests, GetSetTopicId)
{
  // Given
  TopicInfoQuery query;

  // When
  query.setTopicId(getTestTopicId());

  // Then
  EXPECT_EQ(query.getTopicId(), getTestTopicId());
}
