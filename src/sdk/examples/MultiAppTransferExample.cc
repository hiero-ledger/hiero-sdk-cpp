// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"

#include <cstddef>
#include <dotenv.h>
#include <iostream>
#include <vector>

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

  // Generate a key for the exchange account. The exchange should have this key, it's being generated here for
  // demonstration's sake.
  const std::shared_ptr<PrivateKey> exchangeKey = ED25519PrivateKey::generatePrivateKey();

  // Generate a key for a user.
  const std::shared_ptr<PrivateKey> userKey = ED25519PrivateKey::generatePrivateKey();

  // Generate an exchange account.
  const AccountId exchangeAccountId =
    AccountCreateTransaction()
      // The exchange only accepts transfers that it validates through a side channel (e.g. REST API).
      .setReceiverSignatureRequired(true)
      .setKey(exchangeKey)
      .freezeWith(&client)
      // The owner key has to sign the transaction if receiver signature required is true.
      .sign(exchangeKey)
      .execute(client)
      .getReceipt(client)
      .mAccountId.value();
  std::cout << "Created exchange account with ID " << exchangeAccountId.toString() << std::endl;

  // Generate a user account with 5 Hbar.
  const AccountId userAccountId = AccountCreateTransaction()
                                    .setKey(userKey)
                                    .setInitialBalance(Hbar(5LL))
                                    .execute(client)
                                    .getReceipt(client)
                                    .mAccountId.value();
  std::cout << "Created user account with ID " << userAccountId.toString() << std::endl;

  // Create a transfer from the user account to the exchange account. This must be signed by both parties.
  TransferTransaction transferTransaction = TransferTransaction()
                                              .addHbarTransfer(userAccountId, Hbar(-2LL))
                                              .addHbarTransfer(exchangeAccountId, Hbar(2LL))
                                              // The exchange-provided memo required to validate the transaction.
                                              .setTransactionMemo("https://some-exchange.com/user1/account1")
                                              // Manually sign with user key.
                                              .freezeWith(&client)
                                              .sign(userKey);

  // The exchange must sign the transaction in order for it to be accepted by the network. Serialize the transaction to
  // "send".
  std::vector<std::byte> transferTransactionBytes = transferTransaction.toBytes();

  // "Receive" and deserialize the transaction.
  WrappedTransaction transactionReceivedByExchange =
    Transaction<TransferTransaction>::fromBytes(transferTransactionBytes);

  // Sign the transaction with the user key.
  transactionReceivedByExchange.getTransaction<TransferTransaction>()->sign(exchangeKey);

  // Serialize the transaction with the exchange's signature to "send" back to the user.
  transferTransactionBytes = transactionReceivedByExchange.getTransaction<TransferTransaction>()->toBytes();

  // "Receive" and deserialize the exchange-signed transaction on the user side.
  transferTransaction =
    *Transaction<TransferTransaction>::fromBytes(transferTransactionBytes).getTransaction<TransferTransaction>();

  // Send the transfer to the Hiero network.
  std::cout << "Sending 2 Hbar to exchange from user: "
            << gStatusToString.at(transferTransaction.execute(client).getReceipt(client).mStatus) << std::endl;

  // Get the balances.
  std::cout << "Balance of user account: "
            << AccountBalanceQuery().setAccountId(userAccountId).execute(client).mBalance.toString() << std::endl;
  std::cout << "Balance of exchange account: "
            << AccountBalanceQuery().setAccountId(exchangeAccountId).execute(client).mBalance.toString() << std::endl;

  return 0;
}
