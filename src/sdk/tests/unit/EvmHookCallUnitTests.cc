// SPDX-License-Identifier: Apache-2.0
#include "hooks/EvmHookCall.h"
#include "impl/Utilities.h"

#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <services/basic_types.pb.h>
#include <vector>

using namespace Hiero;

class EvmHookCallUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::vector<std::byte>& getTestCallData() const { return mCallData; }
  [[nodiscard]] inline uint64_t getTestGasLimit() const { return mGasLimit; }

private:
  const std::vector<std::byte> mCallData = { std::byte(0x01), std::byte(0x23), std::byte(0x45) };
  const uint64_t mGasLimit = 1000000ULL;
};

//-----
TEST_F(EvmHookCallUnitTests, GetSetCallData)
{
  // Given
  EvmHookCall evmHookCall;

  // When
  EXPECT_NO_THROW(evmHookCall.setData(getTestCallData()));

  // Then
  EXPECT_EQ(evmHookCall.getData(), getTestCallData());
}

//-----
TEST_F(EvmHookCallUnitTests, GetSetGasLimit)
{
  // Given
  EvmHookCall evmHookCall;

  // When
  EXPECT_NO_THROW(evmHookCall.setGasLimit(getTestGasLimit()));

  // Then
  EXPECT_EQ(evmHookCall.getGasLimit(), getTestGasLimit());
}

//-----
TEST_F(EvmHookCallUnitTests, FromProtobuf)
{
  // Given
  proto::EvmHookCall protoEvmHookCall;
  protoEvmHookCall.set_data(internal::Utilities::byteVectorToString(getTestCallData()));
  protoEvmHookCall.set_gas_limit(getTestGasLimit());

  // When
  const EvmHookCall evmHookCall = EvmHookCall::fromProtobuf(protoEvmHookCall);

  // Then
  EXPECT_EQ(evmHookCall.getData(), getTestCallData());
  EXPECT_EQ(evmHookCall.getGasLimit(), getTestGasLimit());
}

//-----
TEST_F(EvmHookCallUnitTests, ToProtobuf)
{
  // Given
  EvmHookCall evmHookCall;
  evmHookCall.setData(getTestCallData());
  evmHookCall.setGasLimit(getTestGasLimit());

  // When
  const std::unique_ptr<proto::EvmHookCall> protoEvmHookCall = evmHookCall.toProtobuf();

  // Then
  EXPECT_EQ(protoEvmHookCall->data(), internal::Utilities::byteVectorToString(getTestCallData()));
  EXPECT_EQ(protoEvmHookCall->gas_limit(), getTestGasLimit());
}
