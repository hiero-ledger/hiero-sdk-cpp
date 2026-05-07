// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeApi.h"
#include "BlockNodeServiceEndpoint.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "RegisteredNodeAddressBook.h"
#include "RegisteredNodeAddressBookQuery.h"
#include "RegisteredNodeCreateTransaction.h"
#include "RegisteredNodeDeleteTransaction.h"
#include "RegisteredNodeUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <iostream>
#include <memory>
#include <stdexcept>

using namespace Hiero;

static void printAddressBook(const RegisteredNodeAddressBook& book)
{
  std::cout << "Address book contains " << book.mRegisteredNodes.size() << " registered node(s)." << std::endl;
  for (const auto& node : book.mRegisteredNodes)
  {
    std::cout << "  Node ID: " << node.mRegisteredNodeId;
    if (!node.mDescription.empty())
    {
      std::cout << "  Description: " << node.mDescription;
    }
    std::cout << std::endl;
  }
}

static uint64_t createRegisteredNode(const Client& client,
                                     const std::shared_ptr<ED25519PrivateKey>& adminKey,
                                     const std::shared_ptr<BlockNodeServiceEndpoint>& endpoint)
{
  const TransactionReceipt receipt = RegisteredNodeCreateTransaction()
                                       .setAdminKey(adminKey->getPublicKey())
                                       .setDescription("My Block Node")
                                       .addServiceEndpoint(endpoint)
                                       .freezeWith(&client)
                                       .sign(adminKey)
                                       .execute(client)
                                       .getReceipt(client);

  if (!receipt.mRegisteredNodeId.has_value() || receipt.mRegisteredNodeId.value() == 0)
  {
    throw std::runtime_error("RegisteredNodeCreate did not return a valid registeredNodeId");
  }
  return receipt.mRegisteredNodeId.value();
}

static void updateRegisteredNode(const Client& client,
                                 const std::shared_ptr<ED25519PrivateKey>& adminKey,
                                 uint64_t nodeId,
                                 const std::shared_ptr<BlockNodeServiceEndpoint>& ep1,
                                 const std::shared_ptr<BlockNodeServiceEndpoint>& ep2)
{
  RegisteredNodeUpdateTransaction()
    .setRegisteredNodeId(nodeId)
    .setDescription("My Updated Block Node")
    .addServiceEndpoint(ep1)
    .addServiceEndpoint(ep2)
    .freezeWith(&client)
    .sign(adminKey)
    .execute(client)
    .getReceipt(client)
    .validateStatus();
}

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cout << "Usage: " << argv[0] << " <operatorAccountId> <operatorPrivateKey>" << std::endl;
    return 1;
  }

  Client client = Client::forTestnet();
  client.setOperator(AccountId::fromString(argv[1]), ED25519PrivateKey::fromString(argv[2]));

  const std::shared_ptr<ED25519PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  std::cout << "Admin key generated: " << adminKey->getPublicKey()->toStringDer() << std::endl;

  auto endpoint = std::make_shared<BlockNodeServiceEndpoint>();
  endpoint->setIpAddress({ std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) });
  endpoint->setPort(8080);
  endpoint->setRequiresTls(true);
  endpoint->setEndpointApi(BlockNodeApi::SUBSCRIBE_STREAM);

  const uint64_t registeredNodeId = createRegisteredNode(client, adminKey, endpoint);
  std::cout << "Registered node created with ID: " << registeredNodeId << std::endl;

  printAddressBook(RegisteredNodeAddressBookQuery().execute(client));

  auto secondEndpoint = std::make_shared<BlockNodeServiceEndpoint>();
  secondEndpoint->setDomainName("block-node.example.com");
  secondEndpoint->setPort(8443);
  secondEndpoint->setRequiresTls(true);
  secondEndpoint->setEndpointApi(BlockNodeApi::STATUS);

  updateRegisteredNode(client, adminKey, registeredNodeId, endpoint, secondEndpoint);
  std::cout << "Registered node updated successfully." << std::endl;

  RegisteredNodeDeleteTransaction()
    .setRegisteredNodeId(registeredNodeId)
    .freezeWith(&client)
    .sign(adminKey)
    .execute(client)
    .getReceipt(client)
    .validateStatus();
  std::cout << "Registered node " << registeredNodeId << " deleted successfully." << std::endl;

  return 0;
}
