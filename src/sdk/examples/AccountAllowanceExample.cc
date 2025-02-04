// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/ReceiptStatusException.h"

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

  // Generate ECDSAsecp256k1 key combinations for Alice, Bob, and Charlie.
  const std::shared_ptr<PrivateKey> alicePrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> bobPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> charliePrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  std::cout << "Generated Alice private key: " << alicePrivateKey->toStringRaw() << std::endl;
  std::cout << "Generated Bob private key: " << bobPrivateKey->toStringRaw() << std::endl;
  std::cout << "Generated Charlie private key: " << charliePrivateKey->toStringRaw() << std::endl << std::endl;

  // Grab the public keys for Alice, Bob, and Charlie.
  const std::shared_ptr<PublicKey> alicePublicKey = alicePrivateKey->getPublicKey();
  const std::shared_ptr<PublicKey> bobPublicKey = bobPrivateKey->getPublicKey();
  const std::shared_ptr<PublicKey> charliePublicKey = charliePrivateKey->getPublicKey();

  // Generate accounts for Alice, Bob, and Charlie, giving each 5 Hbar.
  const AccountId aliceAccountId = AccountCreateTransaction()
                                     .setKey(alicePublicKey)
                                     .setInitialBalance(Hbar(5LL))
                                     .execute(client)
                                     .getReceipt(client)
                                     .mAccountId.value();
  std::cout << "Generated Alice account ID " << aliceAccountId.toString() << " and initialized with "
            << AccountBalanceQuery().setAccountId(aliceAccountId).execute(client).mBalance.toString() << std::endl;

  const AccountId bobAccountId = AccountCreateTransaction()
                                   .setKey(bobPublicKey)
                                   .setInitialBalance(Hbar(5LL))
                                   .execute(client)
                                   .getReceipt(client)
                                   .mAccountId.value();
  std::cout << "Generated Bob account ID " << bobAccountId.toString() << " and initialized with "
            << AccountBalanceQuery().setAccountId(bobAccountId).execute(client).mBalance.toString() << std::endl;

  const AccountId charlieAccountId = AccountCreateTransaction()
                                       .setKey(charliePublicKey)
                                       .setInitialBalance(Hbar(5LL))
                                       .execute(client)
                                       .getReceipt(client)
                                       .mAccountId.value();
  std::cout << "Generated Charlie account ID " << charlieAccountId.toString() << " and initialized with "
            << AccountBalanceQuery().setAccountId(charlieAccountId).execute(client).mBalance.toString() << std::endl
            << std::endl;

  std::cout << "Alice is now going to try and approve Bob to spend 2 of her Hbar" << std::endl;
  TransactionReceipt txReceipt = AccountAllowanceApproveTransaction()
                                   .approveHbarAllowance(aliceAccountId, bobAccountId, Hbar(2LL))
                                   .freezeWith(&client)
                                   .sign(alicePrivateKey)
                                   .execute(client)
                                   .getReceipt(client);
  std::cout << "Alice 2 Hbar allowance approval transaction status: " << gStatusToString.at(txReceipt.mStatus)
            << std::endl
            << std::endl;

  std::cout << "Transferring 1 Hbar from Alice to Charlie, but only signed by Bob" << std::endl;
  txReceipt = TransferTransaction()
                // Use "addApprovedHbarTransfer" to show that the transfer has been approved by an allowance.
                .addApprovedHbarTransfer(aliceAccountId, Hbar(-1LL))
                .addHbarTransfer(charlieAccountId, Hbar(1LL))
                // The allowance spender must pay the fee for the transaction.
                .setTransactionId(TransactionId::generate(bobAccountId))
                .freezeWith(&client)
                .sign(bobPrivateKey)
                .execute(client)
                .getReceipt(client);
  std::cout << "Transfer of 1 Hbar from Alice to Charlie, using Bob's allowance: "
            << gStatusToString.at(txReceipt.mStatus) << std::endl
            << std::endl;

  std::cout << "Going to attempt to transfer another 2 Hbar from Alice to Charlie using Bob's allowance. "
               "However, this should fail since there should only be 1 Hbar left in Bob's allowance"
            << std::endl;
  try
  {
    txReceipt = TransferTransaction()
                  .addApprovedHbarTransfer(aliceAccountId, Hbar(-2LL))
                  .addHbarTransfer(charlieAccountId, Hbar(2LL))
                  .setTransactionId(TransactionId::generate(bobAccountId))
                  .freezeWith(&client)
                  .sign(bobPrivateKey)
                  .execute(client)
                  .getReceipt(client);
    std::cout << "Transfer of 2 Hbar from Alice to Charlie, using Bob's allowance: "
              << gStatusToString.at(txReceipt.mStatus) << std::endl
              << std::endl;
  }
  catch (const ReceiptStatusException& ex)
  {
    std::cout << "Transfer of 2 Hbar from Alice to Charlie, using Bob's allowance failed: " << ex.what() << std::endl
              << std::endl;
  }

  std::cout << "Adjusting Bob's allowance to 3 Hbar" << std::endl;
  txReceipt = AccountAllowanceApproveTransaction()
                .approveHbarAllowance(aliceAccountId, bobAccountId, Hbar(3LL))
                .freezeWith(&client)
                .sign(alicePrivateKey)
                .execute(client)
                .getReceipt(client);
  std::cout << "Alice 3 Hbar allowance approval transaction status: " << gStatusToString.at(txReceipt.mStatus)
            << std::endl
            << std::endl;

  std::cout << "Going to attempt to transfer 2 Hbar again from Alice to Charlie using Bob's allowance. This time "
               "it should succeed."
            << std::endl;
  txReceipt = TransferTransaction()
                .addApprovedHbarTransfer(aliceAccountId, Hbar(-2LL))
                .addHbarTransfer(charlieAccountId, Hbar(2LL))
                .setTransactionId(TransactionId::generate(bobAccountId))
                .freezeWith(&client)
                .sign(bobPrivateKey)
                .execute(client)
                .getReceipt(client);
  std::cout << "Transfer of 2 Hbar from Alice to Charlie, using Bob's allowance: "
            << gStatusToString.at(txReceipt.mStatus) << std::endl
            << std::endl;

  std::cout << "Alice's final account balance: "
            << AccountBalanceQuery().setAccountId(aliceAccountId).execute(client).mBalance.toString() << std::endl;
  std::cout << "Bob's final account balance: "
            << AccountBalanceQuery().setAccountId(bobAccountId).execute(client).mBalance.toString() << std::endl;
  std::cout << "Charlie's final account balance: "
            << AccountBalanceQuery().setAccountId(charlieAccountId).execute(client).mBalance.toString() << std::endl
            << std::endl;

  std::cout << "Now going to attempt to delete Bob's allowance" << std::endl;
  txReceipt = AccountAllowanceApproveTransaction()
                .approveHbarAllowance(aliceAccountId, bobAccountId, Hbar(0LL))
                .freezeWith(&client)
                .sign(alicePrivateKey)
                .execute(client)
                .getReceipt(client);
  std::cout << "Alice allowance deletion for Bob: " << gStatusToString.at(txReceipt.mStatus) << std::endl << std::endl;

  std::cout << "Deleting created accounts.." << std::endl;
  txReceipt = AccountDeleteTransaction()
                .setDeleteAccountId(aliceAccountId)
                .setTransferAccountId(operatorAccountId)
                .freezeWith(&client)
                .sign(alicePrivateKey)
                .execute(client)
                .getReceipt(client);
  std::cout << "Alice account deletion: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  txReceipt = AccountDeleteTransaction()
                .setDeleteAccountId(bobAccountId)
                .setTransferAccountId(operatorAccountId)
                .freezeWith(&client)
                .sign(bobPrivateKey)
                .execute(client)
                .getReceipt(client);
  std::cout << "Bob account deletion: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  txReceipt = AccountDeleteTransaction()
                .setDeleteAccountId(charlieAccountId)
                .setTransferAccountId(operatorAccountId)
                .freezeWith(&client)
                .sign(charliePrivateKey)
                .execute(client)
                .getReceipt(client);
  std::cout << "Charlie account deletion: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  return 0;
}
