// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"

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
   * Step 1:
   * Create batch key
   */
  const std::shared_ptr<PrivateKey> batchKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  /*
   * Step 2:
   * Create acccount - alice
   */
  std::cout << "Creating Alice account and preparing batched transfer..." << std::endl;
  const std::shared_ptr<PrivateKey> aliceKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  AccountId alice = AccountCreateTransaction()
                      .setKeyWithoutAlias(aliceKey)
                      .setInitialBalance(Hbar(15))
                      .execute(client)
                      .getReceipt(client)
                      .mAccountId.value();

  std::cout << "Created Alice: " << alice.toString() << std::endl;

  /*
   * Step 3:
   * Create client for alice
   */
  Client aliceClient = Client::forTestnet();
  aliceClient.setOperator(alice, aliceKey);

  /*
   * Step 4:
   * Batchify a transfer transaction
   */
  WrappedTransaction aliceBatchedTransfer(TransferTransaction()
                                            .addHbarTransfer(client.getOperatorAccountId().value(), Hbar(1LL))
                                            .addHbarTransfer(alice, Hbar(1LL).negated())
                                            .batchify(aliceClient, batchKey));

  /*
   * Step 5:
   * Get the balances in order to compare after the batch execution
   */
  Hbar aliceBalanceBefore = AccountBalanceQuery().setAccountId(alice).execute(client).mBalance;
  Hbar operatorBalanceBefore =
    AccountBalanceQuery().setAccountId(client.getOperatorAccountId().value()).execute(client).mBalance;

  /*
   * Step 6:
   * Execute the batch
   */
  std::cout << "Executing batch transaction..." << std::endl;
  TransactionReceipt txReceipt = BatchTransaction()
                                   .addInnerTransaction(aliceBatchedTransfer)
                                   .freezeWith(&client)
                                   .sign(batchKey)
                                   .execute(client)
                                   .getReceipt(client);

  std::cout << "Batch transaction executed" << std::endl;

  /*
   * Step 7:
   * Verify the new balances
   */
  std::cout << "Verifying the balances after batch execution..." << std::endl;
  Hbar aliceBalanceAfter = AccountBalanceQuery().setAccountId(alice).execute(client).mBalance;
  Hbar operatorBalanceAfter =
    AccountBalanceQuery().setAccountId(client.getOperatorAccountId().value()).execute(client).mBalance;

  std::cout << "Alice's initial balance: " << aliceBalanceBefore.toString()
            << ", after: " + aliceBalanceAfter.toString() << std::endl;
  std::cout << "Operator's initial balance: " << operatorBalanceBefore.toString()
            << ", after: " << operatorBalanceAfter.toString() << std::endl;

  return 0;
}
