// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/EvmHook.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class HookCreationDetailsUnitTests : public ::testing::Test
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

    mEvmHook.addStorageUpdate(mEvmHookStorageUpdate1);
    mEvmHook.addStorageUpdate(mEvmHookStorageUpdate2);
  }

  [[nodiscard]] inline HookExtensionPoint getTestHookExtensionPoint() const { return mHookExtensionPoint; }
  [[nodiscard]] inline int64_t getTestHookId() const { return mHookId; }
  [[nodiscard]] inline const std::shared_ptr<Key>& getTestAdminKey() const { return mAdminKey; }

  [[nodiscard]] const EvmHook& getTestEvmHook() const { return mEvmHook; }

private:
  const HookExtensionPoint mHookExtensionPoint = HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK;
  const int64_t mHookId = 1LL;
  const std::shared_ptr<Key> mAdminKey = ED25519PrivateKey::generatePrivateKey();

  EvmHook mEvmHook;
};

//-----
TEST_F(HookCreationDetailsUnitTests, GetSetExtensionPoint)
{
  // Given
  HookCreationDetails hookCreationDetails;

  // When
  EXPECT_NO_THROW(hookCreationDetails.setHookId(getTestHookId()));

  // Then
  EXPECT_EQ(hookCreationDetails.getHookId(), getTestHookId());
}

//-----
TEST_F(HookCreationDetailsUnitTests, GetSetEvmHook)
{
  // Given
  HookCreationDetails hookCreationDetails;

  // When
  EXPECT_NO_THROW(hookCreationDetails.setEvmHook(getTestEvmHook()));

  // Then
  EXPECT_TRUE(hookCreationDetails.getEvmHook().has_value());
  EXPECT_EQ(hookCreationDetails.getEvmHook()->getStorageUpdates().size(),
            getTestEvmHook().getStorageUpdates().size());
}

//-----
TEST_F(HookCreationDetailsUnitTests, GetSetAdminKey)
{
  // Given
  HookCreationDetails hookCreationDetails;

  // When
  EXPECT_NO_THROW(hookCreationDetails.setAdminKey(getTestAdminKey()));

  // Then
  EXPECT_EQ(hookCreationDetails.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
}

//-----
TEST_F(HookCreationDetailsUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::HookCreationDetails protoHookCreationDetails;

  protoHookCreationDetails.set_extension_point(
    gHookExtensionPointToProtobufHookExtensionPoint.at(getTestHookExtensionPoint()));
  protoHookCreationDetails.set_hook_id(getTestHookId());
  protoHookCreationDetails.set_allocated_evm_hook(getTestEvmHook().toProtobuf().release());
  protoHookCreationDetails.set_allocated_admin_key(getTestAdminKey()->toProtobufKey().release());

  // When
  const HookCreationDetails hookCreationDetails = HookCreationDetails::fromProtobuf(protoHookCreationDetails);

  // Then
  EXPECT_EQ(hookCreationDetails.getExtensionPoint(), getTestHookExtensionPoint());
  EXPECT_EQ(hookCreationDetails.getHookId(), getTestHookId());
  EXPECT_TRUE(hookCreationDetails.getEvmHook().has_value());
  EXPECT_TRUE(hookCreationDetails.getAdminKey());
}

//-----
TEST_F(HookCreationDetailsUnitTests, ToProtobuf)
{
  // Given
  HookCreationDetails hookCreationDetails;

  hookCreationDetails.setExtensionPoint(getTestHookExtensionPoint());
  hookCreationDetails.setHookId(getTestHookId());
  hookCreationDetails.setEvmHook(getTestEvmHook());
  hookCreationDetails.setAdminKey(getTestAdminKey());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::HookCreationDetails> protoHookCreationDetails =
    hookCreationDetails.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCreationDetails->extension_point(),
            gHookExtensionPointToProtobufHookExtensionPoint.at(getTestHookExtensionPoint()));
  EXPECT_EQ(protoHookCreationDetails->hook_id(), getTestHookId());
  EXPECT_TRUE(protoHookCreationDetails->has_evm_hook());
  EXPECT_TRUE(protoHookCreationDetails->has_admin_key());
}
