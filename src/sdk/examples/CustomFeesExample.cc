// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "CustomFee.h"
#include "CustomFixedFee.h"
#include "CustomFractionalFee.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "Status.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenFeeScheduleUpdateTransaction.h"
#include "TokenId.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

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

  // Create three accounts, Alice, Bob, and Charlie. Alice will be the treasury for our example token. Fees only apply
  // in transactions not involving the treasury, so we need two other accounts.
  const std::shared_ptr<ED25519PrivateKey> aliceKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId aliceAccountId = AccountCreateTransaction()
                                     .setKey(aliceKey)
                                     .setInitialBalance(Hbar(10LL))
                                     .execute(client)
                                     .getReceipt(client)
                                     .mAccountId.value();
  std::cout << "Created account for Alice: " << aliceAccountId.toString() << std::endl;

  const std::shared_ptr<ED25519PrivateKey> bobKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId bobAccountId = AccountCreateTransaction()
                                   .setKey(bobKey)
                                   .setInitialBalance(Hbar(10LL))
                                   .execute(client)
                                   .getReceipt(client)
                                   .mAccountId.value();
  std::cout << "Created account for Bob: " << bobAccountId.toString() << std::endl;

  const std::shared_ptr<ED25519PrivateKey> charlieKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId charlieAccountId = AccountCreateTransaction()
                                       .setKey(charlieKey)
                                       .setInitialBalance(Hbar(10LL))
                                       .execute(client)
                                       .getReceipt(client)
                                       .mAccountId.value();
  std::cout << "Created account for Charlie: " << charlieAccountId.toString() << std::endl;

  // Let's start with a custom fee list of 1 fixed fee.  A custom fee list can be a list of up to 10 custom fees, where
  // each fee is a fixed fee or a fractional fee. This fixed fee will mean that every time Bob transfers any number of
  // tokens to Charlie, Alice will collect 1 Hbar from each account involved in the transaction who is SENDING the Token
  // (in this case, Bob).
  const std::vector<std::shared_ptr<CustomFee>> customHbarFeeList = { std::make_shared<CustomFixedFee>(
    CustomFixedFee().setHbarAmount(Hbar(1LL)).setFeeCollectorAccountId(aliceAccountId)) };

  // In this example, the fee is in Hbar, but you can charge a fixed fee in a token if you'd like (e.g. you can make it
  // so that each time an account transfers Foo tokens, they must pay a fee in Bar tokens to the fee collecting
  // account). To charge a fixed fee in tokens, instead of calling setHbarAmount(), call
  // setDenominatingTokenId(tokenForFee) and setHbarAmount(tokenFeeAmount).

  // Setting mFeeScheduleKey to Alice's key will enable Alice to change the custom fees list on this token later using
  // the TokenFeeScheduleUpdateTransaction. We will create an initial supply of 100 of these tokens.
  const TokenId tokenId = TokenCreateTransaction()
                            .setTokenName("Example Token")
                            .setTokenSymbol("EX")
                            .setAdminKey(aliceKey)
                            .setSupplyKey(aliceKey)
                            .setFeeScheduleKey(aliceKey)
                            .setWipeKey(aliceKey)
                            .setTreasuryAccountId(aliceAccountId)
                            .setCustomFees(customHbarFeeList)
                            .setInitialSupply(100ULL)
                            .freezeWith(&client)
                            .sign(aliceKey)
                            .execute(client)
                            .getReceipt(client)
                            .mTokenId.value();
  std::cout << "Created token: " << tokenId.toString() << std::endl;

  // We must associate the token with Bob and Charlie so that they can trade it.
  std::cout << "Associate Bob with the created token: "
            << gStatusToString.at(TokenAssociateTransaction()
                                    .setAccountId(bobAccountId)
                                    .setTokenIds({ tokenId })
                                    .freezeWith(&client)
                                    .sign(bobKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  std::cout << "Associate Charlie with the created token: "
            << gStatusToString.at(TokenAssociateTransaction()
                                    .setAccountId(charlieAccountId)
                                    .setTokenIds({ tokenId })
                                    .freezeWith(&client)
                                    .sign(charlieKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Give all 100 tokens to Bob.
  std::cout << "Transfer all 100 tokens to Bob: "
            << gStatusToString.at(TransferTransaction()
                                    .addTokenTransfer(tokenId, bobAccountId, 100LL)
                                    .addTokenTransfer(tokenId, aliceAccountId, -100LL)
                                    .freezeWith(&client)
                                    .sign(aliceKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Get Alice's Hbar balance before Bob transfers 20 tokens to Charlie, so that we can see how much Hbar she made.
  std::cout << "Alice's Hbar balance before Bob transfers 20 tokens to Charlie: "
            << AccountBalanceQuery().setAccountId(aliceAccountId).execute(client).mBalance.toString() << std::endl;

  // Transfer 20 tokens to Charlie.
  std::cout << "Bob transfer 20 tokens to Charlie: ";
  TransactionRecord txRecord = TransferTransaction()
                                 .addTokenTransfer(tokenId, bobAccountId, -20LL)
                                 .addTokenTransfer(tokenId, charlieAccountId, 20LL)
                                 .freezeWith(&client)
                                 .sign(bobKey)
                                 .execute(client)
                                 .getRecord(client);
  std::cout << gStatusToString.at(txRecord.mReceipt->mStatus) << std::endl;

  // Get Alice's Hbar balance after the transfer.
  std::cout << "Alice's Hbar balance after Bob transfers 20 tokens to Charlie: "
            << AccountBalanceQuery().setAccountId(aliceAccountId).execute(client).mBalance.toString() << std::endl;

  // Look at the assessed custom fees in the TransactionRecord.
  std::cout << "Assessed fees: " << std::endl;
  for (const auto& assessedFee : txRecord.mAssessedCustomFees)
  {
    std::cout << " - Amount: " << assessedFee.mAmount << std::endl;
    std::cout << " - Fee Collector: " << assessedFee.mFeeCollectorAccountId.toString() << std::endl;
    std::cout << " - Fee Payers: ";

    for (const auto& payer : assessedFee.mPayerAccountIdList)
    {
      std::cout << payer.toString() << ' ';
    }

    std::cout << std::endl;
  }

  // Let's use the TokenUpdateFeeScheduleTransaction with Alice's key to change the custom fees on our token.
  // TokenUpdateFeeScheduleTransaction will replace the list of fees that apply to the token with an entirely new list.
  // Let's charge a 10% fractional fee.  This means that when Bob attempts to transfer 20 tokens to Charlie, 10% of the
  // tokens he attempts to transfer (2 in this case) will be transferred to Alice instead.

  // Fractional fees default to FeeAssessmentMethod::INCLUSIVE, which is the behavior described above. If you set the
  // assessment method to EXCLUSIVE, then when Bob attempts to transfer 20 tokens to Charlie, Charlie will receive all
  // 20 tokens, and Bob will be charged an _additional_ 10% fee which will be transferred to Alice.
  const std::vector<std::shared_ptr<CustomFee>> fractionalFeeList = { std::make_shared<CustomFractionalFee>(
    CustomFractionalFee()
      .setNumerator(1LL)
      .setDenominator(10LL)
      .setMinimumAmount(1ULL)
      .setMaximumAmount(10ULL)
      .setFeeCollectorAccountId(aliceAccountId)) };

  std::cout << "Updating token fee to be fractional: "
            << gStatusToString.at(TokenFeeScheduleUpdateTransaction()
                                    .setTokenId(tokenId)
                                    .setCustomFees(fractionalFeeList)
                                    .freezeWith(&client)
                                    .sign(aliceKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Transfer another 20 tokens to Charlie.
  std::cout << "Bob transfer 20 tokens to Charlie: ";
  txRecord = TransferTransaction()
               .addTokenTransfer(tokenId, bobAccountId, -20LL)
               .addTokenTransfer(tokenId, charlieAccountId, 20LL)
               .freezeWith(&client)
               .sign(bobKey)
               .execute(client)
               .getRecord(client);
  std::cout << gStatusToString.at(txRecord.mReceipt->mStatus) << std::endl;

  // Look at the assessed custom fees in the TransactionRecord.
  std::cout << "Assessed fees (should be 2 tokens to Alice): " << std::endl;
  for (const auto& assessedFee : txRecord.mAssessedCustomFees)
  {
    std::cout << " - Amount: " << assessedFee.mAmount << std::endl;
    std::cout << " - Fee Collector: " << assessedFee.mFeeCollectorAccountId.toString() << std::endl;
    std::cout << " - Fee Payers: ";

    for (const auto& payer : assessedFee.mPayerAccountIdList)
    {
      std::cout << payer.toString() << ' ';
    }

    std::cout << std::endl;
  }

  return 0;
}
