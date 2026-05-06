// SPDX-License-Identifier: Apache-2.0
#include "NftId.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <limits>
#include <services/token_get_nft_info.pb.h>

using namespace Hiero;

class NftIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTokenId; }
  [[nodiscard]] inline const uint64_t& getTestSerialNum() const { return mSerialNum; }

private:
  const TokenId mTokenId = TokenId(10ULL, 200ULL, 3000ULL);
  const uint64_t mSerialNum = 40000ULL;
};

//-----
TEST_F(NftIdUnitTests, ConstructWithTokenIdSerialNum)
{
  // Given / When
  const NftId nftId(getTestTokenId(), getTestSerialNum());

  // Then
  EXPECT_EQ(nftId.mTokenId, getTestTokenId());
  EXPECT_EQ(nftId.mSerialNum, getTestSerialNum());
}

//-----
TEST_F(NftIdUnitTests, CompareNftIds)
{
  // Given / When / Then
  EXPECT_TRUE(NftId() == NftId());
  EXPECT_TRUE(NftId(getTestTokenId(), getTestSerialNum()) == NftId(getTestTokenId(), getTestSerialNum()));
}

//-----
TEST_F(NftIdUnitTests, FromStringWithValidInput)
{
  // Given
  const std::string testTokenIdStr = getTestTokenId().toString();
  const std::string testSerialNumStr = std::to_string(getTestSerialNum());

  // When
  NftId nftId;
  EXPECT_NO_THROW(nftId = NftId::fromString(testTokenIdStr + '/' + testSerialNumStr));

  // Then
  EXPECT_EQ(nftId.mTokenId, getTestTokenId());
  EXPECT_EQ(nftId.mSerialNum, getTestSerialNum());
}

//-----
TEST_F(NftIdUnitTests, FromStringThrowsWithMissingOrExtraDelimiters)
{
  // Given
  const std::string testTokenIdStr = getTestTokenId().toString();
  const std::string testSerialNumStr = std::to_string(getTestSerialNum());

  // When / Then
  EXPECT_THROW(NftId::fromString(testTokenIdStr + testSerialNumStr), std::invalid_argument);
  EXPECT_THROW(NftId::fromString('/' + testTokenIdStr + testSerialNumStr), std::invalid_argument);
  EXPECT_THROW(NftId::fromString(testTokenIdStr + testSerialNumStr + '/'), std::invalid_argument);
  EXPECT_THROW(NftId::fromString("//" + testTokenIdStr + testSerialNumStr), std::invalid_argument);
  EXPECT_THROW(NftId::fromString('/' + testTokenIdStr + '/' + testSerialNumStr), std::invalid_argument);
  EXPECT_THROW(NftId::fromString(testTokenIdStr + '/' + testSerialNumStr + '/'), std::invalid_argument);
  EXPECT_THROW(NftId::fromString(testTokenIdStr + "//" + testSerialNumStr), std::invalid_argument);
}

//-----
TEST_F(NftIdUnitTests, FromStringThrowsWithNonNumericInput)
{
  // Given
  const std::string testTokenIdStr = getTestTokenId().toString();

  // When / Then
  EXPECT_THROW(NftId::fromString(testTokenIdStr + "/abc"), std::invalid_argument);
  EXPECT_THROW(NftId::fromString(testTokenIdStr + "/o.o.e"), std::invalid_argument);
  EXPECT_THROW(NftId::fromString(testTokenIdStr + "/0001!"), std::invalid_argument);
}

//-----
TEST_F(NftIdUnitTests, ProtobufSerializeNftId)
{
  // Given
  const NftId nftId(getTestTokenId(), getTestSerialNum());

  // When
  std::unique_ptr<proto::NftID> protoNftID = nftId.toProtobuf();

  // Then
  EXPECT_EQ(TokenId::fromProtobuf(protoNftID->token_id()), getTestTokenId());
  EXPECT_EQ(protoNftID->serial_number(), getTestSerialNum());
}

//-----
TEST_F(NftIdUnitTests, ProtobufDeserializeNftId)
{
  // Given
  const NftId nftId(getTestTokenId(), getTestSerialNum());
  std::unique_ptr<proto::NftID> protoNftID = nftId.toProtobuf();
  protoNftID->set_serial_number(static_cast<int64_t>(getTestSerialNum() - 1ULL));

  // When
  const NftId deserializedNftId = NftId::fromProtobuf(*protoNftID);

  // Then
  EXPECT_EQ(deserializedNftId.mTokenId, getTestTokenId());
  EXPECT_EQ(deserializedNftId.mSerialNum, getTestSerialNum() - 1ULL);
}

//-----
TEST_F(NftIdUnitTests, ToStringDefaultNftId)
{
  // Given
  const NftId nftId;

  // When / Then
  EXPECT_EQ(nftId.toString(), "0.0.0/0");
}

//-----
TEST_F(NftIdUnitTests, ToStringWithTokenIdAndSerialNum)
{
  // Given
  NftId nftId;
  nftId.mTokenId = getTestTokenId();
  nftId.mSerialNum = getTestSerialNum();

  // When / Then
  EXPECT_EQ(nftId.toString(), getTestTokenId().toString() + '/' + std::to_string(getTestSerialNum()));
}

//-----
TEST_F(NftIdUnitTests, FromBytes)
{
  // Given
  proto::NftID protoNftId;
  protoNftId.set_allocated_token_id(getTestTokenId().toProtobuf().release());
  protoNftId.set_serial_number(static_cast<int64_t>(getTestSerialNum()));

  // When
  const NftId nftId = NftId::fromBytes(internal::Utilities::stringToByteVector(protoNftId.SerializeAsString()));

  // Then
  EXPECT_EQ(nftId.mSerialNum, getTestSerialNum());
  EXPECT_EQ(nftId.mTokenId, getTestTokenId());
}

//-----
TEST_F(NftIdUnitTests, ToBytes)
{
  // Given
  proto::NftID protoNftId;
  protoNftId.set_allocated_token_id(getTestTokenId().toProtobuf().release());
  protoNftId.set_serial_number(static_cast<int64_t>(getTestSerialNum()));

  const std::vector<std::byte> protoBytes = internal::Utilities::stringToByteVector(protoNftId.SerializeAsString());
  const NftId nftId(getTestTokenId(), getTestSerialNum());

  // When
  const std::vector<std::byte> bytes = nftId.toBytes();

  // Then
  EXPECT_EQ(protoBytes, bytes);
}
