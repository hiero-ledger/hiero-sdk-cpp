// SPDX-License-Identifier: Apache-2.0
#include "FeeComponents.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FeeComponentsUnitTests : public ::testing::Test
{
protected:
  FeeComponents feeComponents;
};

//-----
TEST_F(FeeComponentsUnitTests, DefaultConstructor)
{
  // Then
  EXPECT_EQ(feeComponents.getMin(), 0LL);
  EXPECT_EQ(feeComponents.getMax(), 0LL);
  EXPECT_EQ(feeComponents.getConstant(), 0LL);
  EXPECT_EQ(feeComponents.getTransactionBandwidthBytes(), 0LL);
  EXPECT_EQ(feeComponents.getTransactionVerification(), 0LL);
  EXPECT_EQ(feeComponents.getTransactionRamByteHour(), 0LL);
  EXPECT_EQ(feeComponents.getTransactionStorageByteHour(), 0LL);
  EXPECT_EQ(feeComponents.getContractTransactionGas(), 0LL);
  EXPECT_EQ(feeComponents.getTransferVolumeHbar(), 0LL);
  EXPECT_EQ(feeComponents.getResponseMemoryByte(), 0LL);
  EXPECT_EQ(feeComponents.getResponseDiskByte(), 0LL);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorEqualsDefaultConstructed)
{
  // Given
  FeeComponents lhs;
  FeeComponents rhs;

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorEqualsIdentical)
{
  // Given
  FeeComponents lhs;
  lhs.setMin(1LL)
    .setMax(2LL)
    .setConstant(3LL)
    .setTransactionBandwidthBytes(4LL)
    .setTransactionVerification(5LL)
    .setTransactionRamByteHour(6LL)
    .setTransactionStorageByteHour(7LL)
    .setContractTransactionGas(8LL)
    .setTransferVolumeHbar(9LL)
    .setResponseMemoryByte(10LL)
    .setResponseDiskByte(11LL);

  FeeComponents rhs;
  rhs.setMin(1LL)
    .setMax(2LL)
    .setConstant(3LL)
    .setTransactionBandwidthBytes(4LL)
    .setTransactionVerification(5LL)
    .setTransactionRamByteHour(6LL)
    .setTransactionStorageByteHour(7LL)
    .setContractTransactionGas(8LL)
    .setTransferVolumeHbar(9LL)
    .setResponseMemoryByte(10LL)
    .setResponseDiskByte(11LL);

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentMin)
{
  // Given
  FeeComponents lhs;
  lhs.setMin(1LL);
  FeeComponents rhs;
  rhs.setMin(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentMax)
{
  // Given
  FeeComponents lhs;
  lhs.setMax(1LL);
  FeeComponents rhs;
  rhs.setMax(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentConstant)
{
  // Given
  FeeComponents lhs;
  lhs.setConstant(1LL);
  FeeComponents rhs;
  rhs.setConstant(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentTransactionBandwidthBytes)
{
  // Given
  FeeComponents lhs;
  lhs.setTransactionBandwidthBytes(1LL);
  FeeComponents rhs;
  rhs.setTransactionBandwidthBytes(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentTransactionVerification)
{
  // Given
  FeeComponents lhs;
  lhs.setTransactionVerification(1LL);
  FeeComponents rhs;
  rhs.setTransactionVerification(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentTransactionRamByteHour)
{
  // Given
  FeeComponents lhs;
  lhs.setTransactionRamByteHour(1LL);
  FeeComponents rhs;
  rhs.setTransactionRamByteHour(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentTransactionStorageByteHour)
{
  // Given
  FeeComponents lhs;
  lhs.setTransactionStorageByteHour(1LL);
  FeeComponents rhs;
  rhs.setTransactionStorageByteHour(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentContractTransactionGas)
{
  // Given
  FeeComponents lhs;
  lhs.setContractTransactionGas(1LL);
  FeeComponents rhs;
  rhs.setContractTransactionGas(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentTransferVolumeHbar)
{
  // Given
  FeeComponents lhs;
  lhs.setTransferVolumeHbar(1LL);
  FeeComponents rhs;
  rhs.setTransferVolumeHbar(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentResponseMemoryByte)
{
  // Given
  FeeComponents lhs;
  lhs.setResponseMemoryByte(1LL);
  FeeComponents rhs;
  rhs.setResponseMemoryByte(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(FeeComponentsUnitTests, OperatorNotEqualsDifferentResponseDiskByte)
{
  // Given
  FeeComponents lhs;
  lhs.setResponseDiskByte(1LL);
  FeeComponents rhs;
  rhs.setResponseDiskByte(2LL);

  // Then
  EXPECT_FALSE(lhs == rhs);
}
