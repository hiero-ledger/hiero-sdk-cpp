// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "Status.h"
#include "TransactionReceipt.h"
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

  // Generate a ED25519 private, public key pair
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  std::cout << "Generated private key: " << privateKey->toStringRaw() << std::endl;
  std::cout << "Generated public key: " << publicKey->toStringRaw() << std::endl;

  // Create a new account with an initial balance of 2 Hbars. The only required field here is the key.
  TransactionResponse txResp =
    AccountCreateTransaction().setKey(publicKey).setInitialBalance(Hbar(2LL)).execute(client);

  // Get the receipt when it becomes available
  TransactionReceipt txReceipt = txResp.getReceipt(client);

  const AccountId newAccountId = txReceipt.mAccountId.value();
  std::cout << "Created new account with ID " << newAccountId.toString() << std::endl;

  // Delete the newly-created account
  txReceipt = AccountDeleteTransaction()
                .setDeleteAccountId(newAccountId)
                .setTransferAccountId(operatorAccountId)
                .freezeWith(&client)
                .sign(privateKey)
                .execute(client)
                .getReceipt(client);

  std::cout << "Deleted account with response code: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  return 0;
}
