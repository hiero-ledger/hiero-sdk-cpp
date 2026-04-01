// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "ED25519PrivateKey.h"
#include "RegisteredNodeUpdateTransaction.h"

#include <gtest/gtest.h>
#include <services/transaction.pb.h>

using namespace Hiero;

class RegisteredNodeUpdateTransactionUnitTests : public ::testing::Test
{
protected:
  RegisteredNodeUpdateTransaction transaction;

  std::shared_ptr<BlockNodeServiceEndpoint> makeBlockNodeEndpoint() const
  {
    auto ep = std::make_shared<BlockNodeServiceEndpoint>();
    ep->setDomainName("update-test.example.com").setPort(8080).setRequiresTls(true);
    ep->setEndpointApi(BlockNodeApi::STATUS);
    return ep;
  }
};

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests,
       ConstructRegisteredNodeUpdateTransactionFromTransactionBodyProtobuf)
{
  // Given
  proto::TransactionBody transactionBody;
  aproto::RegisteredNodeUpdateTransactionBody* body = transactionBody.mutable_registerednodeupdate();
  body->set_registered_node_id(5ULL);
  body->mutable_description()->set_value("Updated Description");

  auto* ep = body->add_service_endpoint();
  ep->set_domain_name("new.example.com");
  ep->set_port(9090);
  ep->mutable_block_node()->set_endpoint_api(
    com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_STATUS);

  // When
  RegisteredNodeUpdateTransaction tx(transactionBody);

  // Then
  EXPECT_EQ(tx.getRegisteredNodeId(), 5ULL);
  ASSERT_TRUE(tx.getDescription().has_value());
  EXPECT_EQ(tx.getDescription().value(), "Updated Description");
  ASSERT_EQ(tx.getServiceEndpoints().size(), 1ULL);
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests,
       ConstructFromTransactionBodyWithNoOptionalFields)
{
  // Given: only required field set
  proto::TransactionBody transactionBody;
  aproto::RegisteredNodeUpdateTransactionBody* body = transactionBody.mutable_registerednodeupdate();
  body->set_registered_node_id(3ULL);

  // When
  RegisteredNodeUpdateTransaction tx(transactionBody);

  // Then
  EXPECT_EQ(tx.getRegisteredNodeId(), 3ULL);
  EXPECT_FALSE(tx.getDescription().has_value());
  EXPECT_EQ(tx.getAdminKey(), nullptr);
  EXPECT_TRUE(tx.getServiceEndpoints().empty());
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests, SetAndGetRegisteredNodeId)
{
  // Given / When
  transaction.setRegisteredNodeId(12ULL);

  // Then
  EXPECT_EQ(transaction.getRegisteredNodeId(), 12ULL);
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests, SetAndGetAdminKey)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> key = ED25519PrivateKey::generatePrivateKey();

  // When
  transaction.setAdminKey(key->getPublicKey());

  // Then
  EXPECT_NE(transaction.getAdminKey(), nullptr);
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests, SetAndGetDescription)
{
  // Given / When
  transaction.setDescription("New Description");

  // Then
  ASSERT_TRUE(transaction.getDescription().has_value());
  EXPECT_EQ(transaction.getDescription().value(), "New Description");
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests, ClearDescriptionResetsOptional)
{
  // Given
  transaction.setDescription("Some Description");
  ASSERT_TRUE(transaction.getDescription().has_value());

  // When
  transaction.clearDescription();

  // Then
  EXPECT_FALSE(transaction.getDescription().has_value());
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests, AddServiceEndpoint)
{
  // Given / When
  transaction.addServiceEndpoint(makeBlockNodeEndpoint());

  // Then
  EXPECT_EQ(transaction.getServiceEndpoints().size(), 1ULL);
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests, ClearServiceEndpoints)
{
  // Given
  transaction.addServiceEndpoint(makeBlockNodeEndpoint());
  ASSERT_EQ(transaction.getServiceEndpoints().size(), 1ULL);

  // When
  transaction.clearServiceEndpoints();

  // Then
  EXPECT_TRUE(transaction.getServiceEndpoints().empty());
}

//-----
TEST_F(RegisteredNodeUpdateTransactionUnitTests, SerializeDeserializeRoundTrip)
{
  // Given / When: manually build proto body and reconstruct
  proto::TransactionBody txBody;
  auto* body = txBody.mutable_registerednodeupdate();
  body->set_registered_node_id(20ULL);
  body->mutable_description()->set_value("Round Trip");
  auto* ep = body->add_service_endpoint();
  ep->set_domain_name("update-test.example.com");
  ep->set_port(8080);
  ep->mutable_block_node()->set_endpoint_api(
    com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_STATUS);

  RegisteredNodeUpdateTransaction tx2(txBody);

  // Then
  EXPECT_EQ(tx2.getRegisteredNodeId(), 20ULL);
  ASSERT_TRUE(tx2.getDescription().has_value());
  EXPECT_EQ(tx2.getDescription().value(), "Round Trip");
  ASSERT_EQ(tx2.getServiceEndpoints().size(), 1ULL);
}
