// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey =
    ECDSAsecp256k1PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Initialize the client from a mirror node address book. The mirror node endpoint is read from
  // MIRROR_NODE (e.g. "127.0.0.1:5600" for a local solo node, or the default testnet endpoint).
  const char* const mirrorNode = std::getenv("MIRROR_NODE");
  Client client = Client::forMirrorNetwork({ mirrorNode ? mirrorNode : "testnet.mirrornode.hedera.com:443" });
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Attempt to execute a transaction.
  TransactionReceipt txReceipt = AccountCreateTransaction()
                                   .setKeyWithoutAlias(ECDSAsecp256k1PrivateKey::generatePrivateKey())
                                   .execute(client)
                                   .getReceipt(client);
  std::cout << "Created account " << txReceipt.mAccountId->toString() << std::endl;

  return 0;
}
