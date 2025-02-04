// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PrivateKey.h"
#include "EvmAddress.h"
#include "Hbar.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
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

  /*
   * Transfer HBAR or tokens to a Hiero account using their public-address. Reference: [HIP-583 Expand alias support in
   * CryptoCreate & CryptoTransfer Transactions](https://hips.hedera.com/hip/hip-583)
   *
   * - Create an ECSDA private key
   * - Extract the ECDSA public key
   * - Extract the Ethereum public address
   *   - Add function to calculate the Ethereum Address to example in SDK
   *   - Ethereum account address / public-address - This is the rightmost 20 bytes of the 32 byte Keccak-256 hash of
   *     the ECDSA public key of the account. This calculation is in the manner described by the Ethereum Yellow Paper.
   * - Transfer tokens using the `TransferTransaction` to the Etherum Account Address
   *   - The From field should be a complete account that has a public address
   *   - The To field should be to a public address (to create a new account)
   * - Get the child receipt or child record to return the Hiero Account ID for the new account that was created
   * - Get the `AccountInfo` on the new account and show it is a hollow account by not having a public key
   *   - This is a hollow account in this state
   * - Use the hollow account as a transaction fee payer in a HAPI transaction
   * - Sign the transaction with ECDSA private key
   * - Get the `AccountInfo` of the account and show the account is now a complete account by returning the public
   *   key on the account
   */

  /*
   * Step 1: Create an ECDSAsecp256k1PrivateKey.
   */
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  /*
   * Step 2: Extract the ECDSAsecp256k1PublicKey.
   */
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(privateKey->getPublicKey());

  /*
   * Step 3: Extract the Ethereum public address.
   */
  const EvmAddress evmAddress = publicKey->toEvmAddress();
  std::cout << "Corresponding EVM address: " << evmAddress.toString() << std::endl;

  /*
   * Step 4: Transfer Hbar to the Ethereum account address.
   */
  const Hbar amount(10LL);
  const TransactionResponse txResponse = TransferTransaction()
                                           .addHbarTransfer(operatorAccountId, amount.negated())
                                           .addHbarTransfer(evmAddress, amount)
                                           .execute(client);

  /*
   * Step 5: Get the child receipt or child record to get the ID of the new account that was created.
   */
  const AccountId accountId = TransactionReceiptQuery()
                                .setTransactionId(txResponse.mTransactionId)
                                .setIncludeChildren(true)
                                .execute(client)
                                .mChildren.cbegin()
                                ->mAccountId.value();
  std::cout << "Generated account ID " << accountId.toString() << std::endl;

  /*
   * Step 6: Get the AccountInfo on the new account and show it is a hollow account (i.e. it has no public key).
   */
  AccountInfo accountInfo = AccountInfoQuery().setAccountId(accountId).execute(client);
  std::cout << "Account's public key: "
            << ((accountInfo.mKey == nullptr) ? "N/A" : internal::HexConverter::bytesToHex(accountInfo.mKey->toBytes()))
            << std::endl;

  /*
   * Step 7: Use the hollow account as a transaction fee payer in a HAPI transaction.
   */
  client.setOperator(accountId, privateKey);
  AccountCreateTransaction accountCreateTransaction =
    AccountCreateTransaction().setKey(ED25519PrivateKey::generatePrivateKey()).freezeWith(&client);

  /*
   * Step 8: Sign the transaction with the ECDSA private key.
   */
  accountCreateTransaction.sign(privateKey);
  std::cout << "Creating new account using the hollow account as the fee payer: "
            << gStatusToString.at(accountCreateTransaction.execute(client).getReceipt(client).mStatus) << std::endl;

  /*
   * Step 9: Get the AccountInfo of the hollow account and show the account is now complete (i.e. has a public key).
   */
  accountInfo = AccountInfoQuery().setAccountId(accountId).execute(client);
  std::cout << "Account's public key: "
            << ((accountInfo.mKey == nullptr) ? "N/A" : internal::HexConverter::bytesToHex(accountInfo.mKey->toBytes()))
            << std::endl;

  return 0;
}
