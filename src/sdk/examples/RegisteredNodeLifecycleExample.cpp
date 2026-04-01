// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "NodeUpdateTransaction.h"
#include "RegisteredNodeAddressBookQuery.h"
#include "RegisteredNodeCreateTransaction.h"
#include "RegisteredNodeDeleteTransaction.h"
#include "RegisteredNodeUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <iostream>
#include <memory>

using namespace Hiero;

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " <operatorAccountId> <operatorPrivateKey>" << std::endl;
    return 1;
  }

  // Create the Hiero client.
  Client client = Client::forTestnet();
  client.setOperator(AccountId::fromString(argv[1]), ED25519PrivateKey::fromString(argv[2]));

  // Step 1: Generate a new key pair to serve as the registered node's adminKey.
  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  std::cout << "Admin key generated: " << adminKey->getPublicKey()->toStringDer() << std::endl;

  // Step 2: Create a BlockNodeServiceEndpoint with an IP address, port, TLS enabled, and SUBSCRIBE_STREAM API.
  auto endpoint = std::make_shared<BlockNodeServiceEndpoint>();
  endpoint->setIpAddress({ std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) });
  endpoint->setPort(8080);
  endpoint->setRequiresTls(true);
  endpoint->setEndpointApi(BlockNodeApi::SUBSCRIBE_STREAM);

  // Steps 3-4: Build and execute a RegisteredNodeCreateTransaction.
  const TransactionResponse createResponse =
    RegisteredNodeCreateTransaction()
      .setAdminKey(adminKey->getPublicKey())
      .setDescription("My Block Node")
      .addServiceEndpoint(endpoint)
      .freezeWith(&client)
      .sign(adminKey)
      .execute(client);

  // Step 5: Retrieve the TransactionReceipt and read the registeredNodeId.
  const TransactionReceipt createReceipt = createResponse.getReceipt(client);

  // Step 6: Verify the registeredNodeId is present and non-zero.
  if (!createReceipt.mRegisteredNodeId.has_value() || createReceipt.mRegisteredNodeId.value() == 0)
  {
    std::cerr << "ERROR: RegisteredNodeCreate did not return a valid registeredNodeId" << std::endl;
    return 1;
  }

  const uint64_t registeredNodeId = createReceipt.mRegisteredNodeId.value();
  std::cout << "Registered node created with ID: " << registeredNodeId << std::endl;

  // Step 7: Execute a RegisteredNodeAddressBookQuery to confirm the node is discoverable.
  // NOTE: This is a stub — implementation deferred until mirror node API is available.
  // const RegisteredNodeAddressBook addressBook = RegisteredNodeAddressBookQuery().execute(client);

  // Step 8: Create a second endpoint with a domain name and STATUS api.
  auto secondEndpoint = std::make_shared<BlockNodeServiceEndpoint>();
  secondEndpoint->setDomainName("block-node.example.com");
  secondEndpoint->setPort(8443);
  secondEndpoint->setRequiresTls(true);
  secondEndpoint->setEndpointApi(BlockNodeApi::STATUS);

  // Steps 9-11: Build and execute a RegisteredNodeUpdateTransaction.
  const TransactionResponse updateResponse =
    RegisteredNodeUpdateTransaction()
      .setRegisteredNodeId(registeredNodeId)
      .setDescription("My Updated Block Node")
      .addServiceEndpoint(endpoint)
      .addServiceEndpoint(secondEndpoint)
      .freezeWith(&client)
      .sign(adminKey)
      .execute(client);

  updateResponse.getReceipt(client).validateStatus();
  std::cout << "Registered node updated successfully." << std::endl;

  // Steps 12-14: Associate the registered node with an existing consensus node via NodeUpdateTransaction.
  // Replace "0.0.3" with an actual consensus node ID for your network.
  // const TransactionResponse nodeUpdateResponse =
  //   NodeUpdateTransaction()
  //     .setNodeId(3ULL)
  //     .addAssociatedRegisteredNode(registeredNodeId)
  //     .execute(client);
  // nodeUpdateResponse.getReceipt(client).validateStatus();
  // std::cout << "Consensus node updated with registered node association." << std::endl;

  // Steps 15-17: Delete the registered node.
  const TransactionResponse deleteResponse =
    RegisteredNodeDeleteTransaction()
      .setRegisteredNodeId(registeredNodeId)
      .freezeWith(&client)
      .sign(adminKey)
      .execute(client);

  deleteResponse.getReceipt(client).validateStatus();
  std::cout << "Registered node " << registeredNodeId << " deleted successfully." << std::endl;

  return 0;
}
