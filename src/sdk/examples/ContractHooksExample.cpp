// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ContractId.h"
#include "ContractUpdateTransaction.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "hooks/EvmHookSpec.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/EvmHook.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <dotenv.h>
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
    std::cout << "Contract Hooks Example Start!" << std::endl;

    /*
     * Step 1: Create the hook contract.
     */
    std::cout << "Creating hook contract..." << std::endl;

    // Create bytecode file for the hook contract
    std::cout << "Creating bytecode for hook contract..." << std::endl;

    // For this example, we'll create a simple contract bytecode
    // This is a minimal EVM contract that can serve as a hook
    const std::vector<std::byte> hookContractBytecode = internal::HexConverter::hexToBytes(
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
                                     .setBytecode(hookContractBytecode)
                                     .freezeWith(&client)
                                     .sign(operatorPrivateKey)
                                     .execute(client)
                                     .getReceipt(client);

    if (!txReceipt.mContractId.has_value())
    {
      std::cerr << "Failed to create hook contract!" << std::endl;
      return 1;
    }

    const ContractId hookContractId = txReceipt.mContractId.value();
    std::cout << "Hook contract created with ID: " << hookContractId.toString() << std::endl;

    /*
     * Step 2: Demonstrate creating a contract with hooks.
     */
    std::cout << "\n=== Creating Contract with Hooks ===" << std::endl;
    std::cout << "Creating contract with EVM hook..." << std::endl;

    // Create a simple contract bytecode (HelloWorld-like contract)
    const std::vector<std::byte> simpleContractBytecode = internal::HexConverter::hexToBytes(
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

    // Build a basic EVM hook (no admin key, no storage updates)
    EvmHookSpec evmHookSpec;
    evmHookSpec.setContractId(hookContractId);
    EvmHook evmHook;
    evmHook.setEvmHookSpec(evmHookSpec);

    HookCreationDetails hookWithId1;
    hookWithId1.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookWithId1.setHookId(1LL);
    hookWithId1.setEvmHook(evmHook);

    txReceipt = ContractCreateTransaction()
                  .setAdminKey(operatorPrivateKey->getPublicKey())
                  .setGas(400000ULL)
                  .setBytecode(simpleContractBytecode)
                  .addHook(hookWithId1)
                  .freezeWith(&client)
                  .sign(operatorPrivateKey)
                  .execute(client)
                  .getReceipt(client);

    if (!txReceipt.mContractId.has_value())
    {
      std::cerr << "Failed to create contract with hooks!" << std::endl;
      return 1;
    }

    const ContractId contractWithHooksId = txReceipt.mContractId.value();
    std::cout << "Created contract with ID: " << contractWithHooksId.toString() << std::endl;
    std::cout << "Successfully created contract with basic hook!" << std::endl;

    /*
     * Step 3: Demonstrate adding hooks to an existing contract.
     */
    std::cout << "\n=== Adding Hooks to Existing Contract ===" << std::endl;
    std::cout << "Adding hooks to existing contract..." << std::endl;

    const std::shared_ptr<PublicKey> adminKey = client.getOperatorPublicKey();

    // Hook 3: Basic hook with no storage updates (using ID 3 to avoid conflict with existing hook 1)
    EvmHook basicHook;
    basicHook.setEvmHookSpec(evmHookSpec);
    HookCreationDetails hookWithId3;
    hookWithId3.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookWithId3.setHookId(3LL);
    hookWithId3.setEvmHook(basicHook);
    hookWithId3.setAdminKey(adminKey);

    try
    {
      txReceipt = ContractUpdateTransaction()
                    .setContractId(contractWithHooksId)
                    .addHookToCreate(hookWithId3)
                    .freezeWith(&client)
                    .sign(operatorPrivateKey)
                    .execute(client)
                    .getReceipt(client);

      std::cout << "Successfully added hooks to contract!" << std::endl;
    }
    catch (const std::exception& error)
    {
      std::cerr << "Failed to execute hook transaction: " << error.what() << std::endl;
    }

    /*
     * Step 4: Demonstrate hook deletion.
     */
    std::cout << "\n=== Deleting Hooks from Contract ===" << std::endl;
    std::cout << "Deleting hooks from contract..." << std::endl;

    try
    {
      txReceipt = ContractUpdateTransaction()
                    .setContractId(contractWithHooksId)
                    .addHookToDelete(1LL) // Delete hook created during contract creation
                    .addHookToDelete(3LL) // Delete hook added via contract update
                    .freezeWith(&client)
                    .sign(operatorPrivateKey)
                    .execute(client)
                    .getReceipt(client);

      std::cout << "Successfully deleted hooks with IDs: 1 and 3" << std::endl;
    }
    catch (const std::exception& error)
    {
      std::cerr << "Failed to execute hook deletion: " << error.what() << std::endl;
    }

    std::cout << "Contract Hooks Example Complete!" << std::endl;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Error occurred: " << error.what() << std::endl;
    return 1;
  }

  client.close();
  return 0;
}
