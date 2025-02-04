// SPDX-License-Identifier: Apache-2.0
#include "FileInfo.h"
#include "FileInfoQuery.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FileInfoQueryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }

private:
  const FileId mTestFileId = FileId(1ULL);
};

//-----
TEST_F(FileInfoQueryUnitTests, GetSetFileId)
{
  // Given
  FileInfoQuery query;

  // When
  query.setFileId(getTestFileId());

  // Then
  EXPECT_EQ(query.getfileId(), getTestFileId());
}
