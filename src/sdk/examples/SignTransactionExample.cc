// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "KeyList.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"

#include <cstddef>
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

  // Generate a multi-sig account.
  const std::shared_ptr<ED25519PrivateKey> key1 = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> key2 = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const AccountId accountId = AccountCreateTransaction()
                                .setKey(std::make_shared<KeyList>(KeyList::of({ key1, key2 })))
                                .setInitialBalance(Hbar(5LL))
                                .execute(client)
                                .getReceipt(client)
                                .mAccountId.value();
  std::cout << "Created multi-sig account with ID " << accountId.toString() << std::endl;

  // Create a transfer of 2 Hbar from the new account to the operator account.
  TransferTransaction transferTransaction = TransferTransaction()
                                              .setNodeAccountIds({ AccountId(3ULL) })
                                              .addHbarTransfer(accountId, Hbar(-2LL))
                                              .addHbarTransfer(client.getOperatorAccountId().value(), Hbar(2LL))
                                              .freezeWith(&client);

  // Users sign the transaction with their private keys.
  key1->signTransaction(transferTransaction);
  key2->signTransaction(transferTransaction);

  // Execute the transaction with all signatures.
  transferTransaction.execute(client).getReceipt(client);

  // Get the balance of the multi-sig account.
  std::cout << "Balance of multi-sign account (should be 3 Hbar): "
            << AccountBalanceQuery().setAccountId(accountId).execute(client).mBalance.toString() << std::endl;

  return 0;
}