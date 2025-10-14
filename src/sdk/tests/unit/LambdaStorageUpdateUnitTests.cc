// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaStorageUpdate.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class LambdaStorageUpdateUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mLambdaMappingEntry1.setKey(mKey1);
    mLambdaMappingEntry1.setValue(mValue1);

    mLambdaMappingEntry2.setPreimage(mPreimage2);
    mLambdaMappingEntry2.setValue(mValue2);

    mLambdaMappingEntry3.setKey(mKey3);
    mLambdaMappingEntry3.setValue(mValue3);

    mLambdaMappingEntries.setEntries({ mLambdaMappingEntry1, mLambdaMappingEntry2, mLambdaMappingEntry3 });
    mLambdaMappingEntries.setMappingSlot(mMappingSlot);

    mLambdaStorageSlot.setKey(mKey1);
    mLambdaStorageSlot.setValue(mValue1);
  }

  [[nodiscard]] inline const std::vector<std::byte>& getTestMappingSlot() const { return mMappingSlot; }

  [[nodiscard]] inline const std::vector<std::byte>& getTestKey1() const { return mKey1; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestKey3() const { return mKey3; }

  [[nodiscard]] inline const std::vector<std::byte>& getTestPreimage2() const { return mPreimage2; }

  [[nodiscard]] inline const std::vector<std::byte>& getTestValue1() const { return mValue1; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestValue2() const { return mValue2; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestValue3() const { return mValue3; }

  [[nodiscard]] inline const LambdaMappingEntry& getTestLambdaMappingEntry1() const { return mLambdaMappingEntry1; }
  [[nodiscard]] inline const LambdaMappingEntry& getTestLambdaMappingEntry2() const { return mLambdaMappingEntry2; }
  [[nodiscard]] inline const LambdaMappingEntry& getTestLambdaMappingEntry3() const { return mLambdaMappingEntry3; }

  [[nodiscard]] inline const LambdaMappingEntries& getTestLambdaMappingEntries() const { return mLambdaMappingEntries; }

  [[nodiscard]] inline const LambdaStorageSlot& getTestLambdaStorageSlot() const { return mLambdaStorageSlot; }

private:
  const std::vector<std::byte> mMappingSlot = { std::byte(0x17), std::byte(0x19), std::byte(0x1B) };

  const std::vector<std::byte> mKey1 = { std::byte(0x01), std::byte(0x23), std::byte(0x45) };
  const std::vector<std::byte> mKey3 = { std::byte(0x67), std::byte(0x89), std::byte(0xAB) };

  const std::vector<std::byte> mPreimage2 = { std::byte(0xCD), std::byte(0xEF), std::byte(0x02) };

  const std::vector<std::byte> mValue1 = { std::byte(0x04), std::byte(0x06), std::byte(0x08) };
  const std::vector<std::byte> mValue2 = { std::byte(0x0A), std::byte(0x0C), std::byte(0x0E) };
  const std::vector<std::byte> mValue3 = { std::byte(0x11), std::byte(0x13), std::byte(0x15) };

  LambdaMappingEntry mLambdaMappingEntry1;
  LambdaMappingEntry mLambdaMappingEntry2;
  LambdaMappingEntry mLambdaMappingEntry3;

  LambdaMappingEntries mLambdaMappingEntries;

  LambdaStorageSlot mLambdaStorageSlot;
};

//-----
TEST_F(LambdaStorageUpdateUnitTests, GetSetStorageSlot)
{
  // Given
  LambdaStorageUpdate lambdaStorageUpdate;

  // When
  EXPECT_NO_THROW(lambdaStorageUpdate.setStorageSlot(getTestLambdaStorageSlot()));

  // Then
  EXPECT_TRUE(lambdaStorageUpdate.getStorageSlot().has_value());
  EXPECT_EQ(lambdaStorageUpdate.getStorageSlot().value().getKey(), getTestKey1());
  EXPECT_EQ(lambdaStorageUpdate.getStorageSlot().value().getValue(), getTestValue1());
}

//-----
TEST_F(LambdaStorageUpdateUnitTests, GetSetMappingEntries)
{
  // Given
  LambdaStorageUpdate lambdaStorageUpdate;

  // When
  EXPECT_NO_THROW(lambdaStorageUpdate.setMappingEntries(getTestLambdaMappingEntries()));

  // Then
  EXPECT_TRUE(lambdaStorageUpdate.getMappingEntries().has_value());
  EXPECT_EQ(lambdaStorageUpdate.getMappingEntries()->getEntries().size(),
            getTestLambdaMappingEntries().getEntries().size());
}

//-----
TEST_F(LambdaStorageUpdateUnitTests, SetStorageSlotResetsMappingEntries)
{
  // Given
  LambdaStorageUpdate lambdaStorageUpdate;

  // When
  EXPECT_NO_THROW(lambdaStorageUpdate.setMappingEntries(getTestLambdaMappingEntries()));
  EXPECT_NO_THROW(lambdaStorageUpdate.setStorageSlot(getTestLambdaStorageSlot()));

  // Then
  EXPECT_FALSE(lambdaStorageUpdate.getMappingEntries().has_value());
}

//-----
TEST_F(LambdaStorageUpdateUnitTests, SetMappingEntriesResetsStorageSlot)
{
  // Given
  LambdaStorageUpdate lambdaStorageUpdate;

  // When
  EXPECT_NO_THROW(lambdaStorageUpdate.setStorageSlot(getTestLambdaStorageSlot()));
  EXPECT_NO_THROW(lambdaStorageUpdate.setMappingEntries(getTestLambdaMappingEntries()));

  // Then
  EXPECT_FALSE(lambdaStorageUpdate.getStorageSlot().has_value());
}

//-----
TEST_F(LambdaStorageUpdateUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::LambdaStorageUpdate protoLambdaStorageUpdateStorageSlot;
  com::hedera::hapi::node::hooks::LambdaStorageUpdate protoLambdaStorageUpdateMappingEntries;

  protoLambdaStorageUpdateStorageSlot.set_allocated_storage_slot(getTestLambdaStorageSlot().toProtobuf().release());
  protoLambdaStorageUpdateMappingEntries.set_allocated_mapping_entries(
    getTestLambdaMappingEntries().toProtobuf().release());

  // When
  const LambdaStorageUpdate lambdaStorageUpdateStorageSlot =
    LambdaStorageUpdate::fromProtobuf(protoLambdaStorageUpdateStorageSlot);
  const LambdaStorageUpdate lambdaStorageUpdateMappingEntries =
    LambdaStorageUpdate::fromProtobuf(protoLambdaStorageUpdateMappingEntries);

  // Then
  EXPECT_TRUE(lambdaStorageUpdateStorageSlot.getStorageSlot().has_value());
  EXPECT_EQ(lambdaStorageUpdateStorageSlot.getStorageSlot()->getKey(), getTestLambdaStorageSlot().getKey());
  EXPECT_EQ(lambdaStorageUpdateStorageSlot.getStorageSlot()->getValue(), getTestLambdaStorageSlot().getValue());

  EXPECT_TRUE(lambdaStorageUpdateMappingEntries.getMappingEntries().has_value());
  EXPECT_EQ(lambdaStorageUpdateMappingEntries.getMappingEntries()->getEntries().size(),
            getTestLambdaMappingEntries().getEntries().size());
}

//-----
TEST_F(LambdaStorageUpdateUnitTests, ToProtobuf)
{
  // Given
  LambdaStorageUpdate lambdaStorageUpdateStorageSlot;
  LambdaStorageUpdate lambdaStorageUpdateMappingEntries;

  lambdaStorageUpdateStorageSlot.setStorageSlot(getTestLambdaStorageSlot());
  lambdaStorageUpdateMappingEntries.setMappingEntries(getTestLambdaMappingEntries());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::LambdaStorageUpdate> protoLambdaStorageUpdateStorageSlot =
    lambdaStorageUpdateStorageSlot.toProtobuf();
  const std::unique_ptr<com::hedera::hapi::node::hooks::LambdaStorageUpdate> protoLambdaStorageUpdateMappingEntries =
    lambdaStorageUpdateMappingEntries.toProtobuf();

  // Then
  EXPECT_TRUE(protoLambdaStorageUpdateStorageSlot->has_storage_slot());
  EXPECT_EQ(protoLambdaStorageUpdateStorageSlot->storage_slot().key(),
            internal::Utilities::byteVectorToString(getTestKey1()));
  EXPECT_EQ(protoLambdaStorageUpdateStorageSlot->storage_slot().value(),
            internal::Utilities::byteVectorToString(getTestValue1()));

  EXPECT_TRUE(protoLambdaStorageUpdateMappingEntries->has_mapping_entries());
  EXPECT_EQ(protoLambdaStorageUpdateMappingEntries->mapping_entries().mapping_slot(),
            internal::Utilities::byteVectorToString(getTestMappingSlot()));
  EXPECT_EQ(protoLambdaStorageUpdateMappingEntries->mapping_entries().entries_size(),
            getTestLambdaMappingEntries().getEntries().size());
}
