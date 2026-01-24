// SPDX-License-Identifier: Apache-2.0
#include "IPv4Address.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

using namespace Hiero;

class IPv4AddressUnitTests : public ::testing::Test
{
};

//-----
TEST_F(IPv4AddressUnitTests, FromBytesSuccess)
{
  // Given
  const std::vector<std::byte> bytes = { std::byte(192), std::byte(168), std::byte(1), std::byte(1) };

  // When
  const IPv4Address address = IPv4Address::fromBytes(bytes);

  // Then
  EXPECT_EQ(address.toString(), "192.168.1.1");
}

//-----
TEST_F(IPv4AddressUnitTests, FromBytesInvalidSize)
{
  // Given
  const std::vector<std::byte> tooFewBytes = { std::byte(192), std::byte(168), std::byte(1) };
  const std::vector<std::byte> tooManyBytes = {
    std::byte(192), std::byte(168), std::byte(1), std::byte(1), std::byte(1)
  };

  // When / Then
  // Cast to void to suppress [[nodiscard]] warning
  EXPECT_THROW((void)IPv4Address::fromBytes(tooFewBytes), std::invalid_argument);
  EXPECT_THROW((void)IPv4Address::fromBytes(tooManyBytes), std::invalid_argument);
}

//-----
TEST_F(IPv4AddressUnitTests, ToBytes)
{
  // Given
  const std::vector<std::byte> expectedBytes = { std::byte(10), std::byte(0), std::byte(0), std::byte(1) };
  const IPv4Address address = IPv4Address::fromBytes(expectedBytes);

  // When
  const std::vector<std::byte> actualBytes = address.toBytes();

  // Then
  EXPECT_EQ(actualBytes, expectedBytes);
}

//-----
TEST_F(IPv4AddressUnitTests, ToString)
{
  // Given
  const std::vector<std::byte> bytes = { std::byte(127), std::byte(0), std::byte(0), std::byte(1) };
  const IPv4Address address = IPv4Address::fromBytes(bytes);

  // When
  const std::string ipString = address.toString();

  // Then
  EXPECT_EQ(ipString, "127.0.0.1");
}

//-----
TEST_F(IPv4AddressUnitTests, IsEmptyWhenDefault)
{
  // Given
  const IPv4Address address;

  // When / Then
  EXPECT_TRUE(address.isEmpty());
}

//-----
TEST_F(IPv4AddressUnitTests, IsNotEmptyWhenSet)
{
  // Given
  const std::vector<std::byte> bytes = { std::byte(8), std::byte(8), std::byte(8), std::byte(8) };
  const IPv4Address address = IPv4Address::fromBytes(bytes);

  // When / Then
  EXPECT_FALSE(address.isEmpty());
}
