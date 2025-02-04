// SPDX-License-Identifier: Apache-2.0
#include "EvmAddress.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace Hiero;

class EvmAddressUnitTests : public testing::Test
{
protected:
  [[nodiscard]] inline const std::string& getTestString() const { return mTestString; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestBytes() const { return mTestBytes; }

private:
  const std::string mTestString = "303132333435363738396162636465666768696A";
  const std::vector<std::byte> mTestBytes = { std::byte('0'), std::byte('1'), std::byte('2'), std::byte('3'),
                                              std::byte('4'), std::byte('5'), std::byte('6'), std::byte('7'),
                                              std::byte('8'), std::byte('9'), std::byte('a'), std::byte('b'),
                                              std::byte('c'), std::byte('d'), std::byte('e'), std::byte('f'),
                                              std::byte('g'), std::byte('h'), std::byte('i'), std::byte('j') };
};

TEST_F(EvmAddressUnitTests, StringConstructor)
{
  EXPECT_NO_THROW(auto evmAddress = EvmAddress::fromString(getTestString()));
  EXPECT_NO_THROW(auto evmAddress = EvmAddress::fromString("0x" + getTestString()));

  // String too short
  std::string badString = getTestString();
  badString.pop_back();
  EXPECT_THROW(auto evmAddress = EvmAddress::fromString(badString), std::invalid_argument);
  EXPECT_THROW(auto evmAddress = EvmAddress::fromString("0x" + badString), std::invalid_argument);

  // String contains non-hex characters
  badString.push_back('x');
  EXPECT_THROW(auto evmAddress = EvmAddress::fromString(badString), std::invalid_argument);
  EXPECT_THROW(auto evmAddress = EvmAddress::fromString("0x" + badString), std::invalid_argument);

  // String contains prefix not at beginning of string
  badString.pop_back();
  badString.pop_back();
  badString.insert(badString.size() / 2, "0x");
  EXPECT_THROW(auto evmAddress = EvmAddress::fromString(badString), std::invalid_argument);
  EXPECT_THROW(auto evmAddress = EvmAddress::fromString("0x" + badString), std::invalid_argument);
}

TEST_F(EvmAddressUnitTests, ByteConstructor)
{
  EXPECT_NO_THROW(auto evmAddress = EvmAddress::fromBytes(getTestBytes()));

  std::vector<std::byte> badBytes = getTestBytes();

  // Byte array too small
  badBytes.pop_back();
  EXPECT_THROW(auto evmAddress = EvmAddress::fromBytes(badBytes), std::invalid_argument);

  // Byte array too big
  badBytes.push_back(std::byte(255));
  badBytes.push_back(std::byte(172));
  EXPECT_THROW(auto evmAddress = EvmAddress::fromBytes(badBytes), std::invalid_argument);
}

TEST_F(EvmAddressUnitTests, StringByteEquality)
{
  std::string testString = getTestString();
  std::vector<std::byte> testBytes = getTestBytes();

  EXPECT_EQ(EvmAddress::fromString(testString).toBytes(), testBytes);
  EXPECT_EQ(EvmAddress::fromBytes(testBytes).toString(), testString);

  testString.front() = '4';
  testBytes.front() = std::byte('@');

  EXPECT_EQ(EvmAddress::fromString(testString).toBytes(), testBytes);
  EXPECT_EQ(EvmAddress::fromBytes(testBytes).toString(), testString);
}