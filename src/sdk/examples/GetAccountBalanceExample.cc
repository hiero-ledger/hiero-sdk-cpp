// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "Client.h"
#include "ED25519PrivateKey.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));

  // Get a client for the Hiero testnet
  Client client = Client::forTestnet();

  // Because AccountBalanceQuery is a free query, we can make it without setting an operator on the client.
  const AccountBalance accountBalance = AccountBalanceQuery().setAccountId(operatorAccountId).execute(client);

  std::cout << "Balance of account " << operatorAccountId.toString() << " is " << accountBalance.mBalance.toString()
            << std::endl;

  std::cout << "Token balances of account" << std::endl;
  for (auto tokenBalance : accountBalance.mTokens)
  {
    std::cout << tokenBalance.first.toString() << ": " << tokenBalance.second << std::endl;
  }

  return 0;
}
