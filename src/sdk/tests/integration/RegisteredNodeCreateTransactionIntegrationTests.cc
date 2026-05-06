// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "ED25519PrivateKey.h"
#include "GeneralServiceEndpoint.h"
#include "MirrorNodeServiceEndpoint.h"
#include "RegisteredNodeCreateTransaction.h"
#include "RpcRelayServiceEndpoint.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class RegisteredNodeCreateTransactionIntegrationTests : public BaseIntegrationTest
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
};

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, CanCreateRegisteredNodeWithBlockNodeEndpoint)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeCreateTransaction()
                                 .setAdminKey(adminKey->getPublicKey())
                                 .setDescription("Test Block Node")
                                 .addServiceEndpoint(makeBlockNodeEndpoint())
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_TRUE(txReceipt.mRegisteredNodeId.has_value());
  EXPECT_GT(txReceipt.mRegisteredNodeId.value(), 0ULL);
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, CanCreateRegisteredNodeWithMirrorNodeEndpoint)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  auto ep = std::make_shared<MirrorNodeServiceEndpoint>();
  ep->setDomainName("mirror.example.com").setPort(5551).setRequiresTls(true);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeCreateTransaction()
                                 .setAdminKey(adminKey->getPublicKey())
                                 .addServiceEndpoint(ep)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_TRUE(txReceipt.mRegisteredNodeId.has_value());
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, CanCreateRegisteredNodeWithRpcRelayEndpoint)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  auto ep = std::make_shared<RpcRelayServiceEndpoint>();
  ep->setDomainName("relay.example.com").setPort(7546).setRequiresTls(true);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeCreateTransaction()
                                 .setAdminKey(adminKey->getPublicKey())
                                 .addServiceEndpoint(ep)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_TRUE(txReceipt.mRegisteredNodeId.has_value());
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, CanCreateRegisteredNodeWithMixedEndpoints)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  auto mirrorEp = std::make_shared<MirrorNodeServiceEndpoint>();
  mirrorEp->setDomainName("mirror.example.com").setPort(5551).setRequiresTls(true);
  auto rpcEp = std::make_shared<RpcRelayServiceEndpoint>();
  rpcEp->setDomainName("relay.example.com").setPort(7546).setRequiresTls(true);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeCreateTransaction()
                                 .setAdminKey(adminKey->getPublicKey())
                                 .addServiceEndpoint(makeBlockNodeEndpoint())
                                 .addServiceEndpoint(mirrorEp)
                                 .addServiceEndpoint(rpcEp)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_TRUE(txReceipt.mRegisteredNodeId.has_value());
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, CanCreateRegisteredNodeWithDescription)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const std::string description = "My Block Node Description";

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeCreateTransaction()
                                 .setAdminKey(adminKey->getPublicKey())
                                 .setDescription(description)
                                 .addServiceEndpoint(makeBlockNodeEndpoint())
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_TRUE(txReceipt.mRegisteredNodeId.has_value());
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, CanCreateRegisteredNodeWithGeneralServiceEndpoint)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  auto ep = std::make_shared<GeneralServiceEndpoint>();
  ep->setDomainName("general.example.com").setPort(9090).setRequiresTls(false);
  ep->setDescription("General purpose node");

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeCreateTransaction()
                                 .setAdminKey(adminKey->getPublicKey())
                                 .setDescription("Test General Service Node")
                                 .addServiceEndpoint(ep)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceiptGeneral;
  ASSERT_NO_THROW(txReceiptGeneral = txResponse.getReceipt(getTestClient()));
  EXPECT_TRUE(txReceiptGeneral.mRegisteredNodeId.has_value());
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests,
       CanCreateRegisteredNodeWithMixedEndpointsIncludingGeneralService)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  auto generalEp = std::make_shared<GeneralServiceEndpoint>();
  generalEp->setDomainName("general.example.com").setPort(9090).setRequiresTls(false);
  auto mirrorEp = std::make_shared<MirrorNodeServiceEndpoint>();
  mirrorEp->setDomainName("mirror.example.com").setPort(5551).setRequiresTls(true);

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = RegisteredNodeCreateTransaction()
                                 .setAdminKey(adminKey->getPublicKey())
                                 .addServiceEndpoint(makeBlockNodeEndpoint())
                                 .addServiceEndpoint(mirrorEp)
                                 .addServiceEndpoint(generalEp)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceiptMixed;
  ASSERT_NO_THROW(txReceiptMixed = txResponse.getReceipt(getTestClient()));
  EXPECT_TRUE(txReceiptMixed.mRegisteredNodeId.has_value());
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, FailsToCreateRegisteredNodeWithNoAdminKey)
{
  // When / Then — no admin key set, expect KEY_REQUIRED precheck
  EXPECT_THROW(RegisteredNodeCreateTransaction().addServiceEndpoint(makeBlockNodeEndpoint()).execute(getTestClient()),
               PrecheckStatusException);
}

//-----
TEST_F(RegisteredNodeCreateTransactionIntegrationTests, FailsToCreateRegisteredNodeWithNoServiceEndpoints)
{
  // Given — admin key set but no service endpoints
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();

  // When / Then — network rejects at precheck with INVALID_REGISTERED_ENDPOINT
  EXPECT_THROW(RegisteredNodeCreateTransaction()
                 .setAdminKey(adminKey->getPublicKey())
                 .freezeWith(&getTestClient())
                 .sign(adminKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException);
}
