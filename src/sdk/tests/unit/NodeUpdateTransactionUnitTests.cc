// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "NodeUpdateTransaction.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class NodeUpdateTransactionUnitTests : public ::testing::Test
{
protected:
  NodeUpdateTransaction transaction;
  Endpoint endpoint = Endpoint()
                        .setAddress(IPv4Address::fromBytes({
                          std::byte(0x01),
                          std::byte(0x01),
                          std::byte(0x01),
                          std::byte(0x01),
                        }))
                        .setPort(50211);
};

//-----
TEST_F(NodeUpdateTransactionUnitTests, ConstructNodeUpdateTransactionFromTransactionBodyProtobuf)
{
  // Given
  proto::TransactionBody transactionBody;
  aproto::NodeUpdateTransactionBody* body = transactionBody.mutable_nodeupdate();

  // Set AccountId
  proto::AccountID* accountId = body->mutable_account_id();
  accountId->set_shardnum(1);
  accountId->set_realmnum(2);
  accountId->set_accountnum(3);

  // Set Description
  std::string description = "Test Node";
  auto value = std::make_unique<google::protobuf::StringValue>();
  value->set_value(description);
  body->set_allocated_description(value.release());

  // Set Gossip Endpoints
  body->mutable_gossip_endpoint()->AddAllocated(endpoint.toProtobuf().release());

  // Set Service Endpoints
  body->mutable_service_endpoint()->AddAllocated(endpoint.toProtobuf().release());

  // Set Gossip CA Certificate
  const std::vector<std::byte> bytes = { std::byte(0x01), std::byte(0x02) };
  body->mutable_gossip_ca_certificate()->set_value(internal::Utilities::byteVectorToString(bytes));

  // Set GRPC Certificate Hash
  body->mutable_grpc_certificate_hash()->set_value(internal::Utilities::byteVectorToString(bytes));

  // Set Admin Key
  const std::shared_ptr<ED25519PrivateKey> key = ED25519PrivateKey::generatePrivateKey();
  body->set_allocated_admin_key(key->toProtobufKey().release());

  // When
  NodeUpdateTransaction nodeUpdateTransaction(transactionBody);

  // Then
  EXPECT_EQ(nodeUpdateTransaction.getAccountId().mShardNum, 1ULL);
  EXPECT_EQ(nodeUpdateTransaction.getAccountId().mRealmNum, 2ULL);
  EXPECT_TRUE(nodeUpdateTransaction.getAccountId().mAccountNum.has_value());
  EXPECT_EQ(*nodeUpdateTransaction.getAccountId().mAccountNum, 3ULL);
  EXPECT_EQ(nodeUpdateTransaction.getDescription(), description);
  ASSERT_EQ(nodeUpdateTransaction.getGossipEndpoints().size(), 1);
  ASSERT_EQ(nodeUpdateTransaction.getServiceEndpoints().size(), 1);
  EXPECT_EQ(nodeUpdateTransaction.getGossipCaCertificate(), bytes);
  EXPECT_EQ(nodeUpdateTransaction.getGrpcCertificateHash(), bytes);
  EXPECT_EQ(nodeUpdateTransaction.getAdminKey()->toProtobufKey()->SerializeAsString(),
            key->toProtobufKey()->SerializeAsString());
}

//-----
TEST_F(NodeUpdateTransactionUnitTests, SetAndGetAccountId)
{
  // Given
  AccountId accountId(1234);

  // When
  transaction.setAccountId(accountId);

  // Then
  ASSERT_EQ(transaction.getAccountId(), accountId);
}

//-----
TEST_F(NodeUpdateTransactionUnitTests, SetAndGetDescription)
{
  // Given
  std::string description = "Node Description";

  // When
  transaction.setDescription(description);

  // Then
  ASSERT_TRUE(transaction.getDescription().has_value());
  ASSERT_EQ(transaction.getDescription().value(), description);
}

//-----
TEST_F(NodeUpdateTransactionUnitTests, SetAndGetGossipEndpoints)
{
  // Given
  std::vector<Endpoint> gossipEndpoints = { endpoint };

  // When
  transaction.setGossipEndpoints(gossipEndpoints);

  // Then
  ASSERT_EQ(transaction.getGossipEndpoints().size(), gossipEndpoints.size());
}

//-----
TEST_F(NodeUpdateTransactionUnitTests, SetAndGetServiceEndpoints)
{
  // Given
  std::vector<Endpoint> serviceEndpoints = { endpoint };

  // When
  transaction.setServiceEndpoints(serviceEndpoints);

  // Then
  ASSERT_EQ(transaction.getServiceEndpoints().size(), serviceEndpoints.size());
}

//-----
TEST_F(NodeUpdateTransactionUnitTests, SetAndGetGossipCaCertificate)
{
  // Given
  std::vector<std::byte> caCertificate = { std::byte{ 0x01 }, std::byte{ 0x02 }, std::byte{ 0x03 } };

  // When
  transaction.setGossipCaCertificate(caCertificate);

  // Then
  ASSERT_EQ(transaction.getGossipCaCertificate(), caCertificate);
}

//-----
TEST_F(NodeUpdateTransactionUnitTests, SetAndGetGrpcCertificateHash)
{
  // Given
  std::vector<std::byte> grpcHash = { std::byte{ 0x04 }, std::byte{ 0x05 } };

  // When
  transaction.setGrpcCertificateHash(grpcHash);

  // Then
  ASSERT_TRUE(transaction.getGrpcCertificateHash().has_value());
  ASSERT_EQ(transaction.getGrpcCertificateHash().value(), grpcHash);
}

//-----
TEST_F(NodeUpdateTransactionUnitTests, SetAndGetAdminKey)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();

  // When
  transaction.setAdminKey(adminKey);

  // Then
  ASSERT_EQ(transaction.getAdminKey(), adminKey);
}
