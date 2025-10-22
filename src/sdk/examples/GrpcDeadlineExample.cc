// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountId.h"
#include "Client.h"
#include "Defaults.h"
#include "ED25519PrivateKey.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Create a client for Testnet
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  std::cout << "=== Global gRPC Deadline Demo ===" << std::endl;
  std::cout << std::endl;

  // Display default values
  std::cout << "Default Configuration:" << std::endl;
  std::cout << "  Request Timeout: " 
            << std::chrono::duration_cast<std::chrono::seconds>(client.getRequestTimeout()).count() 
            << " seconds" << std::endl;
  std::cout << "  gRPC Deadline: " 
            << (client.getGrpcDeadline().has_value() 
                ? std::to_string(std::chrono::duration_cast<std::chrono::seconds>(client.getGrpcDeadline().value()).count()) 
                : "not set (uses " + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(DEFAULT_GRPC_DEADLINE).count()) + " default)")
            << " seconds" << std::endl;
  std::cout << std::endl;

  // Set a custom gRPC deadline (5 seconds per node request)
  std::cout << "Setting custom gRPC deadline to 5 seconds..." << std::endl;
  client.setGrpcDeadline(std::chrono::seconds(5));
  std::cout << "  gRPC Deadline: " 
            << std::chrono::duration_cast<std::chrono::seconds>(client.getGrpcDeadline().value()).count() 
            << " seconds" << std::endl;
  std::cout << std::endl;

  // Demonstrate validation: requestTimeout must be >= grpcDeadline
  std::cout << "Demonstrating validation..." << std::endl;
  try
  {
    std::cout << "  Attempting to set request timeout to 3 seconds (less than gRPC deadline of 5 seconds)..." << std::endl;
    client.setRequestTimeout(std::chrono::seconds(3));
    std::cout << "  ERROR: Should have thrown an exception!" << std::endl;
  }
  catch (const std::invalid_argument& e)
  {
    std::cout << "  ✓ Validation caught the error: " << e.what() << std::endl;
  }
  std::cout << std::endl;

  // Set a valid request timeout
  std::cout << "Setting valid request timeout to 30 seconds..." << std::endl;
  client.setRequestTimeout(std::chrono::seconds(30));
  std::cout << "  Request Timeout: " 
            << std::chrono::duration_cast<std::chrono::seconds>(client.getRequestTimeout()).count() 
            << " seconds" << std::endl;
  std::cout << std::endl;

  // Execute a simple query to demonstrate the timeout in action
  std::cout << "Executing account balance query with custom gRPC deadline..." << std::endl;
  try
  {
    const AccountBalance balance = AccountBalanceQuery()
                                     .setAccountId(operatorAccountId)
                                     .execute(client);
    std::cout << "  Account balance: " << balance.mBalance.toString() << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "  Error executing query: " << e.what() << std::endl;
  }
  std::cout << std::endl;

  // Demonstrate per-request override
  std::cout << "Per-request override example:" << std::endl;
  std::cout << "  Client gRPC deadline: " 
            << std::chrono::duration_cast<std::chrono::seconds>(client.getGrpcDeadline().value()).count() 
            << " seconds" << std::endl;
  try
  {
    const AccountBalance balance = AccountBalanceQuery()
                                     .setAccountId(operatorAccountId)
                                     .setGrpcDeadline(std::chrono::seconds(15)) // Override for this query only
                                     .execute(client);
    std::cout << "  Query executed with overridden 15-second gRPC deadline" << std::endl;
    std::cout << "  Account balance: " << balance.mBalance.toString() << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << "  Error executing query: " << e.what() << std::endl;
  }
  std::cout << std::endl;

  return 0;
}

