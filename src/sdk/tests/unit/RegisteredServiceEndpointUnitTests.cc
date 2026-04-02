// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "MirrorNodeServiceEndpoint.h"
#include "RegisteredServiceEndpoint.h"
#include "RpcRelayServiceEndpoint.h"

#include <gtest/gtest.h>
#include <services/registered_service_endpoint.pb.h>

using namespace Hiero;

class RegisteredServiceEndpointUnitTests : public ::testing::Test
{
};

//-----
TEST_F(RegisteredServiceEndpointUnitTests, BlockNodeServiceEndpointRoundTripWithIpAddress)
{
  // Given
  BlockNodeServiceEndpoint ep;
  ep.setIpAddress({ std::byte(0x7F), std::byte(0x00), std::byte(0x00), std::byte(0x01) });
  ep.setPort(8080);
  ep.setRequiresTls(true);
  ep.setEndpointApi(BlockNodeApi::SUBSCRIBE_STREAM);

  // When
  auto proto = ep.toProtobuf();
  auto ep2 = RegisteredServiceEndpoint::fromProtobuf(*proto);

  // Then
  ASSERT_NE(ep2, nullptr);
  auto* block = dynamic_cast<BlockNodeServiceEndpoint*>(ep2.get());
  ASSERT_NE(block, nullptr);
  EXPECT_TRUE(block->getIpAddress().has_value());
  EXPECT_EQ(block->getIpAddress().value(),
            (std::vector<std::byte>{ std::byte(0x7F), std::byte(0x00), std::byte(0x00), std::byte(0x01) }));
  EXPECT_EQ(block->getPort(), 8080U);
  EXPECT_TRUE(block->getRequiresTls());
  EXPECT_EQ(block->getEndpointApi(), BlockNodeApi::SUBSCRIBE_STREAM);
}

//-----
TEST_F(RegisteredServiceEndpointUnitTests, BlockNodeServiceEndpointRoundTripWithDomainName)
{
  // Given
  BlockNodeServiceEndpoint ep;
  ep.setDomainName("block-node.example.com");
  ep.setPort(443);
  ep.setRequiresTls(true);
  ep.setEndpointApi(BlockNodeApi::STATUS);

  // When
  auto proto = ep.toProtobuf();
  auto ep2 = RegisteredServiceEndpoint::fromProtobuf(*proto);

  // Then
  ASSERT_NE(ep2, nullptr);
  auto* block = dynamic_cast<BlockNodeServiceEndpoint*>(ep2.get());
  ASSERT_NE(block, nullptr);
  EXPECT_TRUE(block->getDomainName().has_value());
  EXPECT_EQ(block->getDomainName().value(), "block-node.example.com");
  EXPECT_EQ(block->getEndpointApi(), BlockNodeApi::STATUS);
}

//-----
TEST_F(RegisteredServiceEndpointUnitTests, BlockNodeServiceEndpointAllApiValues)
{
  for (auto api : { BlockNodeApi::OTHER,
                    BlockNodeApi::STATUS,
                    BlockNodeApi::PUBLISH,
                    BlockNodeApi::SUBSCRIBE_STREAM,
                    BlockNodeApi::STATE_PROOF })
  {
    BlockNodeServiceEndpoint ep;
    ep.setDomainName("test.example.com");
    ep.setPort(8080);
    ep.setEndpointApi(api);

    auto proto = ep.toProtobuf();
    auto ep2 = RegisteredServiceEndpoint::fromProtobuf(*proto);
    auto* block = dynamic_cast<BlockNodeServiceEndpoint*>(ep2.get());
    ASSERT_NE(block, nullptr);
    EXPECT_EQ(block->getEndpointApi(), api);
  }
}

//-----
TEST_F(RegisteredServiceEndpointUnitTests, MirrorNodeServiceEndpointRoundTrip)
{
  // Given
  MirrorNodeServiceEndpoint ep;
  ep.setDomainName("mirror.example.com").setPort(5551).setRequiresTls(false);

  // When
  auto proto = ep.toProtobuf();
  auto ep2 = RegisteredServiceEndpoint::fromProtobuf(*proto);

  // Then
  ASSERT_NE(ep2, nullptr);
  auto* mirror = dynamic_cast<MirrorNodeServiceEndpoint*>(ep2.get());
  ASSERT_NE(mirror, nullptr);
  EXPECT_TRUE(mirror->getDomainName().has_value());
  EXPECT_EQ(mirror->getDomainName().value(), "mirror.example.com");
  EXPECT_EQ(mirror->getPort(), 5551U);
  EXPECT_FALSE(mirror->getRequiresTls());
}

//-----
TEST_F(RegisteredServiceEndpointUnitTests, RpcRelayServiceEndpointRoundTrip)
{
  // Given
  RpcRelayServiceEndpoint ep;
  ep.setDomainName("relay.example.com").setPort(7546).setRequiresTls(true);

  // When
  auto proto = ep.toProtobuf();
  auto ep2 = RegisteredServiceEndpoint::fromProtobuf(*proto);

  // Then
  ASSERT_NE(ep2, nullptr);
  auto* relay = dynamic_cast<RpcRelayServiceEndpoint*>(ep2.get());
  ASSERT_NE(relay, nullptr);
  EXPECT_TRUE(relay->getDomainName().has_value());
  EXPECT_EQ(relay->getDomainName().value(), "relay.example.com");
  EXPECT_EQ(relay->getPort(), 7546U);
  EXPECT_TRUE(relay->getRequiresTls());
}

//-----
TEST_F(RegisteredServiceEndpointUnitTests, FromProtobufThrowsWhenEndpointTypeNotSet)
{
  // Given: proto with no endpoint_type set
  com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint proto;
  proto.set_domain_name("test.example.com");
  proto.set_port(8080);
  // endpoint_type not set

  // When / Then
  EXPECT_THROW(RegisteredServiceEndpoint::fromProtobuf(proto), std::invalid_argument);
}

//-----
TEST_F(RegisteredServiceEndpointUnitTests, SetIpAddressClearsDomainName)
{
  // Given
  BlockNodeServiceEndpoint ep;
  ep.setDomainName("test.example.com");
  ASSERT_TRUE(ep.getDomainName().has_value());

  // When
  ep.setIpAddress({ std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) });

  // Then
  EXPECT_TRUE(ep.getIpAddress().has_value());
  EXPECT_FALSE(ep.getDomainName().has_value());
}

//-----
TEST_F(RegisteredServiceEndpointUnitTests, SetDomainNameClearsIpAddress)
{
  // Given
  BlockNodeServiceEndpoint ep;
  ep.setIpAddress({ std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) });
  ASSERT_TRUE(ep.getIpAddress().has_value());

  // When
  ep.setDomainName("test.example.com");

  // Then
  EXPECT_FALSE(ep.getIpAddress().has_value());
  EXPECT_TRUE(ep.getDomainName().has_value());
}
