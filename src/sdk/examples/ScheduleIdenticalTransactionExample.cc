// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "KeyList.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "ScheduleSignTransaction.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

#include <array>
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

  // Create three clients, each having a different account and private key.
  std::array<Client, 3> clients;
  std::array<AccountId, 3> accountIds;
  std::array<std::shared_ptr<ED25519PrivateKey>, 3> privateKeys;

  std::cout << "Generate accounts.." << std::endl;

  for (int i = 0; i < 3; ++i)
  {
    privateKeys[i] = ED25519PrivateKey::generatePrivateKey();
    std::cout << "Generated private key " << i + 1 << ": " << privateKeys.at(i)->toStringRaw() << std::endl;

    accountIds[i] = AccountCreateTransaction()
                      .setKey(privateKeys.at(i))
                      .setInitialBalance(Hbar(1LL))
                      .execute(client)
                      .getReceipt(client)
                      .mAccountId.value();
    std::cout << "Generated account " << i + 1 << ": " << accountIds.at(i).toString() << std::endl;

    clients[i] = Client::forTestnet();
    clients.at(i).setOperator(accountIds.at(i), privateKeys.at(i));
    std::cout << "Generated client " << i + 1 << std::endl;
  }

  // Create another account that will contain all three keys.
  const auto keyList =
    std::make_shared<KeyList>(KeyList::of({ privateKeys.at(0), privateKeys.at(1), privateKeys.at(2) }));

  // Create another sender account.
  std::cout << "Generate a sender account" << std::endl;
  const AccountId senderAccountId = AccountCreateTransaction()
                                      .setKey(keyList)
                                      .setInitialBalance(Hbar(10LL))
                                      .execute(client)
                                      .getReceipt(client)
                                      .mAccountId.value();
  std::cout << "Generated sender account with ID " << senderAccountId.toString() << std::endl;

  // For every client, try to send 1 Hbar to each created account from the sender account.
  std::unique_ptr<ScheduleId> scheduleId = nullptr;
  ScheduleCreateTransaction scheduleCreateTransaction = TransferTransaction()
                                                          .addHbarTransfer(accountIds.at(0), Hbar(1LL))
                                                          .addHbarTransfer(accountIds.at(1), Hbar(1LL))
                                                          .addHbarTransfer(accountIds.at(2), Hbar(1LL))
                                                          .addHbarTransfer(senderAccountId, Hbar(-3LL))
                                                          .schedule();
  for (const Client& loopClient : clients)
  {
    // Schedule the transfer.
    ScheduleCreateTransaction tx = scheduleCreateTransaction;
    TransactionReceipt txReceipt =
      tx.setPayerAccountId(senderAccountId).execute(loopClient).setValidateStatus(false).getReceipt(loopClient);

    // Assign the schedule ID if this is the first schedule creation attempt.
    if (!scheduleId)
    {
      std::cout << "Created schedule with ID: " << txReceipt.mScheduleId.value().toString() << std::endl;
      scheduleId = std::make_unique<ScheduleId>(txReceipt.mScheduleId.value());
    }

    // The schedule ID should match the previously-saved schedule ID.
    if (!(*scheduleId == txReceipt.mScheduleId.value()))
    {
      std::cout << "Invalid schedule ID received, received " << txReceipt.mScheduleId->toString()
                << " when was expecting " << scheduleId->toString() << std::endl;
      return 1;
    }

    // If the receipt status indicates that the schedule was already created, sign the schedule.
    if (txReceipt.mStatus == Status::IDENTICAL_SCHEDULE_ALREADY_CREATED)
    {
      std::cout << "Schedule already created, attempting to sign.." << std::endl;
      std::cout
        << "Schedule signing: "
        << gStatusToString.at(
             ScheduleSignTransaction().setScheduleId(*scheduleId).execute(loopClient).getReceipt(loopClient).mStatus)
        << std::endl;
    }

    // Check to see if the scheduled transaction executed.
    ScheduleInfo scheduleInfo = ScheduleInfoQuery().setScheduleId(*scheduleId).execute(client);

    if (scheduleInfo.mExecutionTime.has_value())
    {
      std::cout << "Transfer transaction executed!" << std::endl;
    }
  }

  // Delete the accounts.
  std::cout << "Deleting accounts.." << std::endl;
  for (int i = 0; i < 3; ++i)
  {
    std::cout << "Deleting account: "
              << gStatusToString.at(AccountDeleteTransaction()
                                      .setDeleteAccountId(accountIds.at(i))
                                      .setTransferAccountId(operatorAccountId)
                                      .freezeWith(&client)
                                      .sign(privateKeys.at(i))
                                      .execute(client)
                                      .getReceipt(client)
                                      .mStatus)
              << std::endl;
  }

  std::cout << "Deleting sender account: "
            << gStatusToString.at(AccountDeleteTransaction()
                                    .setDeleteAccountId(senderAccountId)
                                    .setTransferAccountId(operatorAccountId)
                                    .freezeWith(&client)
                                    .sign(privateKeys.at(0))
                                    .sign(privateKeys.at(1))
                                    .sign(privateKeys.at(2))
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  return 0;
}
