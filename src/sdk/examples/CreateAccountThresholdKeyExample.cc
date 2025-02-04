// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "KeyList.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

#include <dotenv.h>
#include <iostream>
#include <memory>
#include <vector>

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

  // Generate three ED25519 private keys.
  const std::vector<std::shared_ptr<PrivateKey>> privateKeys = { ED25519PrivateKey::generatePrivateKey(),
                                                                 ED25519PrivateKey::generatePrivateKey(),
                                                                 ED25519PrivateKey::generatePrivateKey() };

  // Create a Keylist from the list of generated private keys that require 2 of the 3 keys to sign.
  auto keys = std::make_shared<KeyList>(KeyList::of({ privateKeys.at(0), privateKeys.at(1), privateKeys.at(2) }));
  keys->setThreshold(2);

  // Create a new account with an initial balance of 1000 tinybars that uses the KeyList as its key.
  TransactionReceipt txReceipt =
    AccountCreateTransaction().setKey(keys).setInitialBalance(Hbar(10LL)).execute(client).getReceipt(client);

  const AccountId newAccountId = txReceipt.mAccountId.value();
  std::cout << "Created new account with ID " << newAccountId.toString() << std::endl;

  // Transfer out of this account requires 2 out of the three keys to sign.
  std::cout << "Transferring Hbar from new account: "
            << gStatusToString.at(TransferTransaction()
                                    .addHbarTransfer(operatorAccountId, Hbar(9LL))
                                    .addHbarTransfer(newAccountId, Hbar(9LL).negated())
                                    .freezeWith(&client)
                                    .sign(privateKeys.at(0))
                                    .sign(privateKeys.at(1))
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Get the account balance.
  std::cout << "New account balance: "
            << AccountBalanceQuery().setAccountId(newAccountId).execute(client).mBalance.toString() << std::endl;

  return 0;
}
