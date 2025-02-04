// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "KeyList.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "ScheduleSignTransaction.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

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

  std::cout << "Generate two keys for an account.." << std::endl;
  const std::shared_ptr<PrivateKey> key1 = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> key2 = ED25519PrivateKey::generatePrivateKey();
  std::cout << "Private keys generated: \n -" << key1->toStringRaw() << "\n -" << key2->toStringRaw() << std::endl;

  const auto keyList = std::make_shared<KeyList>(KeyList::of({ key1, key2 }));

  std::cout << "Generate account.. " << std::endl;
  const AccountId accountId = AccountCreateTransaction()
                                .setKey(keyList)
                                .setInitialBalance(Hbar(10LL))
                                .execute(client)
                                .getReceipt(client)
                                .mAccountId.value();
  std::cout << "Account generated with ID: " << accountId.toString() << std::endl;

  std::cout << "Schedule a transfer from the created account to the operator for 24 hours from now.." << std::endl;
  TransactionResponse txResponse = TransferTransaction()
                                     .addHbarTransfer(accountId, Hbar(1LL).negated())
                                     .addHbarTransfer(operatorAccountId, Hbar(1LL))
                                     .schedule()
                                     .setExpirationTime(std::chrono::system_clock::now() + std::chrono::hours(24))
                                     .setWaitForExpiry(true)
                                     .execute(client);
  const ScheduleId scheduleId = txResponse.getReceipt(client).mScheduleId.value();
  std::cout << "Scheduled transfer with ID: " << scheduleId.toString() << std::endl;

  // Have the created account's keys sign the scheduled transaction.
  std::cout << "Created account signing transaction: "
            << gStatusToString.at(ScheduleSignTransaction()
                                    .setScheduleId(scheduleId)
                                    .freezeWith(&client)
                                    .sign(key1)
                                    .sign(key2)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  std::cout << "The following link should query the mirror node for the scheduled transaction:" << std::endl;
  std::cout << txResponse.mTransactionId.mAccountId.toString() << '-'
            << txResponse.mTransactionId.mValidTransactionTime.time_since_epoch().count() << std::endl;

  // Delete the created account.
  std::cout << "Deleting created account: "
            << gStatusToString.at(AccountDeleteTransaction()
                                    .setDeleteAccountId(accountId)
                                    .setTransferAccountId(operatorAccountId)
                                    .freezeWith(&client)
                                    .sign(key1)
                                    .sign(key2)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;
}
