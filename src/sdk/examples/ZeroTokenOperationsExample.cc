// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ContractHelper.h"
#include "ED25519PrivateKey.h"
#include "Status.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
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
  client.setMaxTransactionFee(Hbar(10LL));

  // Generate a new account.
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();
  const AccountId accountId = AccountCreateTransaction()
                                .setKey(publicKey)
                                .setInitialBalance(Hbar(10LL))
                                .execute(client)
                                .getReceipt(client)
                                .mAccountId.value();
  std::cout << "Created new account with ID " << accountId.toString() << std::endl;

  // Instantiate the ContractHelper.
  ContractHelper contractHelper("sdk/examples/precompile-example/ZeroTokenOperations.json",
                                ContractFunctionParameters()
                                  .addAddress(operatorAccountId.toSolidityAddress())
                                  .addAddress(accountId.toSolidityAddress()),
                                client);

  /*
   * Step 0 creates a fungible token
   * Step 1 associate with account
   * Step 2 transfer the token by passing a zero value
   * Step 3 mint the token by passing a zero value
   * Step 4 burn the token by passing a zero value
   * Step 5 wip the token by passing a zero value
   */
  contractHelper.setPayableAmountForStep(0, Hbar(20LL)).addSignerForStep(1, privateKey);
  contractHelper.executeSteps(/*from*/ 0, /*to*/ 5, client);

  // Step 6: use SDK and transfer passing a zero value.
  std::cout << "Attempting to execute step 6" << std::endl;
  const TokenId tokenId = TokenCreateTransaction()
                            .setTokenName("Black Sea LimeChain Token")
                            .setTokenSymbol("BSL")
                            .setTreasuryAccountId(operatorAccountId)
                            .setInitialSupply(10000)
                            .setDecimals(2)
                            .setAutoRenewAccountId(operatorAccountId)
                            .execute(client)
                            .getReceipt(client)
                            .mTokenId.value();

  std::cout << "Associate token: "
            << gStatusToString.at(TokenAssociateTransaction()
                                    .setAccountId(accountId)
                                    .setTokenIds({ tokenId })
                                    .freezeWith(&client)
                                    .sign(privateKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  std::cout << "Transfer zero tokens: "
            << gStatusToString.at(TransferTransaction()
                                    .addTokenTransfer(tokenId, operatorAccountId, 0)
                                    .addTokenTransfer(tokenId, accountId, 0)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  return 0;
}
