// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/LambdaEvmHook.h"
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

    mLambdaEvmHook.addStorageUpdate(mLambdaStorageUpdate1);
    mLambdaEvmHook.addStorageUpdate(mLambdaStorageUpdate2);
  }

  [[nodiscard]] inline HookExtensionPoint getTestHookExtensionPoint() const { return mHookExtensionPoint; }
  [[nodiscard]] inline int64_t getTestHookId() const { return mHookId; }
  [[nodiscard]] inline const std::shared_ptr<Key>& getTestAdminKey() const { return mAdminKey; }

  [[nodiscard]] const LambdaEvmHook& getTestLambdaEvmHook() const { return mLambdaEvmHook; }

private:
  const HookExtensionPoint mHookExtensionPoint = HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK;
  const int64_t mHookId = 1LL;
  const std::shared_ptr<Key> mAdminKey = ED25519PrivateKey::generatePrivateKey();

  LambdaEvmHook mLambdaEvmHook;
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
TEST_F(HookCreationDetailsUnitTests, GetSetLambdaEvmHook)
{
  // Given
  HookCreationDetails hookCreationDetails;

  // When
  EXPECT_NO_THROW(hookCreationDetails.setLambdaEvmHook(getTestLambdaEvmHook()));

  // Then
  EXPECT_TRUE(hookCreationDetails.getLambdaEvmHook().has_value());
  EXPECT_EQ(hookCreationDetails.getLambdaEvmHook()->getStorageUpdates().size(),
            getTestLambdaEvmHook().getStorageUpdates().size());
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
  protoHookCreationDetails.set_allocated_lambda_evm_hook(getTestLambdaEvmHook().toProtobuf().release());
  protoHookCreationDetails.set_allocated_admin_key(getTestAdminKey()->toProtobufKey().release());

  // When
  const HookCreationDetails hookCreationDetails = HookCreationDetails::fromProtobuf(protoHookCreationDetails);

  // Then
  EXPECT_EQ(hookCreationDetails.getExtensionPoint(), getTestHookExtensionPoint());
  EXPECT_EQ(hookCreationDetails.getHookId(), getTestHookId());
  EXPECT_TRUE(hookCreationDetails.getLambdaEvmHook().has_value());
  EXPECT_TRUE(hookCreationDetails.getAdminKey());
}

//-----
TEST_F(HookCreationDetailsUnitTests, ToProtobuf)
{
  // Given
  HookCreationDetails hookCreationDetails;

  hookCreationDetails.setExtensionPoint(getTestHookExtensionPoint());
  hookCreationDetails.setHookId(getTestHookId());
  hookCreationDetails.setLambdaEvmHook(getTestLambdaEvmHook());
  hookCreationDetails.setAdminKey(getTestAdminKey());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::HookCreationDetails> protoHookCreationDetails =
    hookCreationDetails.toProtobuf();

  // Then
  EXPECT_EQ(protoHookCreationDetails->extension_point(),
            gHookExtensionPointToProtobufHookExtensionPoint.at(getTestHookExtensionPoint()));
  EXPECT_EQ(protoHookCreationDetails->hook_id(), getTestHookId());
  EXPECT_TRUE(protoHookCreationDetails->has_lambda_evm_hook());
  EXPECT_TRUE(protoHookCreationDetails->has_admin_key());
}
