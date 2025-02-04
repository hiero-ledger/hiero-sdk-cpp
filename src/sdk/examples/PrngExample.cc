// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
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
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Get a random number between 0 and 100.
  const TransactionRecord txRecord = PrngTransaction().setRange(100).execute(client).getRecord(client);
  std::cout << "Randomly generated number: " << txRecord.mPrngNumber.value() << std::endl;

  return 0;
}
