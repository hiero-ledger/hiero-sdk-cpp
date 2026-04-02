// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "ED25519PrivateKey.h"
#include "NodeUpdateTransaction.h"
#include "RegisteredNodeCreateTransaction.h"
#include "RegisteredNodeDeleteTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class RegisteredNodeDeleteTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] std::shared_ptr<BlockNodeServiceEndpoint> makeBlockNodeEndpoint() const
  {
    auto ep = std::make_shared<BlockNodeServiceEndpoint>();
    ep->setIpAddress({ std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) })
      .setPort(8080)
      .setRequiresTls(true);
    ep->setEndpointApi(BlockNodeApi::SUBSCRIBE_STREAM);
    return ep;
  }

  [[nodiscard]] uint64_t createRegisteredNode(const std::shared_ptr<ED25519PrivateKey>& adminKey) const
  {
    const TransactionResponse txResponse = RegisteredNodeCreateTransaction()
                                             .setAdminKey(adminKey->getPublicKey())
                                             .addServiceEndpoint(makeBlockNodeEndpoint())
                                             .freezeWith(&getTestClient())
                                             .sign(adminKey)
                                             .execute(getTestClient());
    return txResponse.getReceipt(getTestClient()).mRegisteredNodeId.value();
  }
};

//-----
TEST_F(RegisteredNodeDeleteTransactionIntegrationTests,
       DISABLED_CanDeleteRegisteredNode)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nodeId = createRegisteredNode(adminKey);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeDeleteTransaction()
                                 .setRegisteredNodeId(nodeId)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  ASSERT_NO_THROW(txResponse.getReceipt(getTestClient()).validateStatus());
}

//-----
TEST_F(RegisteredNodeDeleteTransactionIntegrationTests,
       DISABLED_FailsToDeleteAlreadyDeletedNode)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nodeId = createRegisteredNode(adminKey);

  // Delete the node first
  RegisteredNodeDeleteTransaction()
    .setRegisteredNodeId(nodeId)
    .freezeWith(&getTestClient())
    .sign(adminKey)
    .execute(getTestClient())
    .getReceipt(getTestClient());

  // When / Then: deleting again should fail
  EXPECT_THROW(RegisteredNodeDeleteTransaction()
                 .setRegisteredNodeId(nodeId)
                 .freezeWith(&getTestClient())
                 .sign(adminKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(RegisteredNodeDeleteTransactionIntegrationTests,
       DISABLED_FailsToDeleteNonExistentNode)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nonExistentId = 9999999ULL;

  // When / Then
  EXPECT_THROW(RegisteredNodeDeleteTransaction()
                 .setRegisteredNodeId(nonExistentId)
                 .freezeWith(&getTestClient())
                 .sign(adminKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(RegisteredNodeDeleteTransactionIntegrationTests,
       DISABLED_CanCreateConsensusNodeWithAssociatedRegisteredNodes)
{
  // Given: create a registered node first
  const std::shared_ptr<ED25519PrivateKey> regNodeAdminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t registeredNodeId = createRegisteredNode(regNodeAdminKey);

  // When: create a consensus node referencing the registered node
  // Note: NodeCreateTransaction requires privileged access (council signing).
  // This test is a placeholder to demonstrate the API.
  // In a real test environment, getAccountId() and other parameters would come from the test fixture.
  // ASSERT_NO_THROW(NodeCreateTransaction()
  //   .setAccountId(AccountId::fromString("0.0.4"))
  //   .addAssociatedRegisteredNode(registeredNodeId)
  //   .execute(getTestClient()));

  // Cleanup
  RegisteredNodeDeleteTransaction()
    .setRegisteredNodeId(registeredNodeId)
    .freezeWith(&getTestClient())
    .sign(regNodeAdminKey)
    .execute(getTestClient())
    .getReceipt(getTestClient());
}

//-----
TEST_F(RegisteredNodeDeleteTransactionIntegrationTests,
       DISABLED_CanUpdateConsensusNodeWithAssociatedRegisteredNodes)
{
  // Given: create a registered node
  const std::shared_ptr<ED25519PrivateKey> regNodeAdminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t registeredNodeId = createRegisteredNode(regNodeAdminKey);

  // When: update a consensus node to add the registered node association
  // Note: NodeUpdateTransaction requires privileged access (admin key signing).
  // This test is a placeholder to demonstrate the API.
  // const std::shared_ptr<ED25519PrivateKey> consensusAdminKey = ...;
  // ASSERT_NO_THROW(NodeUpdateTransaction()
  //   .setNodeId(3ULL)
  //   .addAssociatedRegisteredNode(registeredNodeId)
  //   .execute(getTestClient()));

  // Cleanup
  RegisteredNodeDeleteTransaction()
    .setRegisteredNodeId(registeredNodeId)
    .freezeWith(&getTestClient())
    .sign(regNodeAdminKey)
    .execute(getTestClient())
    .getReceipt(getTestClient());
}
