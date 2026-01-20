// SPDX-License-Identifier: Apache-2.0
#include "Endpoint.h"
#include "IPv4Address.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>
#include <string>
#include <vector>

using namespace Hiero;

class EndpointUnitTests : public ::testing::Test
{
};

TEST_F(EndpointUnitTests, SetGetAddress)
{
  // Given
  Endpoint endpoint;
  const std::vector<std::byte> bytes = { std::byte(192), std::byte(168), std::byte(0), std::byte(1) };
  const IPv4Address address = IPv4Address::fromBytes(bytes);

  // When
  endpoint.setAddress(address);

  // Then
  EXPECT_EQ(endpoint.getAddress().toString(), "192.168.0.1");
}

TEST_F(EndpointUnitTests, SetGetPort)
{
  // Given
  Endpoint endpoint;
  const unsigned int port = 50211;

  // When
  endpoint.setPort(port);

  // Then
  EXPECT_EQ(endpoint.getPort(), port);
}

TEST_F(EndpointUnitTests, SetGetDomainName)
{
  // Given
  Endpoint endpoint;
  const std::string domain = "test.hedera.com";

  // When
  endpoint.setDomainName(domain);

  // Then
  EXPECT_EQ(endpoint.getDomainName(), domain);
}

TEST_F(EndpointUnitTests, FromProtobufWithIP)
{
  // Given
  proto::ServiceEndpoint protoEndpoint;
  std::string ipStr;
  ipStr.push_back(static_cast<char>(127));
  ipStr.push_back(static_cast<char>(0));
  ipStr.push_back(static_cast<char>(0));
  ipStr.push_back(static_cast<char>(1));

  protoEndpoint.set_ipaddressv4(ipStr);
  protoEndpoint.set_port(50211);

  // When
  Endpoint endpoint = Endpoint::fromProtobuf(protoEndpoint);

  // Then
  EXPECT_EQ(endpoint.getAddress().toString(), "127.0.0.1");
  EXPECT_EQ(endpoint.getPort(), 50211);
}

TEST_F(EndpointUnitTests, FromProtobufWithDomain)
{
  // Given
  proto::ServiceEndpoint protoEndpoint;
  protoEndpoint.set_domain_name("node.testnet.hedera.com");
  protoEndpoint.set_port(80);

  // When
  Endpoint endpoint = Endpoint::fromProtobuf(protoEndpoint);

  // Then
  EXPECT_EQ(endpoint.getDomainName(), "node.testnet.hedera.com");
  EXPECT_EQ(endpoint.getPort(), 80);
}

TEST_F(EndpointUnitTests, ToProtobufWithIP)
{
  // Given
  Endpoint endpoint;
  const std::vector<std::byte> bytes = { std::byte(10), std::byte(0), std::byte(0), std::byte(5) };
  endpoint.setAddress(IPv4Address::fromBytes(bytes));
  endpoint.setPort(443);

  // When
  std::unique_ptr<proto::ServiceEndpoint> protoEndpoint = endpoint.toProtobuf();

  // Then
  // In proto3, string/bytes fields don't have has_ methods unless optional.
  // Check emptiness instead.
  EXPECT_FALSE(protoEndpoint->ipaddressv4().empty());

  EXPECT_EQ(protoEndpoint->ipaddressv4().size(), 4);
  EXPECT_EQ(protoEndpoint->ipaddressv4()[0], static_cast<char>(10));
  EXPECT_EQ(protoEndpoint->ipaddressv4()[3], static_cast<char>(5));
  EXPECT_EQ(protoEndpoint->port(), 443);
}

TEST_F(EndpointUnitTests, ToProtobufWithDomain)
{
  // Given
  Endpoint endpoint;
  endpoint.setDomainName("api.hedera.com");
  endpoint.setPort(50212);

  // When
  std::unique_ptr<proto::ServiceEndpoint> protoEndpoint = endpoint.toProtobuf();

  // Then
  EXPECT_EQ(protoEndpoint->domain_name(), "api.hedera.com");
  EXPECT_EQ(protoEndpoint->port(), 50212);
}

TEST_F(EndpointUnitTests, ToStringWithIP)
{
  // Given
  Endpoint endpoint;
  const std::vector<std::byte> bytes = { std::byte(192), std::byte(168), std::byte(1), std::byte(100) };
  endpoint.setAddress(IPv4Address::fromBytes(bytes));
  endpoint.setPort(8080);

  // When
  std::string str = endpoint.toString();

  // Then
  EXPECT_EQ(str, "192.168.1.100:8080");
}

TEST_F(EndpointUnitTests, ToStringWithDomain)
{
  // Given
  Endpoint endpoint;
  endpoint.setDomainName("grpc.testnet.hedera.com");
  endpoint.setPort(50211);

  // When
  std::string str = endpoint.toString();

  // Then
  EXPECT_EQ(str, "grpc.testnet.hedera.com:50211");
}