// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "ED25519PrivateKey.h"
#include "MirrorNodeServiceEndpoint.h"
#include "RegisteredNodeCreateTransaction.h"
#include "RpcRelayServiceEndpoint.h"

#include <gtest/gtest.h>
#include <services/transaction.pb.h>

using namespace Hiero;

class RegisteredNodeCreateTransactionUnitTests : public ::testing::Test
{
protected:
  RegisteredNodeCreateTransaction transaction;

  std::shared_ptr<BlockNodeServiceEndpoint> makeBlockNodeEndpoint() const
  {
    auto ep = std::make_shared<BlockNodeServiceEndpoint>();
    ep->setIpAddress({ std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) });
    ep->setPort(8080);
    ep->setRequiresTls(true);
    ep->setEndpointApi(BlockNodeApi::SUBSCRIBE_STREAM);
    return ep;
  }
};

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, ConstructRegisteredNodeCreateTransactionFromTransactionBodyProtobuf)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  proto::TransactionBody transactionBody;
  auto* body = transactionBody.mutable_registerednodecreate();

  body->set_allocated_admin_key(adminKey->toProtobufKey().release());
  body->set_description("Test Node");

  auto* ep = body->add_service_endpoint();
  ep->set_ip_address("\x01\x02\x03\x04");
  ep->set_port(8080);
  ep->set_requires_tls(true);
  ep->mutable_block_node()->set_endpoint_api(
    com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_SUBSCRIBE_STREAM);

  // When
  RegisteredNodeCreateTransaction tx(transactionBody);

  // Then
  EXPECT_NE(tx.getAdminKey(), nullptr);
  EXPECT_EQ(tx.getDescription(), "Test Node");
  ASSERT_EQ(tx.getServiceEndpoints().size(), 1ULL);
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, SetAndGetAdminKey)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();

  // When
  transaction.setAdminKey(adminKey->getPublicKey());

  // Then
  EXPECT_NE(transaction.getAdminKey(), nullptr);
  EXPECT_EQ(transaction.getAdminKey()->toProtobufKey()->SerializeAsString(),
            adminKey->getPublicKey()->toProtobufKey()->SerializeAsString());
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, SetAndGetDescription)
{
  // Given / When
  transaction.setDescription("My Block Node");

  // Then
  EXPECT_EQ(transaction.getDescription(), "My Block Node");
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, AddServiceEndpoint)
{
  // Given / When
  transaction.addServiceEndpoint(makeBlockNodeEndpoint());

  // Then
  ASSERT_EQ(transaction.getServiceEndpoints().size(), 1ULL);
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, AddMultipleServiceEndpoints)
{
  // Given
  auto mirror = std::make_shared<MirrorNodeServiceEndpoint>();
  mirror->setDomainName("mirror.example.com").setPort(5551);
  auto rpc = std::make_shared<RpcRelayServiceEndpoint>();
  rpc->setDomainName("relay.example.com").setPort(7546);

  // When
  transaction.addServiceEndpoint(makeBlockNodeEndpoint()).addServiceEndpoint(mirror).addServiceEndpoint(rpc);

  // Then
  EXPECT_EQ(transaction.getServiceEndpoints().size(), 3ULL);
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, ClearServiceEndpoints)
{
  // Given
  transaction.addServiceEndpoint(makeBlockNodeEndpoint()).addServiceEndpoint(makeBlockNodeEndpoint());
  ASSERT_EQ(transaction.getServiceEndpoints().size(), 2ULL);

  // When
  transaction.clearServiceEndpoints();

  // Then
  EXPECT_TRUE(transaction.getServiceEndpoints().empty());
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, DefaultDescriptionIsEmpty)
{
  EXPECT_TRUE(transaction.getDescription().empty());
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, DefaultAdminKeyIsNull)
{
  EXPECT_EQ(transaction.getAdminKey(), nullptr);
}

//-----
TEST_F(RegisteredNodeCreateTransactionUnitTests, SerializeDeserializeRoundTrip)
{
  // Given / When: manually build proto body and reconstruct
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  proto::TransactionBody txBody;
  auto* body = txBody.mutable_registerednodecreate();
  body->set_allocated_admin_key(adminKey->toProtobufKey().release());
  body->set_description("Round Trip");
  auto* ep = body->add_service_endpoint();
  ep->set_ip_address("\x01\x02\x03\x04");
  ep->set_port(8080);
  ep->set_requires_tls(true);
  ep->mutable_block_node()->set_endpoint_api(
    com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_SUBSCRIBE_STREAM);

  RegisteredNodeCreateTransaction tx2(txBody);

  // Then
  EXPECT_NE(tx2.getAdminKey(), nullptr);
  EXPECT_EQ(tx2.getDescription(), "Round Trip");
  ASSERT_EQ(tx2.getServiceEndpoints().size(), 1ULL);
}
