/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <dotenv.h>
#include <iostream>

using namespace Hedera;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Initialize the client with the testnet mirror node.
  Client client;
  client.setOperator(operatorAccountId, operatorPrivateKey);
  client.setMirrorNetwork({ "testnet.mirrornode.hedera.com:443" });

  // Get the address book from the mirror node and use it to populate the Client's consensus network.
  client.populateNetworkFromMirrorNodeAddressBook();

  // Attempt to execute a transaction.
  TransactionReceipt txReceipt =
    AccountCreateTransaction().setKey(ED25519PrivateKey::generatePrivateKey()).execute(client).getReceipt(client);
  std::cout << "Created account " << txReceipt.mAccountId->toString() << std::endl;

  return 0;
}
