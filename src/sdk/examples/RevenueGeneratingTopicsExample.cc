// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "CustomFixedFee.h"
#include "ED25519PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicId.h"
#include "TopicMessageSubmitTransaction.h"
#include "TopicUpdateTransaction.h"
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

  /*
   * Step 1:
   * Create account - alice
   */
  std::cout << "Creating account - alice" << std::endl;

  std::shared_ptr<PrivateKey> aliceKey = ED25519PrivateKey::generatePrivateKey();

  AccountId alice = AccountCreateTransaction()
                      .setKeyWithoutAlias(aliceKey->getPublicKey())
                      .setInitialBalance(Hbar(10LL))
                      .execute(client)
                      .getReceipt(client)
                      .mAccountId.value();

  std::cout << "Alice account id:" << alice.toString() << std::endl;

  /*
   * Step 2:
   * Create a topic with hbar custom fee
   */
  int64_t feeAmount = 100000000; // 1 HBAR equivalent

  CustomFixedFee customFixedFee = CustomFixedFee().setAmount(feeAmount).setFeeCollectorAccountId(operatorAccountId);

  TopicId topicId = TopicCreateTransaction()
                      .setAdminKey(operatorPrivateKey->getPublicKey())
                      .setFeeScheduleKey(operatorPrivateKey->getPublicKey())
                      .addCustomFixedFee({ customFixedFee })
                      .execute(client)
                      .getReceipt(client)
                      .mTopicId.value();

  /*
   * Step 3:
   * Submit a message to that topic, paid for by alice, specifying max custom fee amount bigger than the topic’s amount.
   */
  AccountBalance accountBalanceBefore = AccountBalanceQuery().setAccountId(alice).execute(client);
  AccountBalance feeCollectorBalanceBefore = AccountBalanceQuery().setAccountId(operatorAccountId).execute(client);

  CustomFeeLimit limit = CustomFeeLimit().setPayerId(alice).addCustomFee(CustomFixedFee().setAmount(feeAmount * 2));

  client.setOperator(alice, aliceKey);
  TopicMessageSubmitTransaction()
    .setTopicId(topicId)
    .setMessage("message")
    .addCustomFeeLimit(limit)
    .execute(client)
    .getReceipt(client);

  std::cout << "Message submitted successfully" << std::endl;

  /*
   * Step 4:
   * Verify alice was debited the fee amount and the fee collector account was credited the amount.
   */
  client.setOperator(operatorAccountId, operatorPrivateKey);

  AccountBalance accountBalanceAfter = AccountBalanceQuery().setAccountId(alice).execute(client);
  AccountBalance feeCollectorBalanceAfter = AccountBalanceQuery().setAccountId(operatorAccountId).execute(client);

  std::cout << "Alice account balance before: " << accountBalanceBefore.toString() << std::endl;
  std::cout << "Alice account balance after: " << accountBalanceAfter.toString() << std::endl;
  std::cout << "Fee collector account balance before: " << feeCollectorBalanceBefore.toString() << std::endl;
  std::cout << "Fee collector account balance after: " << feeCollectorBalanceAfter.toString() << std::endl;

  /*
   * Step 5:
   * Create a fungible token and transfer some tokens to alice
   */
  TokenId tokenId = TokenCreateTransaction()
                      .setTokenName("revenueGeneratingToken")
                      .setTokenSymbol("RGT")
                      .setInitialSupply(10)
                      .setTreasuryAccountId(operatorAccountId)
                      .execute(client)
                      .getReceipt(client)
                      .mTokenId.value();

  std::cout << "Created token with Id: " << tokenId.toString() << std::endl;

  TransferTransaction()
    .addTokenTransfer(tokenId, operatorAccountId, -1ULL)
    .addTokenTransfer(tokenId, alice, 1ULL)
    .execute(client)
    .getReceipt(client);

  /*
   * Step 6:
   * Update the topic to have a fee of the token.
   */
  std::cout << "Updating the topic to have a custom fee of the token" << std::endl;

  CustomFixedFee customTokenFixedFee =
    CustomFixedFee().setAmount(1).setDenominatingTokenId(tokenId).setFeeCollectorAccountId(operatorAccountId);

  TopicUpdateTransaction()
    .setTopicId(topicId)
    .setCustomFixedFees({ customTokenFixedFee })
    .execute(client)
    .getReceipt(client);

  /*
   * Step 7:
   * Submit another message to that topic, paid by alice, without specifying max custom fee amount.
   */
  accountBalanceBefore = AccountBalanceQuery().setAccountId(alice).execute(client);
  feeCollectorBalanceBefore = AccountBalanceQuery().setAccountId(operatorAccountId).execute(client);

  std::cout << "Submitting a message as alice to the topic" << std::endl;

  client.setOperator(alice, aliceKey);
  TopicMessageSubmitTransaction().setTopicId(topicId).setMessage("message").execute(client).getReceipt(client);

  std::cout << "Message submitted successfully" << std::endl;

  /*
   * Step 8:
   * Verify alice was debited the new fee amount and the fee collector account was credited the amount.
   */
  client.setOperator(operatorAccountId, operatorPrivateKey);

  accountBalanceAfter = AccountBalanceQuery().setAccountId(alice).execute(client);
  feeCollectorBalanceAfter = AccountBalanceQuery().setAccountId(operatorAccountId).execute(client);

  std::cout << "Alice account balance before: " << accountBalanceBefore.toString() << std::endl;
  std::cout << "Alice account balance after: " << accountBalanceAfter.toString() << std::endl;
  std::cout << "Fee collector account balance before: " << feeCollectorBalanceBefore.toString() << std::endl;
  std::cout << "Fee collector account balance after: " << feeCollectorBalanceAfter.toString() << std::endl;

  /*
   * Step 9:
   * Create account - bob
   */
  std::cout << "Creating account - bob" << std::endl;

  std::shared_ptr<PrivateKey> bobKey = ED25519PrivateKey::generatePrivateKey();

  AccountId bob = AccountCreateTransaction()
                    .setKeyWithoutAlias(bobKey->getPublicKey())
                    .setInitialBalance(Hbar(10LL))
                    .setMaxAutomaticTokenAssociations(-1)
                    .execute(client)
                    .getReceipt(client)
                    .mAccountId.value();

  std::cout << "Bob account id:" << bob.toString() << std::endl;

  /*
   * Step 10:
   * Update the topic’s fee exempt keys and add bob’s public key.
   */
  std::cout << "Updating the topic fee exempt keys with bob's public key" << std::endl;

  TopicUpdateTransaction()
    .setTopicId(topicId)
    .addFeeExemptKey(bobKey->getPublicKey())
    .execute(client)
    .getReceipt(client);

  /*
   * Step 11:
   * Submit another message to that topic, paid with bob, without specifying max custom fee amount.
   */
  accountBalanceBefore = AccountBalanceQuery().setAccountId(bob).execute(client);
  feeCollectorBalanceBefore = AccountBalanceQuery().setAccountId(operatorAccountId).execute(client);

  std::cout << "Submitting a message as bob to the topic" << std::endl;

  client.setOperator(bob, bobKey);
  TopicMessageSubmitTransaction().setTopicId(topicId).setMessage("message").execute(client).getReceipt(client);

  std::cout << "Message submitted successfully" << std::endl;

  /*
   * Step 12:
   * Verify bob was not debited the fee amount.
   */
  client.setOperator(operatorAccountId, operatorPrivateKey);

  accountBalanceAfter = AccountBalanceQuery().setAccountId(bob).execute(client);
  feeCollectorBalanceAfter = AccountBalanceQuery().setAccountId(operatorAccountId).execute(client);

  std::cout << "Bob account balance before: " << accountBalanceBefore.toString() << std::endl;
  std::cout << "Bob account balance after: " << accountBalanceAfter.toString() << std::endl;

  // Clean up:
  client.close();

  return 0;
}