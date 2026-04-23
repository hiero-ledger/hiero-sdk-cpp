// SPDX-License-Identifier: Apache-2.0
#include "ScheduleId.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <limits>
#include <services/basic_types.pb.h>
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
// Constructor Tests
//-----

//-----
TEST_F(ScheduleIdUnitTests, ConstructWithScheduleNum)
{
  // Given / When
  const ScheduleId scheduleId(getTestScheduleNum());

  // Then
  EXPECT_EQ(scheduleId.mShardNum, 0ULL);
  EXPECT_EQ(scheduleId.mRealmNum, 0ULL);
  EXPECT_EQ(scheduleId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, ConstructWithShardRealmScheduleNum)
{
  // Given / When
  const ScheduleId scheduleId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum());

  // Then
  EXPECT_EQ(scheduleId.mShardNum, getTestShardNum());
  EXPECT_EQ(scheduleId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(scheduleId.mScheduleNum, getTestScheduleNum());
}

//-----
// Comparison Tests
//-----

//-----
TEST_F(ScheduleIdUnitTests, CompareScheduleIds)
{
  // Given / When / Then
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
// FromString Tests
//-----

//-----
TEST_F(ScheduleIdUnitTests, FromStringWithValidShardRealmNum)
{
  // Given
  const std::string validId = std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
                              std::to_string(getTestScheduleNum());

  // When
  ScheduleId scheduleId;
  EXPECT_NO_THROW(scheduleId = ScheduleId::fromString(validId));

  // Then
  EXPECT_EQ(scheduleId.mShardNum, getTestShardNum());
  EXPECT_EQ(scheduleId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(scheduleId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, FromStringThrowsWithMissingDelimiters)
{
  // Given
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testScheduleNumStr = std::to_string(getTestScheduleNum());

  // When / Then
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + testRealmNumStr + testScheduleNumStr), std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + '.' + testRealmNumStr + testScheduleNumStr),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + testRealmNumStr + '.' + testScheduleNumStr),
               std::invalid_argument);
}

//-----
TEST_F(ScheduleIdUnitTests, FromStringThrowsWithExtraDelimiters)
{
  // Given
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testScheduleNumStr = std::to_string(getTestScheduleNum());

  // When / Then
  EXPECT_THROW(ScheduleId::fromString('.' + testShardNumStr + testRealmNumStr + testScheduleNumStr),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + testRealmNumStr + testScheduleNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(".." + testShardNumStr + testRealmNumStr + testScheduleNumStr),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + testScheduleNumStr),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString('.' + testShardNumStr + testRealmNumStr + '.' + testScheduleNumStr),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString('.' + testShardNumStr + testRealmNumStr + testScheduleNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + ".." + testRealmNumStr + testScheduleNumStr),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + '.' + testRealmNumStr + testScheduleNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + testRealmNumStr + ".." + testScheduleNumStr),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString(testShardNumStr + testRealmNumStr + '.' + testScheduleNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + '.' + testScheduleNumStr + '.'),
               std::invalid_argument);
}

//-----
TEST_F(ScheduleIdUnitTests, FromStringThrowsWithNonNumericInput)
{
  // Given / When / Then
  EXPECT_THROW(ScheduleId::fromString("abc"), std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString("o.o.e"), std::invalid_argument);
  EXPECT_THROW(ScheduleId::fromString("0.0.1!"), std::invalid_argument);
}

//-----
// Protobuf Serialization Tests
//-----

//-----
TEST_F(ScheduleIdUnitTests, FromProtobuf)
{
  // Given
  proto::ScheduleID protoScheduleId;
  protoScheduleId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoScheduleId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoScheduleId.set_schedulenum(static_cast<int64_t>(getTestScheduleNum()));

  // When
  const ScheduleId scheduleId = ScheduleId::fromProtobuf(protoScheduleId);

  // Then
  EXPECT_EQ(scheduleId.mShardNum, getTestShardNum());
  EXPECT_EQ(scheduleId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(scheduleId.mScheduleNum, getTestScheduleNum());
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
  const ScheduleId scheduleId =
    ScheduleId::fromBytes(internal::Utilities::stringToByteVector(protoScheduleId.SerializeAsString()));

  // Then
  EXPECT_EQ(scheduleId.mShardNum, getTestShardNum());
  EXPECT_EQ(scheduleId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(scheduleId.mScheduleNum, getTestScheduleNum());
}

//-----
TEST_F(ScheduleIdUnitTests, ToProtobuf)
{
  // Given
  const ScheduleId scheduleId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum());

  // When
  const std::unique_ptr<proto::ScheduleID> protoScheduleId = scheduleId.toProtobuf();

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
  const ScheduleId scheduleId = ScheduleId::fromProtobuf(protoScheduleId);

  // When
  const std::vector<std::byte> bytes = scheduleId.toBytes();

  // Then
  EXPECT_EQ(protoBytes, bytes);
}

//-----
// ToString Tests
//-----

//-----
TEST_F(ScheduleIdUnitTests, ToString)
{
  // Given
  const ScheduleId scheduleId(getTestShardNum(), getTestRealmNum(), getTestScheduleNum());

  // When
  std::string scheduleIdStr;
  EXPECT_NO_THROW(scheduleIdStr = scheduleId.toString());

  // Then
  EXPECT_EQ(scheduleIdStr,
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestScheduleNum()));
}
