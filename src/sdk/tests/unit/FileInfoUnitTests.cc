// SPDX-License-Identifier: Apache-2.0
#include "FileInfo.h"
#include "PublicKey.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/file_get_info.pb.h>

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
  [[nodiscard]] inline const std::string& getTestMemo() const { return mTestMemo; }
  [[nodiscard]] inline const LedgerId& getTestLedgerId() const { return mTestLedgerId; }

  [[nodiscard]] FileInfo makeTestFileInfo() const
  {
    FileInfo fileInfo;
    fileInfo.mFileId = getTestFileId();
    fileInfo.mSize = getTestSize();
    fileInfo.mExpirationTime = getTestExpirationTime();
    fileInfo.mIsDeleted = getTestIsDeleted();
    fileInfo.mAdminKeys = getTestKeys();
    fileInfo.mMemo = getTestMemo();
    fileInfo.mLedgerId = getTestLedgerId();
    return fileInfo;
  }

private:
  const FileId mTestFileId = FileId(1ULL);
  const uint64_t mTestSize = 2ULL;
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const bool mTestIsDeleted = true;
  const KeyList mTestKeys = KeyList::of({ PublicKey::fromStringDer(
    "302A300506032B6570032100D75A980182B10AB7D54BFED3C964073A0EE172f3DAA62325AF021A68F707511A") });
  const std::string mTestMemo = "test memo";
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

//-----
TEST_F(FileInfoUnitTests, DefaultConstructedInstancesAreEqual)
{
  // Given
  const FileInfo lhs;
  const FileInfo rhs;

  // When / Then
  EXPECT_EQ(lhs, rhs);
}

//-----
TEST_F(FileInfoUnitTests, IdenticalInstancesAreEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  const FileInfo rhs = makeTestFileInfo();

  // When / Then
  EXPECT_EQ(lhs, rhs);
}

//-----
TEST_F(FileInfoUnitTests, DifferingFileIdsAreNotEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  FileInfo rhs = makeTestFileInfo();
  rhs.mFileId = FileId(2ULL);

  // When / Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FileInfoUnitTests, DifferingSizesAreNotEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  FileInfo rhs = makeTestFileInfo();
  ++rhs.mSize;

  // When / Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FileInfoUnitTests, DifferingExpirationTimesAreNotEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  FileInfo rhs = makeTestFileInfo();
  rhs.mExpirationTime += std::chrono::seconds(1);

  // When / Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FileInfoUnitTests, DifferingDeletedStatesAreNotEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  FileInfo rhs = makeTestFileInfo();
  rhs.mIsDeleted = !rhs.mIsDeleted;

  // When / Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FileInfoUnitTests, DifferingAdminKeysAreNotEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  FileInfo rhs = makeTestFileInfo();
  rhs.mAdminKeys = KeyList::of({ PublicKey::fromStringDer(
    "302A300506032B65700321008CCD31B53D1835B467AAC795DAB19B274DD3B37E3DAF12FCEC6BC02BAC87B53D") });

  // When / Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FileInfoUnitTests, DifferingMemosAreNotEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  FileInfo rhs = makeTestFileInfo();
  rhs.mMemo = "different memo";

  // When / Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FileInfoUnitTests, DifferingLedgerIdsAreNotEqual)
{
  // Given
  const FileInfo lhs = makeTestFileInfo();
  FileInfo rhs = makeTestFileInfo();
  rhs.mLedgerId = LedgerId({ std::byte(0x06), std::byte(0x07), std::byte(0x08) });

  // When / Then
  EXPECT_FALSE(lhs == rhs);
}
