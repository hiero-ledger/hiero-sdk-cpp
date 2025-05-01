// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "SnippetsHelper.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  // Initialize a Client.
  Client client = SnippetsHelper::initializeClient();

  // Generate a private key.
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();

  // Create a new account with an initial balance of 1000 tinybars. The only required field here is the key.
  const TransactionReceipt txReceipt = AccountCreateTransaction()
                                         .setKeyWithoutAlias(privateKey)
                                         .setInitialBalance(Hbar(10ULL))
                                         .execute(client)
                                         .getReceipt(client);

  const AccountId newAccountId = txReceipt.mAccountId.value();
  std::cout << "Created new account with ID " << newAccountId.toString() << std::endl;

  return 0;
}
