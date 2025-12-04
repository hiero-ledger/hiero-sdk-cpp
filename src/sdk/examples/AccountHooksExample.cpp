// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "AccountUpdateTransaction.h"
#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ContractId.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "hooks/EvmHookSpec.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/LambdaEvmHook.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <dotenv.h>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();

  if (std::getenv("OPERATOR_ID") == nullptr || std::getenv("OPERATOR_KEY") == nullptr ||
      std::getenv("NETWORK_NAME") == nullptr)
  {
    std::cerr << "Environment variables OPERATOR_ID, NETWORK_NAME, and OPERATOR_KEY are required." << std::endl;
    return 1;
  }

  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));
  const std::string network = std::getenv("NETWORK_NAME");

  // Get a client for the specified network
  Client client = Client::forName(network);
  client.setOperator(operatorAccountId, operatorPrivateKey);

  try
  {
    std::cout << "Account Hooks Example Start!" << std::endl;

    /*
     * Step 1: Create the hook contract.
     */
    std::cout << "Creating hook contract..." << std::endl;

    // Create bytecode file for the hook contract
    std::cout << "Creating bytecode for hook contract..." << std::endl;

    // For this example, we'll create a simple contract bytecode
    const std::vector<std::byte> contractBytecode = internal::HexConverter::hexToBytes(
      "608060405234801561001057600080fd5b50600436106100365760003560e01c8063c29855781461003b578063f2fde38b14610059575b60"
      "0080fd5b610043610075565b60405161005091906100a1565b60405180910390f35b610073600480360381019061006e91906100ed565b61"
      "007b565b005b60005481565b8073ffffffffffffffffffffffffffffffffffffffff1660008054906101000a900473ffffffffffffffffff"
      "ffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff167f8be0079c531659141344cd1fd0a4f28419497f9722"
      "a3daafe3b4186f6b6457e060405160405180910390a3806000806101000a81548173ffffffffffffffffffffffffffffffffffffffff0219"
      "16908373ffffffffffffffffffffffffffffffffffffffff16021790555050565b6000819050919050565b61009b81610088565b82525050"
      "565b60006020820190506100b66000830184610092565b92915050565b600080fd5b600073ffffffffffffffffffffffffffffffffffffff"
      "ff82169050919050565b60006100e7826100bc565b9050919050565b6100f7816100dc565b811461010257600080fd5b50565b6000813590"
      "50610114816100ee565b92915050565b6000602082840312156101305761012f6100bc565b5b600061013e84828501610105565b91505092"
      "91505056fea2646970667358221220");

    // Create the hook contract
    std::cout << "Creating hook contract..." << std::endl;
    TransactionReceipt txReceipt = ContractCreateTransaction()
                                     .setAdminKey(operatorPrivateKey->getPublicKey())
                                     .setGas(500000ULL)
                                     .setBytecode(contractBytecode)
                                     .execute(client)
                                     .getReceipt(client);

    if (!txReceipt.mContractId.has_value())
    {
      std::cerr << "Failed to create hook contract!" << std::endl;
      return 1;
    }

    const ContractId contractId = txReceipt.mContractId.value();
    std::cout << "Hook contract created with ID: " << contractId.toString() << std::endl;

    /*
     * Step 2: Demonstrate creating an account with hooks.
     */
    std::cout << "\n=== Creating Account with Hooks ===" << std::endl;
    std::cout << "Creating account with lambda EVM hook..." << std::endl;

    const std::shared_ptr<PrivateKey> accountKey = ED25519PrivateKey::generatePrivateKey();
    const std::shared_ptr<PublicKey> accountPublicKey = accountKey->getPublicKey();

    // Create a lambda EVM hook
    EvmHookSpec evmHookSpec;
    evmHookSpec.setContractId(contractId);
    LambdaEvmHook lambdaHook;
    lambdaHook.setEvmHookSpec(evmHookSpec);

    // Create hook creation details
    const std::shared_ptr<PublicKey> adminKey = client.getOperatorPublicKey();
    HookCreationDetails hookWithId1002;
    hookWithId1002.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookWithId1002.setHookId(1002LL);
    hookWithId1002.setLambdaEvmHook(lambdaHook);
    hookWithId1002.setAdminKey(adminKey);

    txReceipt = AccountCreateTransaction()
                  .setKeyWithoutAlias(accountPublicKey)
                  .setInitialBalance(Hbar(1ULL))
                  .addHook(hookWithId1002)
                  .execute(client)
                  .getReceipt(client);

    if (!txReceipt.mAccountId.has_value())
    {
      std::cerr << "Failed to create account with hook!" << std::endl;
      return 1;
    }

    const AccountId accountId = txReceipt.mAccountId.value();
    std::cout << "account id = " << accountId.toString() << std::endl;
    std::cout << "Successfully created account with lambda hook!" << std::endl;

    /*
     * Step 3: Demonstrate adding hooks to an existing account.
     */
    std::cout << "\n=== Adding Hooks to Existing Account ===" << std::endl;
    std::cout << "Adding hooks to existing account..." << std::endl;

    // Create basic lambda hooks with no storage updates
    LambdaEvmHook basicHook;
    basicHook.setEvmHookSpec(evmHookSpec);
    HookCreationDetails hookWithId1;
    hookWithId1.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookWithId1.setHookId(1LL);
    hookWithId1.setLambdaEvmHook(basicHook);
    hookWithId1.setAdminKey(adminKey);

    LambdaEvmHook basicHook2;
    basicHook2.setEvmHookSpec(evmHookSpec);
    HookCreationDetails hookWithId2;
    hookWithId2.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookWithId2.setHookId(2LL);
    hookWithId2.setLambdaEvmHook(basicHook2);
    hookWithId2.setAdminKey(adminKey);

    try
    {
      txReceipt = AccountUpdateTransaction()
                    .setAccountId(accountId)
                    .addHookToCreate(hookWithId1)
                    .addHookToCreate(hookWithId2)
                    .freezeWith(&client)
                    .sign(accountKey)
                    .execute(client)
                    .getReceipt(client);

      std::cout << "Successfully added hooks to account!" << std::endl;
    }
    catch (const std::exception& error)
    {
      std::cerr << "Failed to execute hook transaction: " << error.what() << std::endl;
    }

    /*
     * Step 4: Demonstrate hook deletion.
     */
    std::cout << "\n=== Deleting Hooks from Account ===" << std::endl;
    std::cout << "Deleting hooks from account..." << std::endl;

    try
    {
      txReceipt = AccountUpdateTransaction()
                    .setAccountId(accountId)
                    .addHookToDelete(1LL)
                    .addHookToDelete(2LL)
                    .freezeWith(&client)
                    .sign(accountKey)
                    .execute(client)
                    .getReceipt(client);

      std::cout << "Successfully deleted hooks (IDs: 1, 2)" << std::endl;
    }
    catch (const std::exception& error)
    {
      std::cerr << "Failed to execute hook deletion: " << error.what() << std::endl;
    }

    std::cout << "Account Hooks Example Complete!" << std::endl;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Error occurred: " << error.what() << std::endl;
    return 1;
  }

  client.close();
  return 0;
}
