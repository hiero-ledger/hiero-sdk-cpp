// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"

#include <chrono>
#include <dotenv.h>
#include <iostream>
#include <thread>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();

  const auto period = std::chrono::seconds(10);
  client.setNetworkUpdatePeriod(period);

  const auto sleepPeriod = std::chrono::seconds(1);
  while (true)
  {
    std::this_thread::sleep_for(sleepPeriod);
    std::cout << sleepPeriod.count() << std::endl;
  }

  return 0;
}