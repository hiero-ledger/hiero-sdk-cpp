// SPDX-License-Identifier: Apache-2.0
#include "hooks/NftHookType.h"

#include <gtest/gtest.h>

using namespace Hiero;

class NftHookTypeUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline int getTestEnumValue0() const { return mEnumValue0; }
  [[nodiscard]] inline int getTestEnumValue1() const { return mEnumValue1; }
  [[nodiscard]] inline int getTestEnumValue2() const { return mEnumValue2; }
  [[nodiscard]] inline const std::string& getTestStringPreHook() const { return mStringPreHook; }
  [[nodiscard]] inline const std::string& getTestStringPrePostHook() const { return mStringPrePostHook; }
  [[nodiscard]] inline const std::string& getTestStringUninitialized() const { return mStringUninitialized; }

private:
  const int mEnumValue0 = 0;
  const int mEnumValue1 = 1;
  const int mEnumValue2 = 2;
  const std::string mStringPreHook = "PRE_HOOK";
  const std::string mStringPrePostHook = "PRE_POST_HOOK";
  const std::string mStringUninitialized = "UNINITIALIZED";
};

//-----
TEST_F(NftHookTypeUnitTests, EnumValues)
{
  // Then
  EXPECT_EQ(static_cast<int>(NftHookType::PRE_HOOK), getTestEnumValue0());
  EXPECT_EQ(static_cast<int>(NftHookType::PRE_POST_HOOK), getTestEnumValue1());
  EXPECT_EQ(static_cast<int>(NftHookType::UNINITIALIZED), getTestEnumValue2());
}

//-----
TEST_F(NftHookTypeUnitTests, StringMapping)
{
  // Then
  EXPECT_EQ(gNftHookTypeToString.at(NftHookType::PRE_HOOK), getTestStringPreHook());
  EXPECT_EQ(gNftHookTypeToString.at(NftHookType::PRE_POST_HOOK), getTestStringPrePostHook());
  EXPECT_EQ(gNftHookTypeToString.at(NftHookType::UNINITIALIZED), getTestStringUninitialized());
}

//-----
TEST_F(NftHookTypeUnitTests, StringMappingCompleteness)
{
  // Then - verify all enum values have string representations
  EXPECT_EQ(gNftHookTypeToString.size(), 3);

  // Verify all enum values are present
  EXPECT_TRUE(gNftHookTypeToString.find(NftHookType::PRE_HOOK) != gNftHookTypeToString.end());
  EXPECT_TRUE(gNftHookTypeToString.find(NftHookType::PRE_POST_HOOK) != gNftHookTypeToString.end());
  EXPECT_TRUE(gNftHookTypeToString.find(NftHookType::UNINITIALIZED) != gNftHookTypeToString.end());
}
