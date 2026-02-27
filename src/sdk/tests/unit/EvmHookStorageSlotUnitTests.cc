// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookStorageSlot.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class EvmHookStorageSlotUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::vector<std::byte>& getTestKey() const { return mKey; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestValue() const { return mValue; }

private:
  const std::vector<std::byte> mKey = { std::byte(0x01), std::byte(0x23), std::byte(0x45) };
  const std::vector<std::byte> mValue = { std::byte(0x67), std::byte(0x89), std::byte(0xAB) };
};

//-----
TEST_F(EvmHookStorageSlotUnitTests, GetSetKey)
{
  // Given
  EvmHookStorageSlot evmHookStorageSlot;

  // When
  EXPECT_NO_THROW(evmHookStorageSlot.setKey(getTestKey()));

  // Then
  EXPECT_EQ(evmHookStorageSlot.getKey(), getTestKey());
}

//-----
TEST_F(EvmHookStorageSlotUnitTests, GetSetValue)
{
  // Given
  EvmHookStorageSlot evmHookStorageSlot;

  // When
  EXPECT_NO_THROW(evmHookStorageSlot.setValue(getTestValue()));

  // Then
  EXPECT_EQ(evmHookStorageSlot.getValue(), getTestValue());
}

//-----
TEST_F(EvmHookStorageSlotUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::EvmHookStorageSlot protoEvmHookStorageSlot;

  protoEvmHookStorageSlot.set_key(internal::Utilities::byteVectorToString(getTestKey()));
  protoEvmHookStorageSlot.set_value(internal::Utilities::byteVectorToString(getTestValue()));

  // When
  const EvmHookStorageSlot evmHookStorageSlot = EvmHookStorageSlot::fromProtobuf(protoEvmHookStorageSlot);

  // Then
  EXPECT_EQ(evmHookStorageSlot.getKey(), getTestKey());
  EXPECT_EQ(evmHookStorageSlot.getValue(), getTestValue());
}

//-----
TEST_F(EvmHookStorageSlotUnitTests, ToProtobuf)
{
  // Given
  EvmHookStorageSlot evmHookStorageSlot;
  evmHookStorageSlot.setKey(getTestKey());
  evmHookStorageSlot.setValue(getTestValue());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::EvmHookStorageSlot> protoEvmHookStorageSlot =
    evmHookStorageSlot.toProtobuf();

  // Then
  EXPECT_EQ(protoEvmHookStorageSlot->key(), internal::Utilities::byteVectorToString(getTestKey()));
  EXPECT_EQ(protoEvmHookStorageSlot->value(), internal::Utilities::byteVectorToString(getTestValue()));
}
