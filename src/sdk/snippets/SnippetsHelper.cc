// SPDX-License-Identifier: Apache-2.0
#include "SnippetsHelper.h"

#include <AccountId.h>
#include <ED25519PrivateKey.h>
#include <PrivateKey.h>

#include <dotenv.h>

namespace Hiero::SnippetsHelper
{
//-----
Client initializeClient()
{
  // Grab the environment variables from the .env file.
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  return client;
}

} // namespace Hiero::SnippetsHelper
