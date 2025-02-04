// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileContentsQuery.h"
#include "FileId.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

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

  // Get the contents of the exchange rate file.
  FileContents fileContents = FileContentsQuery().setFileId(FileId::EXCHANGE_RATES).execute(client);

  // Convert the contents to an ExchangeRates object.
  const ExchangeRates exchangeRates = ExchangeRates::fromBytes(fileContents);

  std::cout << "Current numerator:       " << exchangeRates.mCurrentRate.mCents << std::endl;
  std::cout << "Current denominator:     " << exchangeRates.mCurrentRate.mHbars << std::endl;
  std::cout << "Current expiration time: " << exchangeRates.mCurrentRate.mExpirationTime.time_since_epoch().count()
            << std::endl;
  std::cout << "Current exchange rate:   " << exchangeRates.mCurrentRate.mExchangeRateInCents << std::endl << std::endl;

  std::cout << "Next numerator:       " << exchangeRates.mNextRate.mCents << std::endl;
  std::cout << "Next denominator:     " << exchangeRates.mNextRate.mHbars << std::endl;
  std::cout << "Next expiration time: " << exchangeRates.mNextRate.mExpirationTime.time_since_epoch().count()
            << std::endl;
  std::cout << "Next exchange rate:   " << exchangeRates.mNextRate.mExchangeRateInCents << std::endl;

  return 0;
}
