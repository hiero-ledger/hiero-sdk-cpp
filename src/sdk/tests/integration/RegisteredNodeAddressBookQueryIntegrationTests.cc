// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "ED25519PrivateKey.h"
#include "RegisteredNodeAddressBook.h"
#include "RegisteredNodeAddressBookQuery.h"
#include "RegisteredNodeCreateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <algorithm>
#include <gtest/gtest.h>

using namespace Hiero;

class RegisteredNodeAddressBookQueryIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] uint64_t createRegisteredNode(const std::shared_ptr<ED25519PrivateKey>& adminKey) const
  {
    auto ep = std::make_shared<BlockNodeServiceEndpoint>();
    ep->setIpAddress({ std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) })
      .setPort(8080)
      .setRequiresTls(true);
    ep->setEndpointApi(BlockNodeApi::SUBSCRIBE_STREAM);

    const TransactionResponse txResponse = RegisteredNodeCreateTransaction()
                                             .setAdminKey(adminKey->getPublicKey())
                                             .addServiceEndpoint(ep)
                                             .freezeWith(&getTestClient())
                                             .sign(adminKey)
                                             .execute(getTestClient());
    return txResponse.getReceipt(getTestClient()).mRegisteredNodeId.value();
  }
};

//-----
TEST_F(RegisteredNodeAddressBookQueryIntegrationTests, CanQueryRegisteredNodes)
{
  // Given — create a registered node so the registry is non-empty
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const uint64_t createdNodeId = createRegisteredNode(adminKey);

  // When
  RegisteredNodeAddressBook addressBook;
  ASSERT_NO_THROW(addressBook = RegisteredNodeAddressBookQuery().execute(getTestClient()));

  // Then
  EXPECT_FALSE(addressBook.mRegisteredNodes.empty());

  const bool found = std::any_of(addressBook.mRegisteredNodes.begin(),
                                 addressBook.mRegisteredNodes.end(),
                                 [createdNodeId](const auto& node) { return node.mRegisteredNodeId == createdNodeId; });
  EXPECT_TRUE(found);
}

//-----
TEST_F(RegisteredNodeAddressBookQueryIntegrationTests, CanQueryRegisteredNodesWithLimit)
{
  // Given — create a registered node and query with a page size of 1
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  createRegisteredNode(adminKey);

  // When
  RegisteredNodeAddressBook addressBook;
  ASSERT_NO_THROW(addressBook = RegisteredNodeAddressBookQuery().setLimit(1).execute(getTestClient()));

  // Then — pagination should still return all nodes across pages
  EXPECT_FALSE(addressBook.mRegisteredNodes.empty());
}
