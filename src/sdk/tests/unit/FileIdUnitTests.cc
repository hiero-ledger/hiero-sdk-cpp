// SPDX-License-Identifier: Apache-2.0
#include "FileId.h"

#include <services/basic_types.pb.h>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace Hiero;

class FileIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const uint64_t& getTestShardNum() const { return mShardNum; }
  [[nodiscard]] inline const uint64_t& getTestRealmNum() const { return mRealmNum; }
  [[nodiscard]] inline const uint64_t& getTestFileNum() const { return mFileNum; }

private:
  const uint64_t mShardNum = 1ULL;
  const uint64_t mRealmNum = 20ULL;
  const uint64_t mFileNum = 300ULL;
};

//-----
TEST_F(FileIdUnitTests, ConstructWithFileNum)
{
  // Given / When
  const FileId fileId(getTestFileNum());

  // Then
  EXPECT_EQ(fileId.mShardNum, 0ULL);
  EXPECT_EQ(fileId.mRealmNum, 0ULL);
  EXPECT_EQ(fileId.mFileNum, getTestFileNum());
}

//-----
TEST_F(FileIdUnitTests, ConstructWithShardRealmFileNum)
{
  // Given / When
  const FileId fileId(getTestShardNum(), getTestRealmNum(), getTestFileNum());

  // Then
  EXPECT_EQ(fileId.mShardNum, getTestShardNum());
  EXPECT_EQ(fileId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(fileId.mFileNum, getTestFileNum());
}

//-----
TEST_F(FileIdUnitTests, CompareFileIds)
{
  // Given / When / Then
  EXPECT_TRUE(FileId() == FileId());
  EXPECT_TRUE(FileId(getTestFileNum()) == FileId(getTestFileNum()));
  EXPECT_TRUE(FileId(getTestShardNum(), getTestRealmNum(), getTestFileNum()) ==
              FileId(getTestShardNum(), getTestRealmNum(), getTestFileNum()));
  EXPECT_FALSE(FileId(getTestFileNum()) == FileId(getTestFileNum() - 1ULL));
  EXPECT_FALSE(FileId(getTestShardNum(), getTestRealmNum(), getTestFileNum()) ==
               FileId(getTestShardNum() - 1ULL, getTestRealmNum(), getTestFileNum()));
  EXPECT_FALSE(FileId(getTestShardNum(), getTestRealmNum(), getTestFileNum()) ==
               FileId(getTestShardNum(), getTestRealmNum() - 1ULL, getTestFileNum()));
}

//-----
TEST_F(FileIdUnitTests, FromString)
{
  // Given
  FileId fileId;

  // When
  EXPECT_NO_THROW(fileId =
                    FileId::fromString(std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) +
                                       '.' + std::to_string(getTestFileNum())));

  // Then
  EXPECT_EQ(fileId.mShardNum, getTestShardNum());
  EXPECT_EQ(fileId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(fileId.mFileNum, getTestFileNum());
}

//-----
TEST_F(FileIdUnitTests, FromBadString)
{
  // Given
  FileId fileId;
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testFileNumStr = std::to_string(getTestFileNum());

  const std::string fileIdStrNoDots = testShardNumStr + testRealmNumStr + testFileNumStr;
  const std::string fileIdOneDotBefore = '.' + fileIdStrNoDots;
  const std::string fileIdOneDotBetweenShardRealm = testShardNumStr + '.' + testRealmNumStr + testFileNumStr;
  const std::string fileIdOneDotBetweenRealmFileNum = testShardNumStr + testRealmNumStr + '.' + testFileNumStr;
  const std::string fileIdOneDotAfter = fileIdStrNoDots + '.';
  const std::string fileIdTwoDotsBefore = '.' + fileIdOneDotBefore;
  const std::string fileIdOneDotBeforeOneBetweenShardRealm = '.' + fileIdOneDotBetweenShardRealm;
  const std::string fileIdOneDotBeforeOneBetweenRealmFileNum = '.' + fileIdOneDotBetweenRealmFileNum;
  const std::string fileIdOneDotBeforeOneAfter = fileIdOneDotBefore + '.';
  const std::string fileIdTwoDotsBetweenShardRealm = testShardNumStr + ".." + testRealmNumStr + testFileNumStr;
  const std::string fileIdOneDotBetweenShardRealmOneAfter = fileIdOneDotBetweenShardRealm + '.';
  const std::string fileIdTwoDotsBetweenRealmFileNum = testShardNumStr + testRealmNumStr + ".." + testFileNumStr;
  const std::string fileIdOneDotBetweenRealmFileNumOneAfter = fileIdOneDotBetweenRealmFileNum + '.';
  const std::string fileIdDotsBetweenAllParts =
    '.' + testShardNumStr + '.' + testRealmNumStr + '.' + testFileNumStr + '.';
  const std::string fileIdRandomAlphaChars = "this is a bad file id";
  const std::string fileIdWithDotsAndAlphaChars = "ab.cd.ef";

  // When / Then
  EXPECT_THROW(fileId = FileId::fromString(fileIdStrNoDots), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBefore), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBetweenShardRealm), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBetweenRealmFileNum), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotAfter), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdTwoDotsBefore), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBeforeOneBetweenShardRealm), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBeforeOneBetweenRealmFileNum), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBeforeOneAfter), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdTwoDotsBetweenShardRealm), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBetweenShardRealmOneAfter), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdTwoDotsBetweenRealmFileNum), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdOneDotBetweenRealmFileNumOneAfter), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdDotsBetweenAllParts), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdRandomAlphaChars), std::invalid_argument);
  EXPECT_THROW(fileId = FileId::fromString(fileIdWithDotsAndAlphaChars), std::invalid_argument);
}

//-----
TEST_F(FileIdUnitTests, FromProtobuf)
{
  // Given
  proto::FileID protoFileId;
  protoFileId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoFileId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoFileId.set_filenum(static_cast<int64_t>(getTestFileNum()));

  // When
  const FileId fileId = FileId::fromProtobuf(protoFileId);

  // Then
  EXPECT_EQ(fileId.mShardNum, getTestShardNum());
  EXPECT_EQ(fileId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(fileId.mFileNum, getTestFileNum());
}

//-----
TEST_F(FileIdUnitTests, ToProtobuf)
{
  // Given
  const FileId fileId(getTestShardNum(), getTestRealmNum(), getTestFileNum());

  // When
  const std::unique_ptr<proto::FileID> protoFileId = fileId.toProtobuf();

  // Then
  EXPECT_EQ(static_cast<uint64_t>(protoFileId->shardnum()), getTestShardNum());
  EXPECT_EQ(static_cast<uint64_t>(protoFileId->realmnum()), getTestRealmNum());
  EXPECT_EQ(static_cast<uint64_t>(protoFileId->filenum()), getTestFileNum());
}

//-----
TEST_F(FileIdUnitTests, ToString)
{
  // Given
  const FileId fileId;
  const FileId fileIdFileNum(getTestFileNum());
  const FileId fileIdShardRealmFileNum(getTestShardNum(), getTestRealmNum(), getTestFileNum());

  // When
  const std::string fileIdStr = fileId.toString();
  const std::string fileIdFileNumStr = fileIdFileNum.toString();
  const std::string fileIdShardRealmFileNumStr = fileIdShardRealmFileNum.toString();

  // Then
  EXPECT_EQ(fileIdStr, "0.0.0");
  EXPECT_EQ(fileIdFileNumStr, "0.0." + std::to_string(getTestFileNum()));
  EXPECT_EQ(fileIdShardRealmFileNumStr,
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestFileNum()));
}

//-----
TEST_F(FileIdUnitTests, GetAddressBookFileIdForReturnsCorrectFileId)
{
  // Given / When
  const uint64_t defaultAddressBookFileIdShard = 0ULL;
  const uint64_t defaultAddressBookFileIdRealm = 0ULL;

  const uint64_t customAddressBookFileIdShard = 5ULL;
  const uint64_t customAddressBookFileIdRealm = 10ULL;

  const FileId defaultAddressBookFileId =
    FileId::getAddressBookFileIdFor(defaultAddressBookFileIdRealm, defaultAddressBookFileIdShard);

  const FileId customAddressBookFileId =
    FileId::getAddressBookFileIdFor(customAddressBookFileIdRealm, customAddressBookFileIdShard);

  // Then
  EXPECT_EQ(defaultAddressBookFileId.mShardNum, defaultAddressBookFileIdShard);
  EXPECT_EQ(defaultAddressBookFileId.mRealmNum, defaultAddressBookFileIdRealm);
  EXPECT_EQ(defaultAddressBookFileId.mFileNum, 102ULL);

  EXPECT_EQ(customAddressBookFileId.mShardNum, customAddressBookFileIdShard);
  EXPECT_EQ(customAddressBookFileId.mRealmNum, customAddressBookFileIdRealm);
  EXPECT_EQ(customAddressBookFileId.mFileNum, 102ULL);
}

//-----
TEST_F(FileIdUnitTests, GetScheduleFileIdForReturnsCorrectFileId)
{
  // Given / When
  const uint64_t defaultScheduleFileIdShard = 0ULL;
  const uint64_t defaultScheduleFileIdRealm = 0ULL;

  const uint64_t customScheduleFileIdShard = 5ULL;
  const uint64_t customScheduleFileIdRealm = 10ULL;

  const FileId defaultScheduleFileId =
    FileId::getFeeScheduleFileIdFor(defaultScheduleFileIdRealm, defaultScheduleFileIdShard);

  const FileId customScheduleFileId =
    FileId::getFeeScheduleFileIdFor(customScheduleFileIdRealm, customScheduleFileIdShard);

  // Then
  EXPECT_EQ(defaultScheduleFileId.mShardNum, defaultScheduleFileIdShard);
  EXPECT_EQ(defaultScheduleFileId.mRealmNum, defaultScheduleFileIdRealm);
  EXPECT_EQ(defaultScheduleFileId.mFileNum, 101ULL);

  EXPECT_EQ(customScheduleFileId.mShardNum, customScheduleFileIdShard);
  EXPECT_EQ(customScheduleFileId.mRealmNum, customScheduleFileIdRealm);
  EXPECT_EQ(customScheduleFileId.mFileNum, 101ULL);
}

//-----
TEST_F(FileIdUnitTests, GetExchangeRatesFileIdForReturnsCorrectFileId)
{
  // Given / When
  const uint64_t defaultExchangeRatesFileIdShard = 0ULL;
  const uint64_t defaultExchangeRatesFileIdRealm = 0ULL;

  const uint64_t customExchangeRatesFileIdShard = 5ULL;
  const uint64_t customExchangeRatesFileIdRealm = 10ULL;

  const FileId defaultExchangeRatesFileId =
    FileId::getExchangeRatesFileIdFor(defaultExchangeRatesFileIdRealm, defaultExchangeRatesFileIdShard);

  const FileId customExchangeRatesFileId =
    FileId::getExchangeRatesFileIdFor(customExchangeRatesFileIdRealm, customExchangeRatesFileIdShard);

  // Then
  EXPECT_EQ(defaultExchangeRatesFileId.mShardNum, defaultExchangeRatesFileIdShard);
  EXPECT_EQ(defaultExchangeRatesFileId.mRealmNum, defaultExchangeRatesFileIdRealm);
  EXPECT_EQ(defaultExchangeRatesFileId.mFileNum, 112ULL);

  EXPECT_EQ(customExchangeRatesFileId.mShardNum, customExchangeRatesFileIdShard);
  EXPECT_EQ(customExchangeRatesFileId.mRealmNum, customExchangeRatesFileIdRealm);
  EXPECT_EQ(customExchangeRatesFileId.mFileNum, 112ULL);
}
