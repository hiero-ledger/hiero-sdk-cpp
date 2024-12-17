// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountUpdateTransaction.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <dotenv.h>
#include <iostream>
#include <thread>

using namespace Hiero;

int main(int argc, char** argv)
{
  // Step 0: Create and configure the SDK Client.
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Step 1: Create key pairs
  std::cout << "Creating a Key List...(with threshold, requiring 2 of 2 keys to sign)." << std::endl;

  // Generate three ED25519 private keys.
  const std::vector<std::shared_ptr<PrivateKey>> privateKeys = {
    ED25519PrivateKey::generatePrivateKey(),
    ED25519PrivateKey::generatePrivateKey(),
  };

  // Create a Keylist from the list of generated private keys that require 2 of the 3 keys to sign.
  auto keys = std::make_shared<KeyList>(KeyList::of({ privateKeys.at(0), privateKeys.at(1) }));
  keys->setThreshold(2);

  std::cout << "Created a Key List: " << keys->toString() << std::endl;

  // Step 2: Create the account
  std::cout << "Creating new account with the above Key List as an account key." << std::endl;

  AccountId alice = AccountCreateTransaction()
                      .setKey(keys)
                      .setInitialBalance(Hbar(2LL))
                      .execute(client)
                      .getReceipt(client)
                      .mAccountId.value();

  std::cout << "Created new account with ID: " << alice.toString() << std::endl;

  // Step 3: Schedule a transfer transaction of 1 Hbar from the created account to the operator account
  std::cout << "Creating new scheduled transaction with 1 day expiry." << std::endl;

  const WrappedTransaction scheduledTransaction(
    TransferTransaction().addHbarTransfer(alice, Hbar(-1LL)).addHbarTransfer(operatorAccountId, Hbar(1LL)));

  TransactionReceipt txReceipt = ScheduleCreateTransaction()
                                   .setScheduledTransaction(scheduledTransaction)
                                   .setExpirationTime(std::chrono::system_clock::now() + std::chrono::hours(24))
                                   .setWaitForExpiry(false)
                                   .execute(client)
                                   .getReceipt(client);

  const ScheduleId scheduleId = txReceipt.mScheduleId.value();

  // Step 4: Sign the transaction with one key and verify it is not executed
  std::cout << "Signing the new scheduled transaction with 1 key" << std::endl;

  txReceipt = ScheduleSignTransaction()
                .setScheduleId(scheduleId)
                .freezeWith(&client)
                .sign(privateKeys[0])
                .execute(client)
                .getReceipt(client);

  ScheduleInfo info = ScheduleInfoQuery().setScheduleId(scheduleId).execute(client);
  std::cout << "Scheduled transaction is not yet executed. Is executed?: " << info.mExecutionTime.has_value()
            << std::endl;

  // Step 5: Sign the transaction with the other key and verify the transaction executes
  auto accountBalance = AccountBalanceQuery().setAccountId(alice).execute(client);
  std::cout << "Alice's account balance before schedule transfer: " << accountBalance.mBalance.toString() << std::endl;

  std::cout << "Signing the new scheduled transaction with the 2nd key" << std::endl;
  txReceipt = ScheduleSignTransaction()
                .setScheduleId(scheduleId)
                .freezeWith(&client)
                .sign(privateKeys[1])
                .execute(client)
                .getReceipt(client);

  accountBalance = AccountBalanceQuery().setAccountId(alice).execute(client);
  std::cout << "Alice's account balance after schedule transfer: " << accountBalance.mBalance.toString() << std::endl;

  info = ScheduleInfoQuery().setScheduleId(scheduleId).execute(client);
  std::cout << "Scheduled transaction is executed. Is executed?: " << info.mExecutionTime.has_value() << std::endl;

  // Step 6: Schedule another transfer transaction with a 10-second expiration
  std::cout << "Creating new scheduled transaction with 10 seconds expiry." << std::endl;

  const WrappedTransaction scheduledTransactionShort(
    TransferTransaction().addHbarTransfer(alice, Hbar(-1LL)).addHbarTransfer(operatorAccountId, Hbar(1LL)));

  txReceipt = ScheduleCreateTransaction()
                .setScheduledTransaction(scheduledTransactionShort)
                .setExpirationTime(std::chrono::system_clock::now() + std::chrono::seconds(10))
                .setWaitForExpiry(true)
                .execute(client)
                .getReceipt(client);

  const ScheduleId scheduleId2 = txReceipt.mScheduleId.value();

  auto expirationTime = std::chrono::system_clock::now() + std::chrono::seconds(10);

  long startTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  long elapsedTime = 0;

  // Step 7: Sign the transaction with one key and verify it is not executed
  std::cout << "Signing the new scheduled transaction with 1 key" << std::endl;

  txReceipt = ScheduleSignTransaction()
                .setScheduleId(scheduleId2)
                .freezeWith(&client)
                .sign(privateKeys[1])
                .execute(client)
                .getReceipt(client);

  info = ScheduleInfoQuery().setScheduleId(scheduleId2).execute(client);
  std::cout << "Scheduled transaction is not yet executed. Is executed?: " << info.mExecutionTime.has_value()
            << std::endl;

  // Step 8: Update the account’s key to be only the one key that has already signed the scheduled transfer.
  std::cout << "Updating Alice's key to be the 1st key" << std::endl;

  txReceipt = AccountUpdateTransaction()
                .setAccountId(alice)
                .setKey(privateKeys[0]->getPublicKey())
                .freezeWith(&client)
                .sign(privateKeys[0])
                .sign(privateKeys[1])
                .execute(client)
                .getReceipt(client);

  // Step 9: Verify the transfer executes roughly at the time of expiration.
  accountBalance = AccountBalanceQuery().setAccountId(alice).execute(client);
  std::cout << "Alice's account balance before schedule transfer: " << accountBalance.mBalance.toString() << std::endl;

  std::cout << "Wait 10 seconds." << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(10000));

  accountBalance = AccountBalanceQuery().setAccountId(alice).execute(client);
  std::cout << "Alice's account balance after schedule transfer: " << accountBalance.mBalance.toString() << std::endl;

  std::cout << "Long Term Scheduled Transaction Example Complete!" << std::endl;
  return 0;
}
