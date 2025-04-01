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

  /*
   * Step 1: Initialize the client.
   * Note: By default, the first network address book update will be executed now
   * and subsequent updates will occur every 24 hours.
   * This is controlled by network update period, which defaults to 24 hours.
   */
  Client client = Client::forTestnet();

  const auto defaultNetworkUpdatePeriod = client.getNetworkUpdatePeriod();
  std::cout << "The network update period is " << defaultNetworkUpdatePeriod.count() << " seconds" << std::endl;

  /*
   * Step 2: Change network update period to 1 hour
   */
  const auto newNetworkUpdatePeriod = std::chrono::hours(1);
  client.setNetworkUpdatePeriod(newNetworkUpdatePeriod);
  std::cout << "Changed the network update period to " << defaultNetworkUpdatePeriod.count() << " hour" << std::endl;

  /*
   * Step 3: Wait for 1 hour
   */
  std::cout << "Waiting for 1 hour..." << std::endl;
  std::this_thread::sleep_for(std::chrono::hours(1));
  std::cout << "1 hour elapsed." << std::endl;

  /*
   * Step 4: Display client network
   */
  for (const auto node : client.getNetwork())
  {
    std::cout << "Node address: " << node.first << " Node account: " << node.second.toString() << std::endl;
  }

  return 0;
}