// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
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

#include <chrono>
#include <dotenv.h>
#include <iostream>
#include <memory>
#include <thread>

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

  /**
   * Auto-create a new account using a public-address via a `TransferTransaction`. Reference: [HIP-583 Expand alias
   * support in CryptoCreate & CryptoTransfer Transactions](https://hips.hedera.com/hip/hip-583)
   *
   * - Create an ECSDA private key.
   * - Extract the ECDSA public key.
   * - Extract the Ethereum public address.
   * - Use the `TransferTransaction`.
   *    - Populate the `FromAddress` with the sender Hiero account ID.
   *    - Populate the `ToAddress` with Ethereum public address.
   *    - Note: Can transfer from public address to public address in the `TransferTransaction` for complete accounts.
   *            Transfers from hollow accounts will not work because the hollow account does not have a public key
   *            assigned to authorize transfers out of the account.
   * - Sign the `TransferTransaction` transaction using an existing Hiero account and key paying for the transaction
   *   fee.
   * - The `AccountCreateTransaction` is executed as a child transaction triggered by the `TransferTransaction`.
   * - The Hiero account that was created has a public address the user specified in the TransferTransaction ToAddress.
   *    - Will not have a public key at this stage.
   *    - Cannot do anything besides receive tokens or hbars.
   *    - The alias property of the account does not have the public address.
   *    - Referred to as a hollow account.
   * - To get the new account ID ask for the child receipts or child records for the parent transaction ID of the
   *   `TransferTransaction`.
   * - Get the `AccountInfo` and verify the account is a hollow account with the supplied public address (may need to
   *   verify with mirror node API).
   * - To enhance the hollow account to have a public key the hollow account needs to be specified as a transaction fee
   *   payer in a HAPI transaction.
   * - Create a HAPI transaction and assign the new hollow account as the transaction fee payer.
   * - Sign with the private key that corresponds to the public key on the hollow account.
   * - Get the `AccountInfo` for the account and return the public key on the account to show it is a complete account.
   */

  /**
   * Step 1: Create an ECSDA private key.
   */
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  /**
   * Step 2: Extract the ECDSA public key.
   */
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(privateKey->getPublicKey());

  /**
   * Step 3: Extract the Ethereum public address.
   */
  const EvmAddress evmAddress = publicKey->toEvmAddress();

  /**
   * Step 4: Use the `TransferTransaction` and set the EVM address field to the Ethereum public address
   */
  TransferTransaction transferTransaction = TransferTransaction()
                                              .addHbarTransfer(operatorAccountId, Hbar(10LL).negated())
                                              .addHbarTransfer(AccountId::fromEvmAddress(evmAddress), Hbar(10LL))
                                              .freezeWith(&client);

  /**
   * Step 5: Sign the `TransferTransaction` transaction using an existing Hiero account and key paying for the
   *         transaction fee.
   */
  const TransactionResponse response = transferTransaction.execute(client);

  /**
   * Step 6: To get the new account ID, ask for the child receipts or child records for the parent transaction ID of the
   *         `TransferTransaction` (the `AccountCreateTransaction` is executed as a child transaction triggered by the
   *         `TransferTransaction`).
   */
  const TransactionReceipt receipt =
    TransactionReceiptQuery().setTransactionId(response.mTransactionId).setIncludeChildren(true).execute(client);

  AccountId newAccountId = receipt.mChildren.at(0).mAccountId.value();

  // Wait some seconds for Mirror Node to update state with the newly created hollow account
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // Populate the account public EVM Address from Mirror Node
  newAccountId.populateAccountEvmAddress(client);

  // If Mirror Node contained the account public EVM Address. It should be printed instead of the Hiero AccountID
  std::cout << newAccountId.toString() << std::endl;

  return 0;
}
