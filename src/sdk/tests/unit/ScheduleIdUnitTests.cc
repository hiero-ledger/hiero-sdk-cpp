// SPDX-License-Identifier: Apache-2.0
#include "ScheduleId.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

using namespace Hiero;

class ScheduleIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const uint64_t& getTestShardNum() const { return mTestShardNum; }
  [[nodiscard]] inline const uint64_t& getTestRealmNum() const { return mTestRealmNum; }
  [[nodiscard]] inline const uint64_t& getTestScheduleNum() const { return mTestScheduleNum; }

private:
  const uint64_t mTestShardNum = 8ULL;
  const uint64_t mTestRealmNum = 90ULL;
  const uint64_t mTestScheduleNum = 1000ULL;
};

//-----
TEST_F(ScheduleIdUnitTests, ConstructWithScheduleNum)
{
  // Given / When
  const ScheduleId topicId(getTestScheduleNum());

  // Then
  EXPECT_EQ(topicId.mShardNum, 0ULL);
  EXPECT_EQ(topicId.mRealmNum, 0ULL);
  EXPECT_EQ(topicId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, ConstructWithShardRealmScheduleNum)
{
  // Given / When
  const ScheduleId topicId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum());

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, CompareScheduleIds)
{
  EXPECT_TRUE(ScheduleId() == ScheduleId());
  EXPECT_TRUE(ScheduleId(getTestScheduleNum()) == ScheduleId(getTestScheduleNum()));
  EXPECT_TRUE(ScheduleId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum()) ==
              ScheduleId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum()));

  EXPECT_FALSE(ScheduleId(getTestScheduleNum()) == ScheduleId(getTestScheduleNum() - 1ULL));
  EXPECT_FALSE(ScheduleId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum()) ==
               ScheduleId(getTestShardNum() - 1ULL, getTestRealmNum(), getTestScheduleNum()));
  EXPECT_FALSE(ScheduleId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum()) ==
               ScheduleId(getTestShardNum(), getTestRealmNum() - 1ULL, getTestScheduleNum()));
}

//-----
TEST_F(ScheduleIdUnitTests, FromString)
{
  // Given
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testAccountNumStr = std::to_string(getTestScheduleNum());

  // When
  ScheduleId topicId;
  EXPECT_NO_THROW(topicId = ScheduleId::fromString(testShardNumStr + '.' + testRealmNumStr + '.' + testAccountNumStr));

  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString('.' + testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + '.' + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(".." + testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString('.' + testShardNumStr + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString('.' + testShardNumStr + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + ".." + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + '.' + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + testRealmNumStr + ".." + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString(testShardNumStr + testRealmNumStr + '.' + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId =
                 ScheduleId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + '.' + testAccountNumStr + '.'),
               std::invalid_argument);

  EXPECT_THROW(topicId = ScheduleId::fromString("abc"), std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString("o.o.e"), std::invalid_argument);
  EXPECT_THROW(topicId = ScheduleId::fromString("0.0.1!"), std::invalid_argument);

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, FromProtobuf)
{
  // Given
  proto::ScheduleID protoScheduleId;
  protoScheduleId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoScheduleId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoScheduleId.set_schedulenum(static_cast<int64_t>(getTestScheduleNum()));

  // When
  const ScheduleId topicId = ScheduleId::fromProtobuf(protoScheduleId);

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, FromBytes)
{
  // Given
  proto::ScheduleID protoScheduleId;
  protoScheduleId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoScheduleId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoScheduleId.set_schedulenum(static_cast<int64_t>(getTestScheduleNum()));

  // When
  const ScheduleId topicId =
    ScheduleId::fromBytes(internal::Utilities::stringToByteVector(protoScheduleId.SerializeAsString()));

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, ToProtobuf)
{
  // Given
  const ScheduleId topicId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum());

  // When
  const std::unique_ptr<proto::ScheduleID> protoScheduleId = topicId.toProtobuf();

  // Then
  EXPECT_EQ(protoScheduleId->shardnum(), getTestShardNum());
  EXPECT_EQ(protoScheduleId->realmnum(), getTestRealmNum());
  EXPECT_EQ(protoScheduleId->schedulenum(), getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, ToBytes)
{
  // Given
  proto::ScheduleID protoScheduleId;
  protoScheduleId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoScheduleId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoScheduleId.set_schedulenum(static_cast<int64_t>(getTestScheduleNum()));

  const std::vector<std::byte> protoBytes =
    internal::Utilities::stringToByteVector(protoScheduleId.SerializeAsString());
  const ScheduleId topicId = ScheduleId::fromProtobuf(protoScheduleId);

  // When
  const std::vector<std::byte> bytes = topicId.toBytes();

  // Then
  EXPECT_EQ(protoBytes, bytes);
}

//-----
TEST_F(ScheduleIdUnitTests, ToString)
{
  // Given
  const ScheduleId topicId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum());

  // When
  std::string topicIdStr;
  EXPECT_NO_THROW(topicIdStr = topicId.toString());

  // Then
  EXPECT_EQ(topicIdStr,
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestScheduleNum()));
}
