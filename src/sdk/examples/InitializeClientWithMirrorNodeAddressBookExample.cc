// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Initialize the client with the testnet mirror node. This will also get the address book from the mirror node and
  // use it to populate the Client's consensus network.
  Client client = Client::forMirrorNetwork({ "testnet.mirrornode.hedera.com:443" });
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Attempt to execute a transaction.
  TransactionReceipt txReceipt =
    AccountCreateTransaction().setKey(ED25519PrivateKey::generatePrivateKey()).execute(client).getReceipt(client);
  std::cout << "Created account " << txReceipt.mAccountId->toString() << std::endl;

  return 0;
}
