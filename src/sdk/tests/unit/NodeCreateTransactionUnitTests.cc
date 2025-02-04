// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "NodeCreateTransaction.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class NodeCreateTransactionUnitTests : public ::testing::Test
{
protected:
  NodeCreateTransaction transaction;
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
TEST_F(NodeCreateTransactionUnitTests, ConstructNodeCreateTransactionFromTransactionBodyProtobuf)
{
  // Given
  proto::TransactionBody transactionBody;
  aproto::NodeCreateTransactionBody* body = transactionBody.mutable_nodecreate();

  // Set AccountId
  proto::AccountID* accountId = body->mutable_account_id();
  accountId->set_shardnum(1);
  accountId->set_realmnum(2);
  accountId->set_accountnum(3);

  // Set Description
  const std::string description = "Test Node";
  body->set_description(description);

  // Set Gossip Endpoints
  body->mutable_gossip_endpoint()->AddAllocated(endpoint.toProtobuf().release());

  // Set Service Endpoints
  body->mutable_service_endpoint()->AddAllocated(endpoint.toProtobuf().release());

  // Set Gossip CA Certificate
  const std::vector<std::byte> bytes = { std::byte(0x01), std::byte(0x02) };
  body->set_gossip_ca_certificate(internal::Utilities::byteVectorToString(bytes));

  // Set GRPC Certificate Hash
  body->set_grpc_certificate_hash(internal::Utilities::byteVectorToString(bytes));

  // Set Admin Key
  const std::shared_ptr<ED25519PrivateKey> key = ED25519PrivateKey::generatePrivateKey();
  body->set_allocated_admin_key(key->toProtobufKey().release());

  // When
  NodeCreateTransaction nodeCreateTransaction(transactionBody);

  // Then
  EXPECT_EQ(nodeCreateTransaction.getAccountId().mShardNum, 1ULL);
  EXPECT_EQ(nodeCreateTransaction.getAccountId().mRealmNum, 2ULL);
  EXPECT_TRUE(nodeCreateTransaction.getAccountId().mAccountNum.has_value());
  EXPECT_EQ(*nodeCreateTransaction.getAccountId().mAccountNum, 3ULL);
  EXPECT_EQ(nodeCreateTransaction.getDescription(), description);
  ASSERT_EQ(nodeCreateTransaction.getGossipEndpoints().size(), 1);
  ASSERT_EQ(nodeCreateTransaction.getServiceEndpoints().size(), 1);
  EXPECT_EQ(nodeCreateTransaction.getGossipCaCertificate(), bytes);
  EXPECT_EQ(nodeCreateTransaction.getGrpcCertificateHash(), bytes);
  EXPECT_EQ(nodeCreateTransaction.getAdminKey()->toProtobufKey()->SerializeAsString(),
            key->toProtobufKey()->SerializeAsString());
}

//-----
TEST_F(NodeCreateTransactionUnitTests, SetAndGetAccountId)
{
  // Given
  AccountId accountId(1234);

  // When
  transaction.setAccountId(accountId);

  // Then
  ASSERT_EQ(transaction.getAccountId(), accountId);
}

//-----
TEST_F(NodeCreateTransactionUnitTests, SetAndGetDescription)
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
TEST_F(NodeCreateTransactionUnitTests, SetAndGetGossipEndpoints)
{
  // Given
  std::vector<Endpoint> gossipEndpoints = { endpoint };

  // When
  transaction.setGossipEndpoints(gossipEndpoints);

  // Then
  ASSERT_EQ(transaction.getGossipEndpoints().size(), gossipEndpoints.size());
}

//-----
TEST_F(NodeCreateTransactionUnitTests, SetAndGetServiceEndpoints)
{
  // Given
  std::vector<Endpoint> serviceEndpoints = { endpoint };

  // When
  transaction.setServiceEndpoints(serviceEndpoints);

  // Then
  ASSERT_EQ(transaction.getServiceEndpoints().size(), serviceEndpoints.size());
}

//-----
TEST_F(NodeCreateTransactionUnitTests, SetAndGetGossipCaCertificate)
{
  // Given
  std::vector<std::byte> caCertificate = { std::byte{ 0x01 }, std::byte{ 0x02 }, std::byte{ 0x03 } };

  // When
  transaction.setGossipCaCertificate(caCertificate);

  // Then
  ASSERT_EQ(transaction.getGossipCaCertificate(), caCertificate);
}

//-----
TEST_F(NodeCreateTransactionUnitTests, SetAndGetGrpcCertificateHash)
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
TEST_F(NodeCreateTransactionUnitTests, SetAndGetAdminKey)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();

  // When
  transaction.setAdminKey(adminKey);

  // Then
  ASSERT_EQ(transaction.getAdminKey(), adminKey);
}
