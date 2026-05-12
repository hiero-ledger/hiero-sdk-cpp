// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "PrngTransaction.h"
#include "TransactionRecord.h"
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

  // Get a client for the configured Hiero network (defaults to testnet when HIERO_NETWORK is unset), and set
  // the operator so all generated transactions will be paid for by this account and signed by this key.
  const char* const network = std::getenv("HIERO_NETWORK");
  Client client = network ? Client::forName(network) : Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Get a random number between 0 and 100.
  const TransactionRecord txRecord = PrngTransaction().setRange(100).execute(client).getRecord(client);
  std::cout << "Randomly generated number: " << txRecord.mPrngNumber.value() << std::endl;

  return 0;
}
