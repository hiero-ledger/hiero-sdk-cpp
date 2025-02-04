// SPDX-License-Identifier: Apache-2.0
#include "FileInfo.h"
#include "PublicKey.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <file_get_info.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class FileInfoUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }
  [[nodiscard]] inline const uint64_t& getTestSize() const { return mTestSize; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline bool getTestIsDeleted() const { return mTestIsDeleted; }
  [[nodiscard]] inline const KeyList& getTestKeys() const { return mTestKeys; }
  [[nodiscard]] inline const LedgerId& getTestLedgerId() const { return mTestLedgerId; }

private:
  const FileId mTestFileId = FileId(1ULL);
  const uint64_t mTestSize = 2ULL;
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const bool mTestIsDeleted = true;
  const KeyList mTestKeys = KeyList::of({ PublicKey::fromStringDer(
    "302A300506032B6570032100D75A980182B10AB7D54BFED3C964073A0EE172f3DAA62325AF021A68F707511A") });
  const LedgerId mTestLedgerId = LedgerId({ std::byte(0x03), std::byte(0x04), std::byte(0x05) });
};

//-----
TEST_F(FileInfoUnitTests, FromProtobuf)
{
  // Given
  proto::FileGetInfoResponse_FileInfo protoFileInfo;
  protoFileInfo.set_allocated_fileid(getTestFileId().toProtobuf().release());
  protoFileInfo.set_size(static_cast<int64_t>(getTestSize()));
  protoFileInfo.set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  protoFileInfo.set_deleted(getTestIsDeleted());
  protoFileInfo.set_allocated_keys(getTestKeys().toProtobuf().release());
  protoFileInfo.set_ledger_id(internal::Utilities::byteVectorToString(getTestLedgerId().toBytes()));

  // When
  const FileInfo fileInfo = FileInfo::fromProtobuf(protoFileInfo);

  // Then
  EXPECT_EQ(fileInfo.mFileId, getTestFileId());
  EXPECT_EQ(fileInfo.mSize, getTestSize());
  EXPECT_EQ(fileInfo.mExpirationTime, getTestExpirationTime());
  EXPECT_EQ(fileInfo.mIsDeleted, getTestIsDeleted());
  EXPECT_EQ(fileInfo.mAdminKeys.toBytes(), getTestKeys().toBytes());
  EXPECT_EQ(fileInfo.mLedgerId.toBytes(), getTestLedgerId().toBytes());
}
