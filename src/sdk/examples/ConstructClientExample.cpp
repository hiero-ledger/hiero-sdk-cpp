// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "LedgerId.h"

#include <dotenv.h>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const std::string networkName = std::getenv("HIERO_NETWORK");

  /*
   * Here are some ways you can construct and configure a client. A client has a network and an operator.
   *
   * A Hiero network is made up of nodes -- individual servers who participate in the process of reaching consensus
   * on the order and validity of transactions on the network. Three networks you likely know of are previewnet,
   * testnet, and mainnet.
   *
   * For the purpose of connecting to it, each node has an IP address or URL and a port number. Each node also has an
   * AccountId used to refer to that node for several purposes, including the paying of fees to that node when a
   * client submits requests to it.
   *
   * You can configure what network you want a client to use -- in other words, you can specify a list of URLS and
   * port numbers with associated AccountIds, and when that client is used to execute queries and transactions, the
   * client will submit requests only to nodes in that list.
   *
   * A Client has an operator, which has an AccountId and a PublicKey, and which can sign requests. A client's
   * operator can also be configured.
   */

  // Here's the simplest way to construct a client. These clients' networks are filled with default lists of nodes that
  // are baked into the SDK. Their operators are not yet set, and trying to use them now will result in exceptions.
  Client previewClient = Client::forPreviewnet();
  Client testClient = Client::forTestnet();
  Client mainClient = Client::forMainnet();

  // We can also construct a client for previewnet, testnet, or mainnet depending on the value of a network name string.
  // If, for example, the input string equals "testnet", this client will be configured to connect to the Hiero
  // Testnet.
  Client namedNetworkClient = Client::forName(networkName);

  // Set the operator on testClient (the AccountId and PrivateKey here are fake, this is just an example).
  testClient.setOperator(
    AccountId::fromString("0.0.3"),
    ED25519PrivateKey::fromString(
      "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e10"));

  // Create a Client with a custom network.
  const std::unordered_map<std::string, AccountId> network = {
    {"2.testnet.hedera.com:50211",  AccountId(5ULL)},
    { "3.testnet.hedera.com:50211", AccountId(6ULL)}
  };
  Client customClient = Client::forNetwork(network);

  // Since the customClient's network is in this case a subset of the Hiero Testnet, we should set the LedgerId of the
  // Client to testnet's LedgerId. If we don't do this, checksum validation won't work (See ValidateChecksumExample.cc).
  // You can use customClient.getLedgerId() to check the ledger ID. If you attempt to validate a checksum against a
  // client whose ledger ID is not set, an IllegalStateException will be thrown.
  customClient.setLedgerId(LedgerId::TESTNET);

  // Let's generate a client from a config.json file. A config file may specify a network by name, or it may provide a
  // custom network in the form of a list of nodes. The config file should specify the operator, so you can use a client
  // constructed using fromConfigFile() immediately.
  Client configClient = Client::fromConfigFile("config/local_node.json");
  configClient.close();

  // Always close a Client when you're done with it.
  previewClient.close();
  testClient.close();
  mainClient.close();
  namedNetworkClient.close();
  customClient.close();

  std::cout << "Success!" << std::endl;

  return 0;
}
