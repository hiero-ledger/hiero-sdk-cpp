// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaMappingEntries.h"
#include "hooks/LambdaMappingEntry.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class LambdaMappingEntriesUnitTests : public ::testing::Test
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

    mLambdaMappingEntries.push_back(mLambdaMappingEntry1);
    mLambdaMappingEntries.push_back(mLambdaMappingEntry2);
    mLambdaMappingEntries.push_back(mLambdaMappingEntry3);
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

  [[nodiscard]] inline const std::vector<LambdaMappingEntry>& getTestLambdaMappingEntries() const
  {
    return mLambdaMappingEntries;
  }

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

  std::vector<LambdaMappingEntry> mLambdaMappingEntries;
};

//-----
TEST_F(LambdaMappingEntriesUnitTests, GetSetMappingSlot)
{
  // Given
  LambdaMappingEntries lambdaMappingEntries;

  // When
  EXPECT_NO_THROW(lambdaMappingEntries.setMappingSlot(getTestMappingSlot()));

  // Then
  EXPECT_EQ(lambdaMappingEntries.getMappingSlot(), getTestMappingSlot());
}

//-----
TEST_F(LambdaMappingEntriesUnitTests, GetSetEntries)
{
  // Given
  LambdaMappingEntries lambdaMappingEntries;

  // When
  EXPECT_NO_THROW(lambdaMappingEntries.setEntries(getTestLambdaMappingEntries()));

  // Then
  EXPECT_EQ(lambdaMappingEntries.getEntries().size(), getTestLambdaMappingEntries().size());
}

//-----
TEST_F(LambdaMappingEntriesUnitTests, AddEntry)
{
  // Given
  LambdaMappingEntries lambdaMappingEntries;

  // When
  EXPECT_NO_THROW(lambdaMappingEntries.addEntry(getTestLambdaMappingEntry1()));

  // Then
  EXPECT_EQ(lambdaMappingEntries.getEntries().size(), 1);
  EXPECT_TRUE(lambdaMappingEntries.getEntries()[0].getKey().has_value());
  EXPECT_EQ(lambdaMappingEntries.getEntries()[0].getKey(), getTestLambdaMappingEntry1().getKey());
  EXPECT_EQ(lambdaMappingEntries.getEntries()[0].getValue(), getTestLambdaMappingEntry1().getValue());
}

//-----
TEST_F(LambdaMappingEntriesUnitTests, ClearEntries)
{
  // Given
  LambdaMappingEntries lambdaMappingEntries;
  lambdaMappingEntries.setEntries(getTestLambdaMappingEntries());

  // When
  EXPECT_NO_THROW(lambdaMappingEntries.clearEntries());

  // Then
  EXPECT_TRUE(lambdaMappingEntries.getEntries().empty());
}

//-----
TEST_F(LambdaMappingEntriesUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::LambdaMappingEntries protoLambdaMappingEntries;

  protoLambdaMappingEntries.set_mapping_slot(internal::Utilities::byteVectorToString(getTestMappingSlot()));
  for (const LambdaMappingEntry& entry : getTestLambdaMappingEntries())
  {
    *protoLambdaMappingEntries.add_entries() = *entry.toProtobuf();
  }

  // When
  const LambdaMappingEntries lambdaMappingEntries = LambdaMappingEntries::fromProtobuf(protoLambdaMappingEntries);

  // Then
  EXPECT_EQ(lambdaMappingEntries.getMappingSlot(), getTestMappingSlot());

  EXPECT_EQ(lambdaMappingEntries.getEntries().size(), getTestLambdaMappingEntries().size());
}

//-----
TEST_F(LambdaMappingEntriesUnitTests, ToProtobuf)
{
  // Given
  LambdaMappingEntries lambdaMappingEntries;

  lambdaMappingEntries.setMappingSlot(getTestMappingSlot());
  lambdaMappingEntries.setEntries(getTestLambdaMappingEntries());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::LambdaMappingEntries> protoLambdaMappingEntries =
    lambdaMappingEntries.toProtobuf();

  // Then
  EXPECT_EQ(protoLambdaMappingEntries->mapping_slot(), internal::Utilities::byteVectorToString(getTestMappingSlot()));
  EXPECT_EQ(protoLambdaMappingEntries->entries_size(), getTestLambdaMappingEntries().size());
}
