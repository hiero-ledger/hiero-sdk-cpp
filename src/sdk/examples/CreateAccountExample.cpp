// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey =
    ECDSAsecp256k1PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the configured Hiero network (defaults to testnet when HIERO_NETWORK is unset), and set
  // the operator so all generated transactions will be paid for by this account and signed by this key.
  const char* const network = std::getenv("HIERO_NETWORK");
  Client client = network ? Client::forName(network) : Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Generate an ECDSA secp256k1 private, public key pair
  const std::unique_ptr<PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  std::cout << "Generated private key: " << privateKey->toStringRaw() << std::endl;
  std::cout << "Generated public key: " << publicKey->toStringRaw() << std::endl;

  // Create a new account with an initial balance of 1000 tinybars. The only required field here is the key.
  TransactionResponse txResp = AccountCreateTransaction()
                                 .setKeyWithoutAlias(publicKey)
                                 .setInitialBalance(Hbar(1000ULL, HbarUnit::TINYBAR()))
                                 .execute(client);

  // Get the receipt when it becomes available
  TransactionReceipt txReceipt = txResp.getReceipt(client);

  const AccountId newAccountId = txReceipt.mAccountId.value();
  std::cout << "Created new account with ID " << newAccountId.toString() << std::endl;

  return 0;
}
