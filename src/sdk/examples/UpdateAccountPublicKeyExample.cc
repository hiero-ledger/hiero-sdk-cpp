// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "AccountUpdateTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/HexConverter.h"

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

  // Generate an ECDSAsecp256k1 public key to use for the new account
  const std::shared_ptr<PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  // Create a new account with an initial balance of 1000 tinybars. The only required field here is the key.
  TransactionResponse txResp =
    AccountCreateTransaction().setKey(publicKey).setInitialBalance(Hbar(1000ULL, HbarUnit::TINYBAR())).execute(client);

  // Get the receipt when it becomes available
  TransactionReceipt txReceipt = txResp.getReceipt(client);

  const AccountId newAccountId = txReceipt.mAccountId.value();
  std::cout << "Created new account with ID " << newAccountId.toString() << " and public key "
            << publicKey->toStringDer() << std::endl;

  // Generate a new ED25519PrivateKey public key with which to update the account.
  const std::shared_ptr<PrivateKey> newPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> newPublicKey = newPrivateKey->getPublicKey();

  // Update the account
  std::cout << "Updating account to use new public key: " << newPublicKey->toStringDer() << std::endl;
  txResp = AccountUpdateTransaction()
             .setAccountId(newAccountId)
             .setKey(newPublicKey)
             .freezeWith(&client)
             .sign(privateKey)
             .sign(newPrivateKey)
             .execute(client);

  txReceipt = txResp.getReceipt(client);
  std::cout << "Transaction response: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  // Query for the new account key
  const AccountInfo accountInfo = AccountInfoQuery().setAccountId(newAccountId).execute(client);
  std::cout << "New queried public key: " << internal::HexConverter::bytesToHex(accountInfo.mKey->toBytes())
            << std::endl;

  return 0;
}
