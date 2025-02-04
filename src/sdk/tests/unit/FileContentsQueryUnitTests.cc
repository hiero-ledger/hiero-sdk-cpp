// SPDX-License-Identifier: Apache-2.0
#include "FileContentsQuery.h"
#include "FileId.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FileContentsQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }

private:
  const FileId mTestFileId = FileId(1ULL);
};

//-----
TEST_F(FileContentsQueryUnitTests, GetSetFileId)
{
  // Given
  FileContentsQuery query;

  // When
  query.setFileId(getTestFileId());

  // Then
  EXPECT_EQ(query.getFileId(), getTestFileId());
}
