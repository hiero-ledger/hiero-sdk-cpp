// SPDX-License-Identifier: Apache-2.0
#include "AddressBookQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AddressBookQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }
  [[nodiscard]] inline unsigned int getTestLimit() const { return mTestLimit; }
  [[nodiscard]] inline unsigned int getTestMaxAttempts() const { return mTestMaxAttempts; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestMaxBackoff() const { return mTestMaxBackoff; }

private:
  const FileId mTestFileId = FileId(1ULL, 2ULL, 3ULL);
  const unsigned int mTestLimit = 4ULL;
  const unsigned int mTestMaxAttempts = 5ULL;
  const std::chrono::system_clock::duration mTestMaxBackoff = std::chrono::seconds(6ULL);
};

//-----
TEST_F(AddressBookQueryUnitTests, GetSetAccountId)
{
  // Given
  AddressBookQuery query;

  // When
  query.setFileId(getTestFileId());

  // Then
  EXPECT_EQ(query.getFileId(), getTestFileId());
}

//-----
TEST_F(AddressBookQueryUnitTests, GetSetLimit)
{
  // Given
  AddressBookQuery query;

  // When
  query.setLimit(getTestLimit());

  // Then
  EXPECT_EQ(query.getLimit(), getTestLimit());
}

//-----
TEST_F(AddressBookQueryUnitTests, GetSetMaxAttempts)
{
  // Given
  AddressBookQuery query;

  // When
  query.setMaxAttempts(getTestMaxAttempts());

  // Then
  EXPECT_EQ(query.getMaxAttempts(), getTestMaxAttempts());
}

//-----
TEST_F(AddressBookQueryUnitTests, GetSetMaxBackoff)
{
  // Given
  AddressBookQuery query;

  // When
  query.setMaxBackoff(getTestMaxBackoff());

  // Then
  EXPECT_EQ(query.getMaxBackoff(), getTestMaxBackoff());
}
