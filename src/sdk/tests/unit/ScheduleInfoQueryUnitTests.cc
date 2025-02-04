// SPDX-License-Identifier: Apache-2.0
#include "ScheduleInfoQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class ScheduleInfoQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ScheduleId& getTestScheduleId() const { return mTestScheduleId; }

private:
  const ScheduleId mTestScheduleId = ScheduleId(1ULL);
};

//-----
TEST_F(ScheduleInfoQueryUnitTests, GetSetScheduleId)
{
  // Given
  ScheduleInfoQuery query;

  // When
  query.setScheduleId(getTestScheduleId());

  // Then
  EXPECT_EQ(query.getScheduleId(), getTestScheduleId());
}
