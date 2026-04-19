// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHook.h"
#include "hooks/EvmHookMappingEntries.h"
#include "hooks/EvmHookMappingEntry.h"
#include "hooks/EvmHookStorageSlot.h"
#include "hooks/EvmHookStorageUpdate.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <services/hook_types.pb.h>
#include <vector>

using namespace Hiero;

class EvmHookUnitTests : public ::testing::Test
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

    EvmHookStorageSlot mEvmHookStorageSlot;

    EvmHookMappingEntry mEvmHookMappingEntry1;
    EvmHookMappingEntry mEvmHookMappingEntry2;
    EvmHookMappingEntries mEvmHookMappingEntries;

    EvmHookStorageUpdate mEvmHookStorageUpdate1;
    EvmHookStorageUpdate mEvmHookStorageUpdate2;

    mEvmHookStorageSlot.setKey(mKey1);
    mEvmHookStorageSlot.setValue(mValue1);

    mEvmHookStorageUpdate1.setStorageSlot(mEvmHookStorageSlot);

    mEvmHookMappingEntry1.setKey(mKey2);
    mEvmHookMappingEntry1.setValue(mValue2);

    mEvmHookMappingEntry2.setPreimage(mPreimage);
    mEvmHookMappingEntry2.setValue(mValue3);

    mEvmHookMappingEntries.addEntry(mEvmHookMappingEntry1);
    mEvmHookMappingEntries.addEntry(mEvmHookMappingEntry2);

    mEvmHookStorageUpdate2.setMappingEntries(mEvmHookMappingEntries);

    mEvmHookStorageUpdates.push_back(mEvmHookStorageUpdate1);
    mEvmHookStorageUpdates.push_back(mEvmHookStorageUpdate2);
  }

  [[nodiscard]] const std::vector<EvmHookStorageUpdate>& getTestEvmHookStorageUpdates() const
  {
    return mEvmHookStorageUpdates;
  }

private:
  std::vector<EvmHookStorageUpdate> mEvmHookStorageUpdates;
};

//-----
TEST_F(EvmHookUnitTests, GetSetStorageUpdates)
{
  // Given
  EvmHook evmHook;

  // When
  EXPECT_NO_THROW(evmHook.setStorageUpdates(getTestEvmHookStorageUpdates()));

  // Then
  EXPECT_EQ(evmHook.getStorageUpdates().size(), getTestEvmHookStorageUpdates().size());
}

//-----
TEST_F(EvmHookUnitTests, AddStorageUpdate)
{
  // Given
  EvmHook evmHook;

  // When
  for (const EvmHookStorageUpdate& update : getTestEvmHookStorageUpdates())
  {
    EXPECT_NO_THROW(evmHook.addStorageUpdate(update));
  }

  // Then
  EXPECT_EQ(evmHook.getStorageUpdates().size(), getTestEvmHookStorageUpdates().size());
}

//-----
TEST_F(EvmHookUnitTests, ClearStorageUpdates)
{
  // Given
  EvmHook evmHook;
  evmHook.setStorageUpdates(getTestEvmHookStorageUpdates());

  // When
  EXPECT_NO_THROW(evmHook.clearStorageUpdates());

  // Then
  EXPECT_TRUE(evmHook.getStorageUpdates().empty());
}

//-----
TEST_F(EvmHookUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::EvmHook protoEvmHook;
  for (const EvmHookStorageUpdate& update : getTestEvmHookStorageUpdates())
  {
    *protoEvmHook.add_storage_updates() = *update.toProtobuf();
  }

  // When
  const EvmHook evmHook = EvmHook::fromProtobuf(protoEvmHook);

  // Then
  EXPECT_EQ(evmHook.getStorageUpdates().size(), getTestEvmHookStorageUpdates().size());
}

//-----
TEST_F(EvmHookUnitTests, ToProtobuf)
{
  // Given
  EvmHook evmHook;
  evmHook.setStorageUpdates(getTestEvmHookStorageUpdates());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHook> protoEvmHook = evmHook.toProtobuf();

  // Then
  EXPECT_EQ(protoEvmHook->storage_updates_size(), getTestEvmHookStorageUpdates().size());
}
