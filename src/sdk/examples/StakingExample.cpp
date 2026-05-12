// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
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

  // Generate a ED25519 private, public key pair
  const std::unique_ptr<PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  std::cout << "Generated private key: " << privateKey->toStringRaw() << std::endl;
  std::cout << "Generated public key: " << publicKey->toStringRaw() << std::endl;

  // Create an account and stake to account ID 0.0.3.
  const AccountId newAccountId = *AccountCreateTransaction()
                                    .setKeyWithoutAlias(publicKey)
                                    .setInitialBalance(Hbar(1LL))
                                    .setStakedAccountId(AccountId(3ULL))
                                    .execute(client)
                                    .getReceipt(client)
                                    .mAccountId;

  std::cout << "Created new account with ID " << newAccountId.toString() << std::endl;

  // Query the account info, it should show the staked account ID to be 0.0.3.
  const AccountInfo accountInfo = AccountInfoQuery().setAccountId(newAccountId).execute(client);
  std::cout << "Account ID " << newAccountId.toString() << " is staked to: "
            << ((accountInfo.mStakingInfo.mStakedAccountId.has_value())
                  ? accountInfo.mStakingInfo.mStakedAccountId->toString()
                  : "NOT STAKED")
            << std::endl;

  return 0;
}
