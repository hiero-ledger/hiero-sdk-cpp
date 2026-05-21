// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookMappingEntries.h"
#include "hooks/EvmHookMappingEntry.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class EvmHookMappingEntriesUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mEvmHookMappingEntry1.setKey(mKey1);
    mEvmHookMappingEntry1.setValue(mValue1);

    mEvmHookMappingEntry2.setPreimage(mPreimage2);
    mEvmHookMappingEntry2.setValue(mValue2);

    mEvmHookMappingEntry3.setKey(mKey3);
    mEvmHookMappingEntry3.setValue(mValue3);

    mEvmHookMappingEntries.push_back(mEvmHookMappingEntry1);
    mEvmHookMappingEntries.push_back(mEvmHookMappingEntry2);
    mEvmHookMappingEntries.push_back(mEvmHookMappingEntry3);
  }

  [[nodiscard]] inline const std::vector<std::byte>& getTestMappingSlot() const { return mMappingSlot; }

  [[nodiscard]] inline const std::vector<std::byte>& getTestKey1() const { return mKey1; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestKey3() const { return mKey3; }

  [[nodiscard]] inline const std::vector<std::byte>& getTestPreimage2() const { return mPreimage2; }

  [[nodiscard]] inline const std::vector<std::byte>& getTestValue1() const { return mValue1; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestValue2() const { return mValue2; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestValue3() const { return mValue3; }

  [[nodiscard]] inline const EvmHookMappingEntry& getTestEvmHookMappingEntry1() const { return mEvmHookMappingEntry1; }
  [[nodiscard]] inline const EvmHookMappingEntry& getTestEvmHookMappingEntry2() const { return mEvmHookMappingEntry2; }
  [[nodiscard]] inline const EvmHookMappingEntry& getTestEvmHookMappingEntry3() const { return mEvmHookMappingEntry3; }

  [[nodiscard]] inline const std::vector<EvmHookMappingEntry>& getTestEvmHookMappingEntries() const
  {
    return mEvmHookMappingEntries;
  }

private:
  const std::vector<std::byte> mMappingSlot = { std::byte(0x17), std::byte(0x19), std::byte(0x1B) };

  const std::vector<std::byte> mKey1 = { std::byte(0x01), std::byte(0x23), std::byte(0x45) };
  const std::vector<std::byte> mKey3 = { std::byte(0x67), std::byte(0x89), std::byte(0xAB) };

  const std::vector<std::byte> mPreimage2 = { std::byte(0xCD), std::byte(0xEF), std::byte(0x02) };

  const std::vector<std::byte> mValue1 = { std::byte(0x04), std::byte(0x06), std::byte(0x08) };
  const std::vector<std::byte> mValue2 = { std::byte(0x0A), std::byte(0x0C), std::byte(0x0E) };
  const std::vector<std::byte> mValue3 = { std::byte(0x11), std::byte(0x13), std::byte(0x15) };

  EvmHookMappingEntry mEvmHookMappingEntry1;
  EvmHookMappingEntry mEvmHookMappingEntry2;
  EvmHookMappingEntry mEvmHookMappingEntry3;

  std::vector<EvmHookMappingEntry> mEvmHookMappingEntries;
};

//-----
TEST_F(EvmHookMappingEntriesUnitTests, GetSetMappingSlot)
{
  // Given
  EvmHookMappingEntries evmHookMappingEntries;

  // When
  EXPECT_NO_THROW(evmHookMappingEntries.setMappingSlot(getTestMappingSlot()));

  // Then
  EXPECT_EQ(evmHookMappingEntries.getMappingSlot(), getTestMappingSlot());
}

//-----
TEST_F(EvmHookMappingEntriesUnitTests, GetSetEntries)
{
  // Given
  EvmHookMappingEntries evmHookMappingEntries;

  // When
  EXPECT_NO_THROW(evmHookMappingEntries.setEntries(getTestEvmHookMappingEntries()));

  // Then
  EXPECT_EQ(evmHookMappingEntries.getEntries().size(), getTestEvmHookMappingEntries().size());
}

//-----
TEST_F(EvmHookMappingEntriesUnitTests, AddEntry)
{
  // Given
  EvmHookMappingEntries evmHookMappingEntries;

  // When
  EXPECT_NO_THROW(evmHookMappingEntries.addEntry(getTestEvmHookMappingEntry1()));

  // Then
  EXPECT_EQ(evmHookMappingEntries.getEntries().size(), 1);
  EXPECT_TRUE(evmHookMappingEntries.getEntries()[0].getKey().has_value());
  EXPECT_EQ(evmHookMappingEntries.getEntries()[0].getKey(), getTestEvmHookMappingEntry1().getKey());
  EXPECT_EQ(evmHookMappingEntries.getEntries()[0].getValue(), getTestEvmHookMappingEntry1().getValue());
}

//-----
TEST_F(EvmHookMappingEntriesUnitTests, ClearEntries)
{
  // Given
  EvmHookMappingEntries evmHookMappingEntries;
  evmHookMappingEntries.setEntries(getTestEvmHookMappingEntries());

  // When
  EXPECT_NO_THROW(evmHookMappingEntries.clearEntries());

  // Then
  EXPECT_TRUE(evmHookMappingEntries.getEntries().empty());
}

//-----
TEST_F(EvmHookMappingEntriesUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::EvmHookMappingEntries protoEvmHookMappingEntries;

  protoEvmHookMappingEntries.set_mapping_slot(internal::Utilities::byteVectorToString(getTestMappingSlot()));
  for (const EvmHookMappingEntry& entry : getTestEvmHookMappingEntries())
  {
    *protoEvmHookMappingEntries.add_entries() = *entry.toProtobuf();
  }

  // When
  const EvmHookMappingEntries evmHookMappingEntries = EvmHookMappingEntries::fromProtobuf(protoEvmHookMappingEntries);

  // Then
  EXPECT_EQ(evmHookMappingEntries.getMappingSlot(), getTestMappingSlot());

  EXPECT_EQ(evmHookMappingEntries.getEntries().size(), getTestEvmHookMappingEntries().size());
}

//-----
TEST_F(EvmHookMappingEntriesUnitTests, ToProtobuf)
{
  // Given
  EvmHookMappingEntries evmHookMappingEntries;

  evmHookMappingEntries.setMappingSlot(getTestMappingSlot());
  evmHookMappingEntries.setEntries(getTestEvmHookMappingEntries());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookMappingEntries> protoEvmHookMappingEntries =
    evmHookMappingEntries.toProtobuf();

  // Then
  EXPECT_EQ(protoEvmHookMappingEntries->mapping_slot(), internal::Utilities::byteVectorToString(getTestMappingSlot()));
  EXPECT_EQ(protoEvmHookMappingEntries->entries_size(), getTestEvmHookMappingEntries().size());
}
