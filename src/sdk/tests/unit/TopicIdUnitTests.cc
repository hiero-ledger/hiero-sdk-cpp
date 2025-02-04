// SPDX-License-Identifier: Apache-2.0
#include "TopicId.h"
#include "exceptions/IllegalStateException.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

using namespace Hiero;

class TopicIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const uint64_t& getTestShardNum() const { return mTestShardNum; }
  [[nodiscard]] inline const uint64_t& getTestRealmNum() const { return mTestRealmNum; }
  [[nodiscard]] inline const uint64_t& getTestTopicNum() const { return mTestTopicNum; }

private:
  const uint64_t mTestShardNum = 8ULL;
  const uint64_t mTestRealmNum = 90ULL;
  const uint64_t mTestTopicNum = 1000ULL;
};

//-----
TEST_F(TopicIdUnitTests, ConstructWithTopicNum)
{
  // Given / When
  const TopicId topicId(getTestTopicNum());

  // Then
  EXPECT_EQ(topicId.mShardNum, 0ULL);
  EXPECT_EQ(topicId.mRealmNum, 0ULL);
  EXPECT_EQ(topicId.mTopicNum, getTestTopicNum());
}

//-----
TEST_F(TopicIdUnitTests, ConstructWithShardRealmTopicNum)
{
  // Given / When
  const TopicId topicId(getTestShardNum(), getTestRealmNum(), getTestTopicNum());

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mTopicNum, getTestTopicNum());
}

//-----
TEST_F(TopicIdUnitTests, CompareTopicIds)
{
  EXPECT_TRUE(TopicId() == TopicId());
  EXPECT_TRUE(TopicId(getTestTopicNum()) == TopicId(getTestTopicNum()));
  EXPECT_TRUE(TopicId(getTestShardNum(), getTestRealmNum(), getTestTopicNum()) ==
              TopicId(getTestShardNum(), getTestRealmNum(), getTestTopicNum()));

  EXPECT_FALSE(TopicId(getTestTopicNum()) == TopicId(getTestTopicNum() - 1ULL));
  EXPECT_FALSE(TopicId(getTestShardNum(), getTestRealmNum(), getTestTopicNum()) ==
               TopicId(getTestShardNum() - 1ULL, getTestRealmNum(), getTestTopicNum()));
  EXPECT_FALSE(TopicId(getTestShardNum(), getTestRealmNum(), getTestTopicNum()) ==
               TopicId(getTestShardNum(), getTestRealmNum() - 1ULL, getTestTopicNum()));
}

//-----
TEST_F(TopicIdUnitTests, FromString)
{
  // Given
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testAccountNumStr = std::to_string(getTestTopicNum());

  // When
  TopicId topicId;
  EXPECT_NO_THROW(topicId = TopicId::fromString(testShardNumStr + '.' + testRealmNumStr + '.' + testAccountNumStr));

  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString('.' + testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + '.' + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(".." + testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString('.' + testShardNumStr + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString('.' + testShardNumStr + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + ".." + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + '.' + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + testRealmNumStr + ".." + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString(testShardNumStr + testRealmNumStr + '.' + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(topicId =
                 TopicId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + '.' + testAccountNumStr + '.'),
               std::invalid_argument);

  EXPECT_THROW(topicId = TopicId::fromString("abc"), std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString("o.o.e"), std::invalid_argument);
  EXPECT_THROW(topicId = TopicId::fromString("0.0.1!"), std::invalid_argument);

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mTopicNum, getTestTopicNum());
}

//-----
TEST_F(TopicIdUnitTests, FromSolidityAddress)
{
  // Given
  const std::string goodAddr = "0123456789ABCDEF0123456789ABCDEF01234567";
  const std::string goodAddrWithPrefix = "0x0123456789ABCDEF0123456789ABCDEF01234567";
  const std::string addrTooBig = "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";
  const std::string addrTooSmall = "0123456789ABCDEF";
  const std::string addrNotHex = "This is a 40 character non-hex string!!!";

  // When
  TopicId topicIdFromGoodAddr;
  TopicId topicIdFromGoodAddrWithPrefix;
  EXPECT_NO_THROW(topicIdFromGoodAddr = TopicId::fromSolidityAddress(goodAddr));
  EXPECT_NO_THROW(topicIdFromGoodAddrWithPrefix = TopicId::fromSolidityAddress(goodAddrWithPrefix));

  EXPECT_THROW(const TopicId topicIdFromAddrTooBig = TopicId::fromSolidityAddress(addrTooBig), std::invalid_argument);
  EXPECT_THROW(const TopicId topicIdFromAddrTooSmall = TopicId::fromSolidityAddress(addrTooSmall),
               std::invalid_argument);
  EXPECT_THROW(const TopicId topicIdFromAddrNotHex = TopicId::fromSolidityAddress(addrNotHex), std::invalid_argument);

  // Then
  EXPECT_EQ(topicIdFromGoodAddr.mShardNum,
            static_cast<uint64_t>(
              *internal::Utilities::toTypePtr<uint32_t>(internal::HexConverter::hexToBytes(goodAddr).data())));
  EXPECT_EQ(
    topicIdFromGoodAddr.mRealmNum,
    *internal::Utilities::toTypePtr<uint64_t>(internal::HexConverter::hexToBytes(goodAddr).data() + sizeof(uint32_t)));
  EXPECT_EQ(topicIdFromGoodAddr.mTopicNum,
            *internal::Utilities::toTypePtr<uint64_t>(internal::HexConverter::hexToBytes(goodAddr).data() +
                                                      sizeof(uint32_t) + sizeof(uint64_t)));

  EXPECT_EQ(topicIdFromGoodAddr.mShardNum, topicIdFromGoodAddrWithPrefix.mShardNum);
  EXPECT_EQ(topicIdFromGoodAddr.mRealmNum, topicIdFromGoodAddrWithPrefix.mRealmNum);
  EXPECT_EQ(topicIdFromGoodAddr.mTopicNum, topicIdFromGoodAddrWithPrefix.mTopicNum);
}

//-----
TEST_F(TopicIdUnitTests, FromProtobuf)
{
  // Given
  proto::TopicID protoTopicId;
  protoTopicId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoTopicId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoTopicId.set_topicnum(static_cast<int64_t>(getTestTopicNum()));

  // When
  const TopicId topicId = TopicId::fromProtobuf(protoTopicId);

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mTopicNum, getTestTopicNum());
}

//-----
TEST_F(TopicIdUnitTests, FromBytes)
{
  // Given
  proto::TopicID protoTopicId;
  protoTopicId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoTopicId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoTopicId.set_topicnum(static_cast<int64_t>(getTestTopicNum()));

  // When
  const TopicId topicId = TopicId::fromBytes(internal::Utilities::stringToByteVector(protoTopicId.SerializeAsString()));

  // Then
  EXPECT_EQ(topicId.mShardNum, getTestShardNum());
  EXPECT_EQ(topicId.mRealmNum, getTestRealmNum());
  EXPECT_EQ(topicId.mTopicNum, getTestTopicNum());
}

//-----
TEST_F(TopicIdUnitTests, ToProtobuf)
{
  // Given
  const TopicId topicId(getTestShardNum(), getTestRealmNum(), getTestTopicNum());

  // When
  const std::unique_ptr<proto::TopicID> protoTopicId = topicId.toProtobuf();

  // Then
  EXPECT_EQ(protoTopicId->shardnum(), getTestShardNum());
  EXPECT_EQ(protoTopicId->realmnum(), getTestRealmNum());
  EXPECT_EQ(protoTopicId->topicnum(), getTestTopicNum());
}

//-----
TEST_F(TopicIdUnitTests, ToBytes)
{
  // Given
  proto::TopicID protoTopicId;
  protoTopicId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoTopicId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoTopicId.set_topicnum(static_cast<int64_t>(getTestTopicNum()));

  const std::vector<std::byte> protoBytes = internal::Utilities::stringToByteVector(protoTopicId.SerializeAsString());
  const TopicId topicId = TopicId::fromProtobuf(protoTopicId);

  // When
  const std::vector<std::byte> bytes = topicId.toBytes();

  // Then
  EXPECT_EQ(protoBytes, bytes);
}

//-----
TEST_F(TopicIdUnitTests, ToString)
{
  // Given
  const TopicId topicId(getTestShardNum(), getTestRealmNum(), getTestTopicNum());

  // When
  std::string topicIdStr;
  EXPECT_NO_THROW(topicIdStr = topicId.toString());

  // Then
  EXPECT_EQ(topicIdStr,
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestTopicNum()));
}
