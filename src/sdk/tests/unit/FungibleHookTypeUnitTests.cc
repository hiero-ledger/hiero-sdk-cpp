// SPDX-License-Identifier: Apache-2.0
#include "hooks/FungibleHookType.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FungibleHookTypeUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline int getTestEnumValue0() const { return mEnumValue0; }
  [[nodiscard]] inline int getTestEnumValue1() const { return mEnumValue1; }
  [[nodiscard]] inline int getTestEnumValue2() const { return mEnumValue2; }
  [[nodiscard]] inline const std::string& getTestStringPreTx() const { return mStringPreTx; }
  [[nodiscard]] inline const std::string& getTestStringPrePostTx() const { return mStringPrePostTx; }
  [[nodiscard]] inline const std::string& getTestStringUninitialized() const { return mStringUninitialized; }

private:
  const int mEnumValue0 = 0;
  const int mEnumValue1 = 1;
  const int mEnumValue2 = 2;
  const std::string mStringPreTx = "PRE_TX_ALLOWANCE_HOOK";
  const std::string mStringPrePostTx = "PRE_POST_TX_ALLOWANCE_HOOK";
  const std::string mStringUninitialized = "UNINITIALIZED";
};

//-----
TEST_F(FungibleHookTypeUnitTests, EnumValues)
{
  // Then
  EXPECT_EQ(static_cast<int>(FungibleHookType::PRE_TX_ALLOWANCE_HOOK), getTestEnumValue0());
  EXPECT_EQ(static_cast<int>(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK), getTestEnumValue1());
  EXPECT_EQ(static_cast<int>(FungibleHookType::UNINITIALIZED), getTestEnumValue2());
}

//-----
TEST_F(FungibleHookTypeUnitTests, StringMapping)
{
  // Then
  EXPECT_EQ(gFungibleHookTypeToString.at(FungibleHookType::PRE_TX_ALLOWANCE_HOOK), getTestStringPreTx());
  EXPECT_EQ(gFungibleHookTypeToString.at(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK), getTestStringPrePostTx());
  EXPECT_EQ(gFungibleHookTypeToString.at(FungibleHookType::UNINITIALIZED), getTestStringUninitialized());
}

//-----
TEST_F(FungibleHookTypeUnitTests, StringMappingCompleteness)
{
  // Then - verify all enum values have string representations
  EXPECT_EQ(gFungibleHookTypeToString.size(), 3);

  // Verify all enum values are present
  EXPECT_TRUE(gFungibleHookTypeToString.find(FungibleHookType::PRE_TX_ALLOWANCE_HOOK) !=
              gFungibleHookTypeToString.end());
  EXPECT_TRUE(gFungibleHookTypeToString.find(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK) !=
              gFungibleHookTypeToString.end());
  EXPECT_TRUE(gFungibleHookTypeToString.find(FungibleHookType::UNINITIALIZED) != gFungibleHookTypeToString.end());
}
