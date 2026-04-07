// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookMappingEntry.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class EvmHookMappingEntryUnitTests : public ::testing::Test
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
TEST_F(EvmHookMappingEntryUnitTests, GetSetKey)
{
  // Given
  EvmHookMappingEntry evmHookMappingEntry;

  // When
  EXPECT_NO_THROW(evmHookMappingEntry.setKey(getTestKey()));

  // Then
  EXPECT_TRUE(evmHookMappingEntry.getKey().has_value());
  EXPECT_EQ(evmHookMappingEntry.getKey().value(), getTestKey());
}

//-----
TEST_F(EvmHookMappingEntryUnitTests, GetSetKeyResetPreimage)
{
  // Given
  EvmHookMappingEntry evmHookMappingEntry;

  // When
  EXPECT_NO_THROW(evmHookMappingEntry.setKey(getTestKey()));
  EXPECT_NO_THROW(evmHookMappingEntry.setPreimage(getTestPreimage()));

  // Then
  EXPECT_FALSE(evmHookMappingEntry.getKey().has_value());
}

//-----
TEST_F(EvmHookMappingEntryUnitTests, GetSetPreimage)
{
  // Given
  EvmHookMappingEntry evmHookMappingEntry;

  // When
  EXPECT_NO_THROW(evmHookMappingEntry.setPreimage(getTestPreimage()));

  // Then
  EXPECT_TRUE(evmHookMappingEntry.getPreimage().has_value());
  EXPECT_EQ(evmHookMappingEntry.getPreimage().value(), getTestPreimage());
}

//-----
TEST_F(EvmHookMappingEntryUnitTests, GetSetPreimateResetsKey)
{
  // Given
  EvmHookMappingEntry evmHookMappingEntry;

  // When
  EXPECT_NO_THROW(evmHookMappingEntry.setPreimage(getTestPreimage()));
  EXPECT_NO_THROW(evmHookMappingEntry.setKey(getTestKey()));

  // Then
  EXPECT_FALSE(evmHookMappingEntry.getPreimage().has_value());
}

//-----
TEST_F(EvmHookMappingEntryUnitTests, GetSetValue)
{
  // Given
  EvmHookMappingEntry evmHookMappingEntry;

  // When
  EXPECT_NO_THROW(evmHookMappingEntry.setValue(getTestValue()));

  // Then
  EXPECT_EQ(evmHookMappingEntry.getValue(), getTestValue());
}

//-----
TEST_F(EvmHookMappingEntryUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::EvmHookMappingEntry protoEvmHookMappingEntryKey;
  com::hedera::hapi::node::hooks::EvmHookMappingEntry protoEvmHookMappingEntryPreimage;

  protoEvmHookMappingEntryKey.set_key(internal::Utilities::byteVectorToString(getTestKey()));
  protoEvmHookMappingEntryKey.set_value(internal::Utilities::byteVectorToString(getTestValue()));

  protoEvmHookMappingEntryPreimage.set_preimage(internal::Utilities::byteVectorToString(getTestPreimage()));

  // When
  const EvmHookMappingEntry evmHookMappingEntryKey = EvmHookMappingEntry::fromProtobuf(protoEvmHookMappingEntryKey);
  const EvmHookMappingEntry evmHookMappingEntryPreimage =
    EvmHookMappingEntry::fromProtobuf(protoEvmHookMappingEntryPreimage);

  // Then
  EXPECT_TRUE(evmHookMappingEntryKey.getKey().has_value());
  EXPECT_EQ(evmHookMappingEntryKey.getKey().value(), getTestKey());
  EXPECT_EQ(evmHookMappingEntryKey.getValue(), getTestValue());

  EXPECT_TRUE(evmHookMappingEntryPreimage.getPreimage().has_value());
  EXPECT_EQ(evmHookMappingEntryPreimage.getPreimage().value(), getTestPreimage());
}

//-----
TEST_F(EvmHookMappingEntryUnitTests, ToProtobuf)
{
  // Given
  EvmHookMappingEntry evmHookMappingEntryKey;
  EvmHookMappingEntry evmHookMappingEntryPreimage;

  evmHookMappingEntryKey.setKey(getTestKey());
  evmHookMappingEntryKey.setValue(getTestValue());

  evmHookMappingEntryPreimage.setPreimage(getTestPreimage());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookMappingEntry> protoEvmHookMappingEntryKey =
    evmHookMappingEntryKey.toProtobuf();
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookMappingEntry> protoEvmHookMappingEntryPreimage =
    evmHookMappingEntryPreimage.toProtobuf();

  // Then
  EXPECT_TRUE(protoEvmHookMappingEntryKey->has_key());
  EXPECT_EQ(protoEvmHookMappingEntryKey->key(), internal::Utilities::byteVectorToString(getTestKey()));
  EXPECT_EQ(protoEvmHookMappingEntryKey->value(), internal::Utilities::byteVectorToString(getTestValue()));

  EXPECT_TRUE(protoEvmHookMappingEntryPreimage->has_preimage());
  EXPECT_EQ(protoEvmHookMappingEntryPreimage->preimage(), internal::Utilities::byteVectorToString(getTestPreimage()));
}
