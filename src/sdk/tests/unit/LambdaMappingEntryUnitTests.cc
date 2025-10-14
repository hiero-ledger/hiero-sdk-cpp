// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaMappingEntry.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class LambdaMappingEntryUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::vector<std::byte>& getTestKey() const { return mKey; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestPreimage() const { return mPreimage; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestValue() const { return mValue; }

private:
  const std::vector<std::byte> mKey = { std::byte(0x01), std::byte(0x23), std::byte(0x45) };
  const std::vector<std::byte> mPreimage = { std::byte(0x67), std::byte(0x89), std::byte(0xAB) };
  const std::vector<std::byte> mValue = { std::byte(0xCD), std::byte(0xEF), std::byte(0x02) };
};

//-----
TEST_F(LambdaMappingEntryUnitTests, GetSetKey)
{
  // Given
  LambdaMappingEntry lambdaMappingEntry;

  // When
  EXPECT_NO_THROW(lambdaMappingEntry.setKey(getTestKey()));

  // Then
  EXPECT_TRUE(lambdaMappingEntry.getKey().has_value());
  EXPECT_EQ(lambdaMappingEntry.getKey().value(), getTestKey());
}

//-----
TEST_F(LambdaMappingEntryUnitTests, GetSetKeyResetPreimage)
{
  // Given
  LambdaMappingEntry lambdaMappingEntry;

  // When
  EXPECT_NO_THROW(lambdaMappingEntry.setKey(getTestKey()));
  EXPECT_NO_THROW(lambdaMappingEntry.setPreimage(getTestPreimage()));

  // Then
  EXPECT_FALSE(lambdaMappingEntry.getKey().has_value());
}

//-----
TEST_F(LambdaMappingEntryUnitTests, GetSetPreimage)
{
  // Given
  LambdaMappingEntry lambdaMappingEntry;

  // When
  EXPECT_NO_THROW(lambdaMappingEntry.setPreimage(getTestPreimage()));

  // Then
  EXPECT_TRUE(lambdaMappingEntry.getPreimage().has_value());
  EXPECT_EQ(lambdaMappingEntry.getPreimage().value(), getTestPreimage());
}

//-----
TEST_F(LambdaMappingEntryUnitTests, GetSetPreimateResetsKey)
{
  // Given
  LambdaMappingEntry lambdaMappingEntry;

  // When
  EXPECT_NO_THROW(lambdaMappingEntry.setPreimage(getTestPreimage()));
  EXPECT_NO_THROW(lambdaMappingEntry.setKey(getTestKey()));

  // Then
  EXPECT_FALSE(lambdaMappingEntry.getPreimage().has_value());
}

//-----
TEST_F(LambdaMappingEntryUnitTests, GetSetValue)
{
  // Given
  LambdaMappingEntry lambdaMappingEntry;

  // When
  EXPECT_NO_THROW(lambdaMappingEntry.setValue(getTestValue()));

  // Then
  EXPECT_EQ(lambdaMappingEntry.getValue(), getTestValue());
}

//-----
TEST_F(LambdaMappingEntryUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::LambdaMappingEntry protoLambdaMappingEntryKey;
  com::hedera::hapi::node::hooks::LambdaMappingEntry protoLambdaMappingEntryPreimage;

  protoLambdaMappingEntryKey.set_key(internal::Utilities::byteVectorToString(getTestKey()));
  protoLambdaMappingEntryKey.set_value(internal::Utilities::byteVectorToString(getTestValue()));

  protoLambdaMappingEntryPreimage.set_preimage(internal::Utilities::byteVectorToString(getTestPreimage()));

  // When
  const LambdaMappingEntry lambdaMappingEntryKey = LambdaMappingEntry::fromProtobuf(protoLambdaMappingEntryKey);
  const LambdaMappingEntry lambdaMappingEntryPreimage =
    LambdaMappingEntry::fromProtobuf(protoLambdaMappingEntryPreimage);

  // Then
  EXPECT_TRUE(lambdaMappingEntryKey.getKey().has_value());
  EXPECT_EQ(lambdaMappingEntryKey.getKey().value(), getTestKey());
  EXPECT_EQ(lambdaMappingEntryKey.getValue(), getTestValue());

  EXPECT_TRUE(lambdaMappingEntryPreimage.getPreimage().has_value());
  EXPECT_EQ(lambdaMappingEntryPreimage.getPreimage().value(), getTestPreimage());
}

//-----
TEST_F(LambdaMappingEntryUnitTests, ToProtobuf)
{
  // Given
  LambdaMappingEntry lambdaMappingEntryKey;
  LambdaMappingEntry lambdaMappingEntryPreimage;

  lambdaMappingEntryKey.setKey(getTestKey());
  lambdaMappingEntryKey.setValue(getTestValue());

  lambdaMappingEntryPreimage.setPreimage(getTestPreimage());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntry> protoLambdaMappingEntryKey =
    lambdaMappingEntryKey.toProtobuf();
  const std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntry> protoLambdaMappingEntryPreimage =
    lambdaMappingEntryPreimage.toProtobuf();

  // Then
  EXPECT_TRUE(protoLambdaMappingEntryKey->has_key());
  EXPECT_EQ(protoLambdaMappingEntryKey->key(), internal::Utilities::byteVectorToString(getTestKey()));
  EXPECT_EQ(protoLambdaMappingEntryKey->value(), internal::Utilities::byteVectorToString(getTestValue()));

  EXPECT_TRUE(protoLambdaMappingEntryPreimage->has_preimage());
  EXPECT_EQ(protoLambdaMappingEntryPreimage->preimage(), internal::Utilities::byteVectorToString(getTestPreimage()));
}
