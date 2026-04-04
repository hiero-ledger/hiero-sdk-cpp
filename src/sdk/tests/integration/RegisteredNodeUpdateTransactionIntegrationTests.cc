// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "ED25519PrivateKey.h"
#include "RegisteredNodeCreateTransaction.h"
#include "RegisteredNodeUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class RegisteredNodeUpdateTransactionIntegrationTests : public BaseIntegrationTest
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
TEST_F(RegisteredNodeUpdateTransactionIntegrationTests, CanUpdateRegisteredNodeDescription)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nodeId = createRegisteredNode(adminKey);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeUpdateTransaction()
                                 .setRegisteredNodeId(nodeId)
                                 .setDescription("Updated Description")
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  ASSERT_NO_THROW(txResponse.getReceipt(getTestClient()).validateStatus());
}

//-----
TEST_F(RegisteredNodeUpdateTransactionIntegrationTests, CanReplaceRegisteredNodeServiceEndpoints)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nodeId = createRegisteredNode(adminKey);
  auto newEndpoint = std::make_shared<BlockNodeServiceEndpoint>();
  newEndpoint->setDomainName("new-block-node.example.com").setPort(9090).setRequiresTls(true);
  newEndpoint->setEndpointApi(BlockNodeApi::STATUS);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeUpdateTransaction()
                                 .setRegisteredNodeId(nodeId)
                                 .addServiceEndpoint(newEndpoint)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  ASSERT_NO_THROW(txResponse.getReceipt(getTestClient()).validateStatus());
}

//-----
TEST_F(RegisteredNodeUpdateTransactionIntegrationTests, CanChangeAdminKeyWhenSignedByBothKeys)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> oldAdminKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ED25519PrivateKey> newAdminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nodeId = createRegisteredNode(oldAdminKey);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeUpdateTransaction()
                                 .setRegisteredNodeId(nodeId)
                                 .setAdminKey(newAdminKey->getPublicKey())
                                 .freezeWith(&getTestClient())
                                 .sign(oldAdminKey)
                                 .sign(newAdminKey)
                                 .execute(getTestClient()));

  // Then
  ASSERT_NO_THROW(txResponse.getReceipt(getTestClient()).validateStatus());
}

//-----
TEST_F(RegisteredNodeUpdateTransactionIntegrationTests, FailsToChangeAdminKeyWhenOnlyOldKeySign)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> oldAdminKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ED25519PrivateKey> newAdminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nodeId = createRegisteredNode(oldAdminKey);

  // When / Then
  EXPECT_THROW(RegisteredNodeUpdateTransaction()
                 .setRegisteredNodeId(nodeId)
                 .setAdminKey(newAdminKey->getPublicKey())
                 .freezeWith(&getTestClient())
                 .sign(oldAdminKey) // only old key — should fail
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(RegisteredNodeUpdateTransactionIntegrationTests, FailsToUpdateNonExistentRegisteredNode)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t nonExistentId = 9999999ULL;

  // When / Then
  EXPECT_THROW(RegisteredNodeUpdateTransaction()
                 .setRegisteredNodeId(nonExistentId)
                 .setDescription("No such node")
                 .freezeWith(&getTestClient())
                 .sign(adminKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException);
}
