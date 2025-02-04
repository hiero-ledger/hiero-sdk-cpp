// SPDX-License-Identifier: Apache-2.0
#include "ContractFunctionParameters.h"
#include "impl/HexConverter.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <stdexcept>
#include <string>
#include <vector>

using namespace Hiero;

class ContractFunctionParametersUnitTests : public ::testing::Test
{
};

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeIntTypes)
{
  // Given
  ContractFunctionParameters contractFunctionParameters =
    ContractFunctionParameters()
      .addUint8(1)
      .addInt8(-2)
      .addUint32(3)
      .addInt32(-4)
      .addUint64(4)
      .addInt64(-5)
      .addUint256({
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x06)
  })
      .addInt256({ std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xF9) })
      .addUint8Array({ 1, 2, 3, 4 })
      .addInt8Array({ -5, 6, 7, -8 })
      .addUint32Array({ 9, 10, 11, 12 })
      .addInt32Array({ -13, 14, 15, -16 })
      .addUint64Array({ 17, 18, 19, 20 })
      .addInt64Array({ -21, 22, 23, -24 })
      .addUint256Array({ {
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
        std::byte(0x00), std::byte(0x19),
      } })
      .addInt256Array(
        { { std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
            std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
            std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
            std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
            std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
            std::byte(0xFF), std::byte(0xE6) } });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes("foo"));

  // Then
  EXPECT_EQ(hexParams,
            "11BCD903"
            "0000000000000000000000000000000000000000000000000000000000000001" // addUint8
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE" // addInt8
            "0000000000000000000000000000000000000000000000000000000000000003" // addUint32
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC" // addInt32
            "0000000000000000000000000000000000000000000000000000000000000004" // addUInt64
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB" // addInt64
            "0000000000000000000000000000000000000000000000000000000000000006" // addUInt256
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF9" // addInt256
            "0000000000000000000000000000000000000000000000000000000000000200" // addUint8Array offset
            "00000000000000000000000000000000000000000000000000000000000002A0" // addInt8Array offset
            "0000000000000000000000000000000000000000000000000000000000000340" // addUint32Array offset
            "00000000000000000000000000000000000000000000000000000000000003E0" // addInt32Array offset
            "0000000000000000000000000000000000000000000000000000000000000480" // addUint64Array offset
            "0000000000000000000000000000000000000000000000000000000000000520" // addInt64Array offset
            "00000000000000000000000000000000000000000000000000000000000005C0" // addUint256Array offset
            "0000000000000000000000000000000000000000000000000000000000000600" // addInt256Array offset
            "0000000000000000000000000000000000000000000000000000000000000004" // addUint8Array size
            "0000000000000000000000000000000000000000000000000000000000000001" // addUint8Array values
            "0000000000000000000000000000000000000000000000000000000000000002"
            "0000000000000000000000000000000000000000000000000000000000000003"
            "0000000000000000000000000000000000000000000000000000000000000004"
            "0000000000000000000000000000000000000000000000000000000000000004" // addInt8Array size
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB" // addInt8Array values
            "0000000000000000000000000000000000000000000000000000000000000006"
            "0000000000000000000000000000000000000000000000000000000000000007"
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF8"
            "0000000000000000000000000000000000000000000000000000000000000004" // addUint32Array size
            "0000000000000000000000000000000000000000000000000000000000000009" // addUint32Array values
            "000000000000000000000000000000000000000000000000000000000000000A"
            "000000000000000000000000000000000000000000000000000000000000000B"
            "000000000000000000000000000000000000000000000000000000000000000C"
            "0000000000000000000000000000000000000000000000000000000000000004" // addInt32Array size
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF3" // addInt32Array values
            "000000000000000000000000000000000000000000000000000000000000000E"
            "000000000000000000000000000000000000000000000000000000000000000F"
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0"
            "0000000000000000000000000000000000000000000000000000000000000004" // addUint64Array size
            "0000000000000000000000000000000000000000000000000000000000000011" // addUint64Array values
            "0000000000000000000000000000000000000000000000000000000000000012"
            "0000000000000000000000000000000000000000000000000000000000000013"
            "0000000000000000000000000000000000000000000000000000000000000014"
            "0000000000000000000000000000000000000000000000000000000000000004" // addInt64Array size
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEB" // addInt64Array values
            "0000000000000000000000000000000000000000000000000000000000000016"
            "0000000000000000000000000000000000000000000000000000000000000017"
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE8"
            "0000000000000000000000000000000000000000000000000000000000000001"   // addUint256Array size
            "0000000000000000000000000000000000000000000000000000000000000019"   // addUint256Array values
            "0000000000000000000000000000000000000000000000000000000000000001"   // addInt256Array size
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE6"); // addInt256Array values
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeAddresses)
{
  // Given
  ContractFunctionParameters contractFunctionParameters =
    ContractFunctionParameters()
      .addAddress("1122334455667788990011223344556677889900")
      .addAddress("0x1122334455667788990011223344556677889900")
      .addAddressArray({ "1122334455667788990011223344556677889900", "1122334455667788990011223344556677889900" });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes("foo"));

  // Then
  EXPECT_EQ(hexParams,
            "7D48C86D"
            "0000000000000000000000001122334455667788990011223344556677889900" // addAddress
            "0000000000000000000000001122334455667788990011223344556677889900" // addAddress
            "0000000000000000000000000000000000000000000000000000000000000060" // addAddressArray offset
            "0000000000000000000000000000000000000000000000000000000000000002" // addAddressArray size
            "0000000000000000000000001122334455667788990011223344556677889900" // addAddressArray values
            "0000000000000000000000001122334455667788990011223344556677889900");
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeFunctions)
{
  // Given
  ContractFunctionParameters contractFunctionParameters = ContractFunctionParameters().addFunction(
    "0x1122334455667788990011223344556677889900", ContractFunctionSelector().addBool().finish("randomFunction"));

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes("foo"));

  // Then
  EXPECT_EQ(hexParams,
            "76D47909"
            "112233445566778899001122334455667788990063441D820000000000000000");
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeBytes32)
{
  // Given
  ContractFunctionParameters contractFunctionParameters = ContractFunctionParameters().addBytes32({
    std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04), std::byte(0x05), std::byte(0x06),
    std::byte(0x07), std::byte(0x08), std::byte(0x09), std::byte(0x0A), std::byte(0x0B), std::byte(0x0C),
    std::byte(0x0D), std::byte(0x0E), std::byte(0x0F), std::byte(0x10), std::byte(0x11), std::byte(0x12),
    std::byte(0x13), std::byte(0x14), std::byte(0x15), std::byte(0x16), std::byte(0x17), std::byte(0x18),
    std::byte(0x19), std::byte(0x1A), std::byte(0x1B), std::byte(0x1C), std::byte(0x1D), std::byte(0x1E),
    std::byte(0x1F), std::byte(0x20),
  });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes("foo"));

  // Then
  EXPECT_EQ(hexParams,
            "11E814C1"
            "0102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20");
}

//-----
TEST_F(ContractFunctionParametersUnitTests, ThrowBytes32EncodeTooLong)
{
  // Given / When / Then
  EXPECT_THROW(ContractFunctionParameters contractFunctionParameters = ContractFunctionParameters().addBytes32(
                 { std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04), std::byte(0x05), std::byte(0x06),
                   std::byte(0x07), std::byte(0x08), std::byte(0x09), std::byte(0x0A), std::byte(0x0B), std::byte(0x0C),
                   std::byte(0x0D), std::byte(0x0E), std::byte(0x0F), std::byte(0x10), std::byte(0x11), std::byte(0x12),
                   std::byte(0x13), std::byte(0x14), std::byte(0x15), std::byte(0x16), std::byte(0x17), std::byte(0x18),
                   std::byte(0x19), std::byte(0x1A), std::byte(0x1B), std::byte(0x1C), std::byte(0x1D), std::byte(0x1E),
                   std::byte(0x1F), std::byte(0x20), std::byte(0x21) }),
               std::invalid_argument);
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeBool)
{
  // Given
  ContractFunctionParameters contractFunctionParameters = ContractFunctionParameters().addBool(true).addBool(false);

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes("foo"));

  // Then
  EXPECT_EQ(hexParams,
            "B3CEDFCF"
            "0000000000000000000000000000000000000000000000000000000000000001"   // addBool(true)
            "0000000000000000000000000000000000000000000000000000000000000000"); // addBool(false)
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeDynamicParameters)
{
  // Given
  ContractFunctionParameters contractFunctionParametersStr = ContractFunctionParameters().addString("Hello, world!");
  ContractFunctionParameters contractFunctionParametersBytes =
    ContractFunctionParameters().addBytes({ std::byte('H'),
                                            std::byte('e'),
                                            std::byte('l'),
                                            std::byte('l'),
                                            std::byte('o'),
                                            std::byte(','),
                                            std::byte(' '),
                                            std::byte('w'),
                                            std::byte('o'),
                                            std::byte('r'),
                                            std::byte('l'),
                                            std::byte('d'),
                                            std::byte('!') });

  // When
  const std::string hexParamsStr =
    internal::HexConverter::bytesToHex(contractFunctionParametersStr.toBytes("set_message"));
  const std::string hexParamsBytes =
    internal::HexConverter::bytesToHex(contractFunctionParametersBytes.toBytes("set_message"));

  // Then
  const std::string encoding = "0000000000000000000000000000000000000000000000000000000000000020"  // offset
                               "000000000000000000000000000000000000000000000000000000000000000D"  // size
                               "48656C6C6F2C20776F726C642100000000000000000000000000000000000000"; // value
  EXPECT_EQ(hexParamsStr, "2E982602" + encoding);
  EXPECT_EQ(hexParamsBytes, "010473A7" + encoding);
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeStaticParameters)
{
  // Given
  ContractFunctionParameters contractFunctionParameters = ContractFunctionParameters()
                                                            .addInt32(0x11223344)
                                                            .addInt32(-65536)
                                                            .addUint64(-65536)
                                                            .addAddress("00112233445566778899aabbccddeeff00112233");

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes());

  // Then
  EXPECT_EQ(hexParams,
            "0000000000000000000000000000000000000000000000000000000011223344"   // addInt32
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000"   // addInt32
            "000000000000000000000000000000000000000000000000FFFFFFFFFFFF0000"   // addUint64
            "00000000000000000000000000112233445566778899AABBCCDDEEFF00112233"); // addAddress
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeMixedStaticAndDynamic)
{
  // Given
  ContractFunctionParameters contractFunctionParameters =
    ContractFunctionParameters()
      .addInt256({ std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xFF),
                   std::byte(0xFF), std::byte(0xFF), std::byte(0xFF), std::byte(0xDE), std::byte(0xAD), std::byte(0xBE),
                   std::byte(0xEF), std::byte(0x00) })
      .addString("Hello, world!")
      .addBytes({ std::byte(-1), std::byte(-18), std::byte(63), std::byte(127) })
      .addBool(true)
      .addUint8Array({ static_cast<uint8_t>(-1), 127 });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes("foo"));

  // Then
  EXPECT_EQ(hexParams,
            "6A5BB8F2"
            "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDEADBEEF00" // addInt256
            "00000000000000000000000000000000000000000000000000000000000000A0" // addString offset
            "00000000000000000000000000000000000000000000000000000000000000E0" // addBytes offset
            "0000000000000000000000000000000000000000000000000000000000000001" // addBool
            "0000000000000000000000000000000000000000000000000000000000000120" // addUint8Array offset
            "000000000000000000000000000000000000000000000000000000000000000D" // addString size
            "48656C6C6F2C20776F726C642100000000000000000000000000000000000000" // addString value
            "0000000000000000000000000000000000000000000000000000000000000004" // addBytes size
            "FFEE3F7F00000000000000000000000000000000000000000000000000000000" // addBytes value
            "0000000000000000000000000000000000000000000000000000000000000002" // addUint8Array size
            "00000000000000000000000000000000000000000000000000000000000000FF" // addUint8Array values
            "000000000000000000000000000000000000000000000000000000000000007F");
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeArrayTypes)
{
  // Given
  ContractFunctionParameters contractFunctionParameters =
    ContractFunctionParameters()
      .addStringArray({
        "hello", ",", "world!"
  })
      .addInt32Array({ 0x88, 0x99, 0xAA, 0xBB })
      .addInt256Array(
        { { std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x00),
            std::byte(0x11), std::byte(0x11) } });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes("foo"));

  // Then
  EXPECT_EQ(hexParams,
            "025838FC"
            "0000000000000000000000000000000000000000000000000000000000000060" // addStringArray offset
            "00000000000000000000000000000000000000000000000000000000000001A0" // addInt32Array offset
            "0000000000000000000000000000000000000000000000000000000000000240" // addInt256Array offset
            "0000000000000000000000000000000000000000000000000000000000000003" // addStringArray size
            "0000000000000000000000000000000000000000000000000000000000000060" // addStringArray value offsets
            "00000000000000000000000000000000000000000000000000000000000000A0"
            "00000000000000000000000000000000000000000000000000000000000000E0"
            "0000000000000000000000000000000000000000000000000000000000000005" // addStringArray value[0] size
            "68656C6C6F000000000000000000000000000000000000000000000000000000" // addStringArray value[0] value
            "0000000000000000000000000000000000000000000000000000000000000001" // addStringArray value[1] size
            "2C00000000000000000000000000000000000000000000000000000000000000" // addStringArray value[1] value
            "0000000000000000000000000000000000000000000000000000000000000006" // addStringArray value[2] size
            "776F726C64210000000000000000000000000000000000000000000000000000" // addStringArray value[2] value
            "0000000000000000000000000000000000000000000000000000000000000004" // addInt32Array size
            "0000000000000000000000000000000000000000000000000000000000000088" // addInt32Array values
            "0000000000000000000000000000000000000000000000000000000000000099"
            "00000000000000000000000000000000000000000000000000000000000000AA"
            "00000000000000000000000000000000000000000000000000000000000000BB"
            "0000000000000000000000000000000000000000000000000000000000000001"   // addInt256Array size
            "0000000000000000000000000000000000000000000000000000000000001111"); // addInt256Array value
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeBytes32Array)
{
  // Given
  ContractFunctionParameters contractFunctionParameters = ContractFunctionParameters().addBytes32Array({
    { std::byte('H'), std::byte('e'), std::byte('l'), std::byte('l'), std::byte('o') },
    { std::byte(',') },
    { std::byte('w'), std::byte('o'), std::byte('r'), std::byte('l'), std::byte('d'), std::byte('!') }
  });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes());

  // Then
  EXPECT_EQ(hexParams,
            "0000000000000000000000000000000000000000000000000000000000000020"   // addBytes32Array offset
            "0000000000000000000000000000000000000000000000000000000000000003"   // addBytes32Array size
            "48656C6C6F000000000000000000000000000000000000000000000000000000"   // addBytes32Array values ("Hello")
            "2C00000000000000000000000000000000000000000000000000000000000000"   // (",")
            "776F726C64210000000000000000000000000000000000000000000000000000"); // ("world!")
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeBytesArray)
{
  // Given
  ContractFunctionParameters contractFunctionParameters = ContractFunctionParameters().addBytesArray({
    { std::byte('H'), std::byte('e'), std::byte('l'), std::byte('l'), std::byte('o') },
    { std::byte(',') },
    { std::byte('w'), std::byte('o'), std::byte('r'), std::byte('l'), std::byte('d'), std::byte('!') }
  });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes());

  // Then
  EXPECT_EQ(hexParams,
            "0000000000000000000000000000000000000000000000000000000000000020" // addBytesArray offset
            "0000000000000000000000000000000000000000000000000000000000000003" // addBytesArray size
            "0000000000000000000000000000000000000000000000000000000000000060" // addBytesArray value offsets
            "00000000000000000000000000000000000000000000000000000000000000A0"
            "00000000000000000000000000000000000000000000000000000000000000E0"
            "0000000000000000000000000000000000000000000000000000000000000005"   // addBytesArray value[0] size
            "48656C6C6F000000000000000000000000000000000000000000000000000000"   // addBytesArray value[0] value
            "0000000000000000000000000000000000000000000000000000000000000001"   // addBytesArray value[1] size
            "2C00000000000000000000000000000000000000000000000000000000000000"   // addBytesArray value[1] value
            "0000000000000000000000000000000000000000000000000000000000000006"   // addBytesArray value[2] size
            "776F726C64210000000000000000000000000000000000000000000000000000"); // addBytesArray value[2] value
}

//-----
TEST_F(ContractFunctionParametersUnitTests, EncodeStringArray)
{
  // Given
  ContractFunctionParameters contractFunctionParameters =
    ContractFunctionParameters().addStringArray({ "Hello", ",", "world!" });

  // When
  const std::string hexParams = internal::HexConverter::bytesToHex(contractFunctionParameters.toBytes());

  // Then
  EXPECT_EQ(hexParams,
            "0000000000000000000000000000000000000000000000000000000000000020" // addStringArray offset
            "0000000000000000000000000000000000000000000000000000000000000003" // addStringArray size
            "0000000000000000000000000000000000000000000000000000000000000060" // addStringArray value offsets
            "00000000000000000000000000000000000000000000000000000000000000A0"
            "00000000000000000000000000000000000000000000000000000000000000E0"
            "0000000000000000000000000000000000000000000000000000000000000005"   // addStringArray value[0] size
            "48656C6C6F000000000000000000000000000000000000000000000000000000"   // addStringArray value[0] value
            "0000000000000000000000000000000000000000000000000000000000000001"   // addStringArray value[1] size
            "2C00000000000000000000000000000000000000000000000000000000000000"   // addStringArray value[1] value
            "0000000000000000000000000000000000000000000000000000000000000006"   // addStringArray value[2] size
            "776F726C64210000000000000000000000000000000000000000000000000000"); // addStringArray value[2] value
}

//-----
TEST_F(ContractFunctionParametersUnitTests, ThrowOnBadAddress)
{
  // Given
  ContractFunctionParameters contractFunctionParameters;

  // When / Then
  EXPECT_THROW(contractFunctionParameters.addAddress(""), std::invalid_argument);
  EXPECT_THROW(contractFunctionParameters.addAddress("aabbccdd"), std::invalid_argument);
  EXPECT_THROW(contractFunctionParameters.addAddress("00112233445566778899aabbccddeeff0011223344"),
               std::invalid_argument);
  EXPECT_THROW(contractFunctionParameters.addAddress("gghhii--__zz66778899aabbccddeeff00112233"),
               std::invalid_argument);
}
