// SPDX-License-Identifier: Apache-2.0
#include "hooks/LambdaStorageSlot.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/hook_types.pb.h>

using namespace Hiero;

class LambdaStorageSlotUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::vector<std::byte>& getTestKey() const { return mKey; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestValue() const { return mValue; }

private:
  const std::vector<std::byte> mKey = { std::byte(0x01), std::byte(0x23), std::byte(0x45) };
  const std::vector<std::byte> mValue = { std::byte(0x67), std::byte(0x89), std::byte(0xAB) };
};

//-----
TEST_F(LambdaStorageSlotUnitTests, GetSetKey)
{
  // Given
  LambdaStorageSlot lambdaStorageSlot;

  // When
  EXPECT_NO_THROW(lambdaStorageSlot.setKey(getTestKey()));

  // Then
  EXPECT_EQ(lambdaStorageSlot.getKey(), getTestKey());
}

//-----
TEST_F(LambdaStorageSlotUnitTests, GetSetValue)
{
  // Given
  LambdaStorageSlot lambdaStorageSlot;

  // When
  EXPECT_NO_THROW(lambdaStorageSlot.setValue(getTestValue()));

  // Then
  EXPECT_EQ(lambdaStorageSlot.getValue(), getTestValue());
}

//-----
TEST_F(LambdaStorageSlotUnitTests, FromProtobuf)
{
  // Given
  com::hedera::hapi::node::hooks::LambdaStorageSlot protoLambdaStorageSlot;

  protoLambdaStorageSlot.set_key(internal::Utilities::byteVectorToString(getTestKey()));
  protoLambdaStorageSlot.set_value(internal::Utilities::byteVectorToString(getTestValue()));

  // When
  const LambdaStorageSlot lambdaStorageSlot = LambdaStorageSlot::fromProtobuf(protoLambdaStorageSlot);

  // Then
  EXPECT_EQ(lambdaStorageSlot.getKey(), getTestKey());
  EXPECT_EQ(lambdaStorageSlot.getValue(), getTestValue());
}

//-----
TEST_F(LambdaStorageSlotUnitTests, ToProtobuf)
{
  // Given
  LambdaStorageSlot lambdaStorageSlot;
  lambdaStorageSlot.setKey(getTestKey());
  lambdaStorageSlot.setValue(getTestValue());

  // When
  const std::unique_ptr<com::hedera::hapi::node::hooks::LambdaStorageSlot> protoLambdaStorageSlot =
    lambdaStorageSlot.toProtobuf();

  // Then
  EXPECT_EQ(protoLambdaStorageSlot->key(), internal::Utilities::byteVectorToString(getTestKey()));
  EXPECT_EQ(protoLambdaStorageSlot->value(), internal::Utilities::byteVectorToString(getTestValue()));
}
