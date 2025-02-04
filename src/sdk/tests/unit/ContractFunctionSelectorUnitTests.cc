// SPDX-License-Identifier: Apache-2.0
#include "ContractFunctionSelector.h"
#include "exceptions/IllegalStateException.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class ContractFunctionSelectorUnitTests : public ::testing::Test
{
};

//-----
TEST_F(ContractFunctionSelectorUnitTests, AddAllTypes)
{
  // Given
  ContractFunctionSelector selector = ContractFunctionSelector()
                                        .addAddress()
                                        .addAddressArray()
                                        .addBool()
                                        .addBytes()
                                        .addBytes32()
                                        .addBytes32Array()
                                        .addBytesArray()
                                        .addFunction()
                                        .addInt8()
                                        .addInt8Array()
                                        .addInt32()
                                        .addInt32Array()
                                        .addInt64()
                                        .addInt64Array()
                                        .addInt256()
                                        .addInt256Array()
                                        .addUint8()
                                        .addUint8Array()
                                        .addUint32()
                                        .addUint32Array()
                                        .addUint64()
                                        .addUint64Array()
                                        .addUint256()
                                        .addUint256Array()
                                        .addString()
                                        .addStringArray();

  // When
  const std::vector<std::byte> bytes = selector.finish("testFunction");

  // Then
  const std::vector<std::byte> expectedBytes = { std::byte(0x44), std::byte(0x38), std::byte(0xE4), std::byte(0xCE) };
  EXPECT_EQ(bytes, expectedBytes);
}
