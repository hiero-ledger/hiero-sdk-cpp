// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "KeyList.h"
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

  // Create an account with a Keylist of four keys.
  std::cout << "Generating key list of four keys with a threshold of three.." << std::endl;
  const std::array<std::shared_ptr<PrivateKey>, 4> keys = { ED25519PrivateKey::generatePrivateKey(),
                                                            ED25519PrivateKey::generatePrivateKey(),
                                                            ED25519PrivateKey::generatePrivateKey(),
                                                            ED25519PrivateKey::generatePrivateKey() };

  // Put the four generated keys in a keylist and set the threshold so that only 3 out of 4 keys need to sign.
  auto keyList = std::make_shared<KeyList>(KeyList::of({ keys.at(0), keys.at(1), keys.at(2), keys.at(3) }));
  keyList->setThreshold(3);

  // Create an account with the four keys.
  std::cout << "Creating account with generated keys: ";
  TransactionReceipt txReceipt =
    AccountCreateTransaction().setKey(keyList).setInitialBalance(Hbar(10LL)).execute(client).getReceipt(client);
  std::cout << gStatusToString.at(txReceipt.mStatus) << std::endl;

  // Schedule a transfer out of the created account with only one signature.
  std::cout << "Scheduling transaction with one signature.." << std::endl;
  const AccountId accountId = txReceipt.mAccountId.value();
  const ScheduleId scheduleId = TransferTransaction()
                                  .addHbarTransfer(accountId, Hbar(-1LL))
                                  .addHbarTransfer(operatorAccountId, Hbar(1LL))
                                  .schedule()
                                  .setPayerAccountId(operatorAccountId)
                                  .setAdminKey(operatorPrivateKey)
                                  .freezeWith(&client)
                                  .sign(keys.at(0))
                                  .execute(client)
                                  .getReceipt(client)
                                  .mScheduleId.value();
  std::cout << "Scheduled transaction with schedule ID: " << scheduleId.toString() << std::endl;

  // Keep adding signatures until the transaction executes.
  for (int i = 1; i < keys.size(); ++i)
  {
    // Determine if the transaction has executed.
    if (const ScheduleInfo scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(client);
        scheduleInfo.mExecutionTime.has_value())
    {
      std::cout << "Transaction has executed with (should be 3) " << i << " signatures!" << std::endl;
      break;
    }

    // Add a signature if the transfer hasn't yet executed.
    std::cout << "Adding signature.. "
              << gStatusToString.at(ScheduleSignTransaction()
                                      .setScheduleId(scheduleId)
                                      .freezeWith(&client)
                                      .sign(keys.at(i))
                                      .execute(client)
                                      .getReceipt(client)
                                      .mStatus)
              << std::endl;
  }

  // Delete the created account.
  std::cout << "Delete created account: "
            << gStatusToString.at(AccountDeleteTransaction()
                                    .setDeleteAccountId(accountId)
                                    .setTransferAccountId(operatorAccountId)
                                    .freezeWith(&client)
                                    .sign(keys.at(0))
                                    .sign(keys.at(1))
                                    .sign(keys.at(2))
                                    .sign(keys.at(3))
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;
}
