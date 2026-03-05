// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookStorageUpdate.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class EvmHookStorageUpdateUnitTests : public ::testing::Test
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

    mEvmHookMappingEntries.setEntries({ mEvmHookMappingEntry1, mEvmHookMappingEntry2, mEvmHookMappingEntry3 });
    mEvmHookMappingEntries.setMappingSlot(mMappingSlot);

    mEvmHookStorageSlot.setKey(mKey1);
    mEvmHookStorageSlot.setValue(mValue1);
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

  [[nodiscard]] inline const EvmHookMappingEntries& getTestEvmHookMappingEntries() const { return mEvmHookMappingEntries; }

  [[nodiscard]] inline const EvmHookStorageSlot& getTestEvmHookStorageSlot() const { return mEvmHookStorageSlot; }

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

  EvmHookMappingEntries mEvmHookMappingEntries;

  EvmHookStorageSlot mEvmHookStorageSlot;
};

//-----
TEST_F(EvmHookStorageUpdateUnitTests, GetSetStorageSlot)
{
  // Given
  EvmHookStorageUpdate evmHookStorageUpdate;

  // When
  EXPECT_NO_THROW(evmHookStorageUpdate.setStorageSlot(getTestEvmHookStorageSlot()));

  // Then
  EXPECT_TRUE(evmHookStorageUpdate.getStorageSlot().has_value());
  EXPECT_EQ(evmHookStorageUpdate.getStorageSlot().value().getKey(), getTestKey1());
  EXPECT_EQ(evmHookStorageUpdate.getStorageSlot().value().getValue(), getTestValue1());
}

//-----
TEST_F(EvmHookStorageUpdateUnitTests, GetSetMappingEntries)
{
  // Given
  EvmHookStorageUpdate evmHookStorageUpdate;

  // When
  EXPECT_NO_THROW(evmHookStorageUpdate.setMappingEntries(getTestEvmHookMappingEntries()));

  // Then
  EXPECT_TRUE(evmHookStorageUpdate.getMappingEntries().has_value());
  EXPECT_EQ(evmHookStorageUpdate.getMappingEntries()->getEntries().size(),
            getTestEvmHookMappingEntries().getEntries().size());
}

//-----
TEST_F(EvmHookStorageUpdateUnitTests, SetStorageSlotResetsMappingEntries)
{
  // Given
  EvmHookStorageUpdate evmHookStorageUpdate;

  // When
  EXPECT_NO_THROW(evmHookStorageUpdate.setMappingEntries(getTestEvmHookMappingEntries()));
  EXPECT_NO_THROW(evmHookStorageUpdate.setStorageSlot(getTestEvmHookStorageSlot()));

  // Then
  EXPECT_FALSE(evmHookStorageUpdate.getMappingEntries().has_value());
}

//-----
TEST_F(EvmHookStorageUpdateUnitTests, SetMappingEntriesResetsStorageSlot)
{
  // Given
  EvmHookStorageUpdate evmHookStorageUpdate;

  // When
  EXPECT_NO_THROW(evmHookStorageUpdate.setStorageSlot(getTestEvmHookStorageSlot()));
  EXPECT_NO_THROW(evmHookStorageUpdate.setMappingEntries(getTestEvmHookMappingEntries()));

  // Then
  EXPECT_FALSE(evmHookStorageUpdate.getStorageSlot().has_value());
}

//-----
TEST_F(EvmHookStorageUpdateUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::EvmHookStorageUpdate protoEvmHookStorageUpdateStorageSlot;
  com::hedera::hapi::node::hooks::EvmHookStorageUpdate protoEvmHookStorageUpdateMappingEntries;

  protoEvmHookStorageUpdateStorageSlot.set_allocated_storage_slot(getTestEvmHookStorageSlot().toProtobuf().release());
  protoEvmHookStorageUpdateMappingEntries.set_allocated_mapping_entries(
    getTestEvmHookMappingEntries().toProtobuf().release());

  // When
  const EvmHookStorageUpdate evmHookStorageUpdateStorageSlot =
    EvmHookStorageUpdate::fromProtobuf(protoEvmHookStorageUpdateStorageSlot);
  const EvmHookStorageUpdate evmHookStorageUpdateMappingEntries =
    EvmHookStorageUpdate::fromProtobuf(protoEvmHookStorageUpdateMappingEntries);

  // Then
  EXPECT_TRUE(evmHookStorageUpdateStorageSlot.getStorageSlot().has_value());
  EXPECT_EQ(evmHookStorageUpdateStorageSlot.getStorageSlot()->getKey(), getTestEvmHookStorageSlot().getKey());
  EXPECT_EQ(evmHookStorageUpdateStorageSlot.getStorageSlot()->getValue(), getTestEvmHookStorageSlot().getValue());

  EXPECT_TRUE(evmHookStorageUpdateMappingEntries.getMappingEntries().has_value());
  EXPECT_EQ(evmHookStorageUpdateMappingEntries.getMappingEntries()->getEntries().size(),
            getTestEvmHookMappingEntries().getEntries().size());
}

//-----
TEST_F(EvmHookStorageUpdateUnitTests, ToProtobuf)
{
  // Given
  EvmHookStorageUpdate evmHookStorageUpdateStorageSlot;
  EvmHookStorageUpdate evmHookStorageUpdateMappingEntries;

  evmHookStorageUpdateStorageSlot.setStorageSlot(getTestEvmHookStorageSlot());
  evmHookStorageUpdateMappingEntries.setMappingEntries(getTestEvmHookMappingEntries());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookStorageUpdate> protoEvmHookStorageUpdateStorageSlot =
    evmHookStorageUpdateStorageSlot.toProtobuf();
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookStorageUpdate> protoEvmHookStorageUpdateMappingEntries =
    evmHookStorageUpdateMappingEntries.toProtobuf();

  // Then
  EXPECT_TRUE(protoEvmHookStorageUpdateStorageSlot->has_storage_slot());
  EXPECT_EQ(protoEvmHookStorageUpdateStorageSlot->storage_slot().key(),
            internal::Utilities::byteVectorToString(getTestKey1()));
  EXPECT_EQ(protoEvmHookStorageUpdateStorageSlot->storage_slot().value(),
            internal::Utilities::byteVectorToString(getTestValue1()));

  EXPECT_TRUE(protoEvmHookStorageUpdateMappingEntries->has_mapping_entries());
  EXPECT_EQ(protoEvmHookStorageUpdateMappingEntries->mapping_entries().mapping_slot(),
            internal::Utilities::byteVectorToString(getTestMappingSlot()));
  EXPECT_EQ(protoEvmHookStorageUpdateMappingEntries->mapping_entries().entries_size(),
            getTestEvmHookMappingEntries().getEntries().size());
}
