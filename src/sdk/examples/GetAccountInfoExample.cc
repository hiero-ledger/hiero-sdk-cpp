// SPDX-License-Identifier: Apache-2.0
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"

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

  const AccountInfo accountInfo =
    AccountInfoQuery().setAccountId(client.getOperatorAccountId().value()).execute(client);

  std::cout << "Account key:                         "
            << std::dynamic_pointer_cast<PublicKey>(accountInfo.mKey)->toStringRaw() << std::endl;
  std::cout << "Account receiver signature required: " << (accountInfo.mReceiverSignatureRequired ? "true" : "false")
            << std::endl;
  std::cout << "Account expiration time:             " << accountInfo.mExpirationTime.time_since_epoch().count()
            << std::endl;

  std::cout << "Account token relationships: " << std::endl;
  for (auto tr : accountInfo.mTokenRelationships)
  {
    std::cout << tr.second.toString() << std::endl;
  }

  return 0;
}
