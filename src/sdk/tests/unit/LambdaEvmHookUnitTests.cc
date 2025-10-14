// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaEvmHook.h"
#include "hooks/LambdaMappingEntries.h"
#include "hooks/LambdaMappingEntry.h"
#include "hooks/LambdaStorageSlot.h"
#include "hooks/LambdaStorageUpdate.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <services/hook_types.pb.h>
#include <vector>

using namespace Hiero;

class LambdaEvmHookUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    const std::vector<std::byte> mKey1 = { std::byte(0x01), std::byte(0x23), std::byte(0x45) };
    const std::vector<std::byte> mKey2 = { std::byte(0x67), std::byte(0x89), std::byte(0xAB) };

    const std::vector<std::byte> mPreimage = { std::byte(0xCD), std::byte(0xEF), std::byte(0x02) };

    const std::vector<std::byte> mValue1 = { std::byte(0x04), std::byte(0x06), std::byte(0x08) };
    const std::vector<std::byte> mValue2 = { std::byte(0x0A), std::byte(0x0C), std::byte(0x0E) };
    const std::vector<std::byte> mValue3 = { std::byte(0x11), std::byte(0x13), std::byte(0x15) };

    LambdaStorageSlot mLambdaStorageSlot;

    LambdaMappingEntry mLambdaMappingEntry1;
    LambdaMappingEntry mLambdaMappingEntry2;
    LambdaMappingEntries mLambdaMappingEntries;

    LambdaStorageUpdate mLambdaStorageUpdate1;
    LambdaStorageUpdate mLambdaStorageUpdate2;

    mLambdaStorageSlot.setKey(mKey1);
    mLambdaStorageSlot.setValue(mValue1);

    mLambdaStorageUpdate1.setStorageSlot(mLambdaStorageSlot);

    mLambdaMappingEntry1.setKey(mKey2);
    mLambdaMappingEntry1.setValue(mValue2);

    mLambdaMappingEntry2.setPreimage(mPreimage);
    mLambdaMappingEntry2.setValue(mValue3);

    mLambdaMappingEntries.addEntry(mLambdaMappingEntry1);
    mLambdaMappingEntries.addEntry(mLambdaMappingEntry2);

    mLambdaStorageUpdate2.setMappingEntries(mLambdaMappingEntries);

    mLambdaStorageUpdates.push_back(mLambdaStorageUpdate1);
    mLambdaStorageUpdates.push_back(mLambdaStorageUpdate2);
  }

  [[nodiscard]] const std::vector<LambdaStorageUpdate>& getTestLambdaStorageUpdates() const
  {
    return mLambdaStorageUpdates;
  }

private:
  std::vector<LambdaStorageUpdate> mLambdaStorageUpdates;
};

//-----
TEST_F(LambdaEvmHookUnitTests, GetSetStorageUpdates)
{
  // Given
  LambdaEvmHook lambdaEvmHook;

  // When
  EXPECT_NO_THROW(lambdaEvmHook.setStorageUpdates(getTestLambdaStorageUpdates()));

  // Then
  EXPECT_EQ(lambdaEvmHook.getStorageUpdates().size(), getTestLambdaStorageUpdates().size());
}

//-----
TEST_F(LambdaEvmHookUnitTests, AddStorageUpdate)
{
  // Given
  LambdaEvmHook lambdaEvmHook;

  // When
  for (const LambdaStorageUpdate& update : getTestLambdaStorageUpdates())
  {
    EXPECT_NO_THROW(lambdaEvmHook.addStorageUpdate(update));
  }

  // Then
  EXPECT_EQ(lambdaEvmHook.getStorageUpdates().size(), getTestLambdaStorageUpdates().size());
}

//-----
TEST_F(LambdaEvmHookUnitTests, ClearStorageUpdates)
{
  // Given
  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setStorageUpdates(getTestLambdaStorageUpdates());

  // When
  EXPECT_NO_THROW(lambdaEvmHook.clearStorageUpdates());

  // Then
  EXPECT_TRUE(lambdaEvmHook.getStorageUpdates().empty());
}

//-----
TEST_F(LambdaEvmHookUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::LambdaEvmHook protoLambdaEvmHook;
  for (const LambdaStorageUpdate& update : getTestLambdaStorageUpdates())
  {
    *protoLambdaEvmHook.add_storage_updates() = *update.toProtobuf();
  }

  // When
  const LambdaEvmHook lambdaEvmHook = LambdaEvmHook::fromProtobuf(protoLambdaEvmHook);

  // Then
  EXPECT_EQ(lambdaEvmHook.getStorageUpdates().size(), getTestLambdaStorageUpdates().size());
}

//-----
TEST_F(LambdaEvmHookUnitTests, ToProtobuf)
{
  // Given
  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setStorageUpdates(getTestLambdaStorageUpdates());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::LambdaEvmHook> protoLambdaEvmHook = lambdaEvmHook.toProtobuf();

  // Then
  EXPECT_EQ(protoLambdaEvmHook->storage_updates_size(), getTestLambdaStorageUpdates().size());
}
