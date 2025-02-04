// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TopicId.h"
#include "TopicMessageQuery.h"

#include <chrono>
#include <gtest/gtest.h>

using namespace Hiero;

class TopicMessageQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TopicId& getTestTopicId() const { return mTestTopicId; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestStartTime() const { return mTestStartTime; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestEndTime() const { return mTestEndTime; }
  [[nodiscard]] inline uint64_t getTestLimit() const { return mTestLimit; }
  [[nodiscard]] inline uint32_t getTestMaxAttempts() const { return mTestMaxAttempts; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestMaxBackoff() const { return mTestMaxBackoff; }

private:
  const TopicId mTestTopicId = TopicId(1ULL, 2ULL, 3ULL);
  const std::chrono::system_clock::time_point mTestStartTime = std::chrono::system_clock::now();
  const std::chrono::system_clock::time_point mTestEndTime = mTestStartTime + std::chrono::seconds(4);
  const uint64_t mTestLimit = 5ULL;
  const uint32_t mTestMaxAttempts = 6ULL;
  const std::chrono::system_clock::duration mTestMaxBackoff = std::chrono::seconds(7);
};

//-----
TEST_F(TopicMessageQueryUnitTests, CopyConstructor)
{
  // Given
  TopicMessageQuery query;
  query.setTopicId(getTestTopicId());
  query.setStartTime(getTestStartTime());
  query.setEndTime(getTestEndTime());
  query.setLimit(getTestLimit());
  query.setMaxAttempts(getTestMaxAttempts());
  query.setMaxBackoff(getTestMaxBackoff());

  // When
  const TopicMessageQuery newQuery(query);

  // Then
  EXPECT_EQ(newQuery.getTopicId(), getTestTopicId());
  EXPECT_EQ(newQuery.getStartTime(), getTestStartTime());
  EXPECT_EQ(newQuery.getEndTime(), getTestEndTime());
  EXPECT_EQ(newQuery.getLimit(), getTestLimit());
  EXPECT_EQ(newQuery.getMaxAttempts(), getTestMaxAttempts());
  EXPECT_EQ(newQuery.getMaxBackoff(), getTestMaxBackoff());
}

//-----
TEST_F(TopicMessageQueryUnitTests, CopyAssignmentOperator)
{
  // Given
  TopicMessageQuery query;
  query.setTopicId(getTestTopicId());
  query.setStartTime(getTestStartTime());
  query.setEndTime(getTestEndTime());
  query.setLimit(getTestLimit());
  query.setMaxAttempts(getTestMaxAttempts());
  query.setMaxBackoff(getTestMaxBackoff());

  // When
  const TopicMessageQuery newQuery = query;

  // Then
  EXPECT_EQ(newQuery.getTopicId(), getTestTopicId());
  EXPECT_EQ(newQuery.getStartTime(), getTestStartTime());
  EXPECT_EQ(newQuery.getEndTime(), getTestEndTime());
  EXPECT_EQ(newQuery.getLimit(), getTestLimit());
  EXPECT_EQ(newQuery.getMaxAttempts(), getTestMaxAttempts());
  EXPECT_EQ(newQuery.getMaxBackoff(), getTestMaxBackoff());
}

//-----
TEST_F(TopicMessageQueryUnitTests, MoveConstructor)
{
  // Given
  TopicMessageQuery query;
  query.setTopicId(getTestTopicId());
  query.setStartTime(getTestStartTime());
  query.setEndTime(getTestEndTime());
  query.setLimit(getTestLimit());
  query.setMaxAttempts(getTestMaxAttempts());
  query.setMaxBackoff(getTestMaxBackoff());

  // When
  const TopicMessageQuery newQuery(std::move(query));

  // Then
  EXPECT_EQ(newQuery.getTopicId(), getTestTopicId());
  EXPECT_EQ(newQuery.getStartTime(), getTestStartTime());
  EXPECT_EQ(newQuery.getEndTime(), getTestEndTime());
  EXPECT_EQ(newQuery.getLimit(), getTestLimit());
  EXPECT_EQ(newQuery.getMaxAttempts(), getTestMaxAttempts());
  EXPECT_EQ(newQuery.getMaxBackoff(), getTestMaxBackoff());
}

//-----
TEST_F(TopicMessageQueryUnitTests, MoveAssignmentOperator)
{
  // Given
  TopicMessageQuery query;
  query.setTopicId(getTestTopicId());
  query.setStartTime(getTestStartTime());
  query.setEndTime(getTestEndTime());
  query.setLimit(getTestLimit());
  query.setMaxAttempts(getTestMaxAttempts());
  query.setMaxBackoff(getTestMaxBackoff());

  // When
  const TopicMessageQuery newQuery = std::move(query);

  // Then
  EXPECT_EQ(newQuery.getTopicId(), getTestTopicId());
  EXPECT_EQ(newQuery.getStartTime(), getTestStartTime());
  EXPECT_EQ(newQuery.getEndTime(), getTestEndTime());
  EXPECT_EQ(newQuery.getLimit(), getTestLimit());
  EXPECT_EQ(newQuery.getMaxAttempts(), getTestMaxAttempts());
  EXPECT_EQ(newQuery.getMaxBackoff(), getTestMaxBackoff());
}

//-----
TEST_F(TopicMessageQueryUnitTests, GetSetTopicId)
{
  // Given
  TopicMessageQuery query;

  // When
  query.setTopicId(getTestTopicId());

  // Then
  EXPECT_EQ(query.getTopicId(), getTestTopicId());
}

//-----
TEST_F(TopicMessageQueryUnitTests, GetSetStartTime)
{
  // Given
  TopicMessageQuery query;

  // When
  query.setStartTime(getTestStartTime());

  // Then
  EXPECT_EQ(query.getStartTime(), getTestStartTime());
}

//-----
TEST_F(TopicMessageQueryUnitTests, GetSetEndTime)
{
  // Given
  TopicMessageQuery query;

  // When
  query.setEndTime(getTestEndTime());

  // Then
  EXPECT_EQ(query.getEndTime(), getTestEndTime());
}

//-----
TEST_F(TopicMessageQueryUnitTests, GetSetLimit)
{
  // Given
  TopicMessageQuery query;

  // When
  query.setLimit(getTestLimit());

  // Then
  EXPECT_EQ(query.getLimit(), getTestLimit());
}

//-----
TEST_F(TopicMessageQueryUnitTests, GetSetMaxAttempts)
{
  // Given
  TopicMessageQuery query;

  // When
  query.setMaxAttempts(getTestMaxAttempts());

  // Then
  EXPECT_EQ(query.getMaxAttempts(), getTestMaxAttempts());
}

//-----
TEST_F(TopicMessageQueryUnitTests, GetSetMaxBackoff)
{
  // Given
  TopicMessageQuery query;

  // When
  query.setMaxBackoff(getTestMaxBackoff());

  // Then
  EXPECT_EQ(query.getMaxBackoff(), getTestMaxBackoff());
}
