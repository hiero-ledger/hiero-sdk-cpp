// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "GeneralServiceEndpoint.h"
#include "MirrorNodeServiceEndpoint.h"
#include "RegisteredNode.h"
#include "RegisteredNodeAddressBook.h"
#include "RegisteredServiceEndpoint.h"
#include "RpcRelayServiceEndpoint.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Hiero;

class RegisteredNodeAddressBookQueryUnitTests : public ::testing::Test
{
};

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonWithEd25519AdminKey)
{
  // Given
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "42",
    "admin_key": {
      "_type": "ED25519",
      "key": "302a300506032b6570032100e0c8ec2758a5879ffac226a13c0c516b799e72e35141a0dd828f94d37988a4b"
    },
    "description": "Test ED25519 node",
    "service_endpoints": []
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  EXPECT_EQ(node.mRegisteredNodeId, 42ULL);
  EXPECT_NE(node.mAdminKey, nullptr);
  EXPECT_EQ(node.mDescription, "Test ED25519 node");
  EXPECT_TRUE(node.mServiceEndpoints.empty());
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonWithNoAdminKey)
{
  // Given
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "7",
    "description": "No key node",
    "service_endpoints": []
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  EXPECT_EQ(node.mRegisteredNodeId, 7ULL);
  EXPECT_EQ(node.mAdminKey, nullptr);
  EXPECT_EQ(node.mDescription, "No key node");
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonWithNumericId)
{
  // Given — registered_node_id as a JSON number rather than a string
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": 99,
    "service_endpoints": []
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  EXPECT_EQ(node.mRegisteredNodeId, 99ULL);
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonWithOptionalDescriptionAbsent)
{
  // Given — no "description" field at all
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "1",
    "service_endpoints": []
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  EXPECT_TRUE(node.mDescription.empty());
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonParsesBlockNodeEndpoint)
{
  // Given
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "5",
    "service_endpoints": [{
      "type": "BLOCK_NODE",
      "ip_address": "10.0.0.5",
      "port": 8080,
      "requires_tls": true,
      "block_node": { "endpoint_apis": ["STATUS"] }
    }]
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  ASSERT_EQ(node.mServiceEndpoints.size(), 1U);
  auto* block = dynamic_cast<BlockNodeServiceEndpoint*>(node.mServiceEndpoints[0].get());
  ASSERT_NE(block, nullptr);
  EXPECT_TRUE(block->getIpAddress().has_value());
  EXPECT_EQ(block->getPort(), 8080U);
  EXPECT_EQ(block->getEndpointApi(), BlockNodeApi::STATUS);
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonParsesMirrorNodeEndpoint)
{
  // Given
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "6",
    "service_endpoints": [{
      "type": "MIRROR_NODE",
      "domain_name": "mirror.example.com",
      "port": 5551,
      "requires_tls": false
    }]
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  ASSERT_EQ(node.mServiceEndpoints.size(), 1U);
  auto* mirror = dynamic_cast<MirrorNodeServiceEndpoint*>(node.mServiceEndpoints[0].get());
  ASSERT_NE(mirror, nullptr);
  EXPECT_TRUE(mirror->getDomainName().has_value());
  EXPECT_EQ(mirror->getDomainName().value(), "mirror.example.com");
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonParsesRpcRelayEndpoint)
{
  // Given
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "7",
    "service_endpoints": [{
      "type": "RPC_RELAY",
      "domain_name": "relay.example.com",
      "port": 7546,
      "requires_tls": true
    }]
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  ASSERT_EQ(node.mServiceEndpoints.size(), 1U);
  auto* relay = dynamic_cast<RpcRelayServiceEndpoint*>(node.mServiceEndpoints[0].get());
  ASSERT_NE(relay, nullptr);
  EXPECT_TRUE(relay->getDomainName().has_value());
  EXPECT_EQ(relay->getDomainName().value(), "relay.example.com");
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonParsesGeneralServiceEndpoint)
{
  // Given
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "8",
    "service_endpoints": [{
      "type": "GENERAL_SERVICE",
      "domain_name": "general.example.com",
      "port": 9090,
      "requires_tls": false,
      "general_service": { "description": "my node" }
    }]
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  ASSERT_EQ(node.mServiceEndpoints.size(), 1U);
  auto* general = dynamic_cast<GeneralServiceEndpoint*>(node.mServiceEndpoints[0].get());
  ASSERT_NE(general, nullptr);
  EXPECT_TRUE(general->getDomainName().has_value());
  EXPECT_TRUE(general->getDescription().has_value());
  EXPECT_EQ(general->getDescription().value(), "my node");
}

//-----
TEST_F(RegisteredNodeAddressBookQueryUnitTests, RegisteredNodeFromJsonParsesMultipleEndpoints)
{
  // Given
  const nlohmann::json json = nlohmann::json::parse(R"({
    "registered_node_id": "10",
    "service_endpoints": [
      {
        "type": "BLOCK_NODE",
        "ip_address": "1.2.3.4",
        "port": 8080,
        "requires_tls": true,
        "block_node": { "endpoint_apis": ["PUBLISH"] }
      },
      {
        "type": "MIRROR_NODE",
        "domain_name": "mirror.example.com",
        "port": 5551,
        "requires_tls": true
      }
    ]
  })");

  // When
  const RegisteredNode node = RegisteredNode::fromJson(json);

  // Then
  ASSERT_EQ(node.mServiceEndpoints.size(), 2U);
  EXPECT_NE(dynamic_cast<BlockNodeServiceEndpoint*>(node.mServiceEndpoints[0].get()), nullptr);
  EXPECT_NE(dynamic_cast<MirrorNodeServiceEndpoint*>(node.mServiceEndpoints[1].get()), nullptr);
}
