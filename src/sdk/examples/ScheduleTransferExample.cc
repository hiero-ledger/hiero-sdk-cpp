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

  // A scheduled transaction is a transaction that has been proposed by an account, but which requires more signatures
  // before it will actually execute on the Hiero network.
  //
  // For example, if Alice wants to transfer an amount of Hbar to Bob, and Bob has receiverSignatureRequired set to
  // true, then that transaction must be signed by both Alice and Bob before the transaction will be executed.
  //
  // To solve this problem, Alice can propose the transaction by creating a scheduled transaction on the Hiero network
  // which, if executed, would transfer Hbar from Alice to Bob.  That scheduled transaction will have a ScheduleId by
  // which we can refer to that scheduled transaction.  Alice can communicate the ScheduleId to Bob, and then Bob can
  // use a ScheduleSignTransaction to sign that scheduled transaction.
  //
  // Bob has a 30 minute window in which to sign the scheduled transaction, starting at the moment that Alice creates
  // the scheduled transaction.  If a scheduled transaction is not signed by all of the necessary signatories within the
  // 30 minute window, that scheduled transaction will expire, and will not be executed.
  //
  // Once a scheduled transaction has all of the signatures necessary to execute, it will be executed on the Hiero
  // network automatically.  If you create a scheduled transaction on the Hiero network, but that transaction only
  // requires your signature in order to execute and no one else's, that scheduled transaction will be automatically
  // executed immediately.
  std::cout << "Generate account.. " << std::endl;
  const std::shared_ptr<PrivateKey> accountPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId accountId = AccountCreateTransaction()
                                .setKey(accountPrivateKey)
                                .setInitialBalance(Hbar(10LL))
                                .setReceiverSignatureRequired(true)
                                .freezeWith(&client)
                                .sign(accountPrivateKey)
                                .execute(client)
                                .getReceipt(client)
                                .mAccountId.value();
  std::cout << "Account generated with ID: " << accountId.toString() << std::endl;

  // Verify the balance of the created account.
  Hbar balance = AccountBalanceQuery().setAccountId(accountId).execute(client).mBalance;
  std::cout << "Balance of created account: " << balance.toString() << std::endl;

  // The payerAccountId is the account that will be charged the fee for executing the scheduled transaction if/when it
  // is executed. That fee is separate from the fee that will pay to execute the ScheduleCreateTransaction itself.
  //
  // To clarify: Alice pays a fee to execute the ScheduleCreateTransaction, which creates the scheduled transaction on
  // the Hiero network. She specifies when creating the scheduled transaction that Bob will pay the fee for the
  // scheduled transaction when it is executed.
  //
  // If payerAccountId is not specified, the account who creates the scheduled transaction will be charged for
  // executing the scheduled transaction.
  const TransferTransaction transferTransaction =
    TransferTransaction().addHbarTransfer(operatorAccountId, Hbar(-10LL)).addHbarTransfer(accountId, Hbar(10LL));

  std::cout << "Scheduling transfer from operator to created account.." << std::endl;
  const ScheduleId scheduleId = ScheduleCreateTransaction()
                                  .setScheduledTransaction(WrappedTransaction(transferTransaction))
                                  .setPayerAccountId(accountId)
                                  .execute(client)
                                  .getReceipt(client)
                                  .mScheduleId.value();
  std::cout << "Scheduled transfer with ID: " << scheduleId.toString() << std::endl;

  // Verify the transaction has not executed and that the created account's balance hasn't changed.
  balance = AccountBalanceQuery().setAccountId(accountId).execute(client).mBalance;
  std::cout << "Balance of account (should be same as before): " << balance.toString() << std::endl;

  // Once the operator has communicated the schedule ID to the created account, it can query for information about the
  // scheduled transaction.
  ScheduleInfo scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(client);

  // Verify the transaction.
  if (!scheduleInfo.mScheduledTransaction.getTransaction<TransferTransaction>())
  {
    std::cout << "Scheduled transaction is not a transfer!" << std::endl;
    return 1;
  }

  // Have the created account sign the scheduled transaction.
  std::cout << "Created account signing transaction: "
            << gStatusToString.at(ScheduleSignTransaction()
                                    .setScheduleId(scheduleId)
                                    .freezeWith(&client)
                                    .sign(accountPrivateKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Verify the transaction executed.
  scheduleInfo = ScheduleInfoQuery().setScheduleId(scheduleId).execute(client);

  if (scheduleInfo.mExecutionTime.has_value())
  {
    std::cout << "Transfer executed!" << std::endl;
  }
  else
  {
    std::cout << "Transfer did not execute!" << std::endl;
    return 1;
  }

  // Delete the created account.
  std::cout << "Deleting created account: "
            << gStatusToString.at(AccountDeleteTransaction()
                                    .setDeleteAccountId(accountId)
                                    .setTransferAccountId(operatorAccountId)
                                    .freezeWith(&client)
                                    .sign(accountPrivateKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;
}
