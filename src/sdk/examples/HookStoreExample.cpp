// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ContractId.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "HookStoreTransaction.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "hooks/EvmHookSpec.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookEntityId.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/HookId.h"
#include "hooks/EvmHook.h"
#include "hooks/EvmHookStorageSlot.h"
#include "hooks/EvmHookStorageUpdate.h"
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
    std::cout << "HookStore Example Start!" << std::endl;

    /*
     * Step 1: Set up prerequisites - create hook contract and account with hook
     */
    std::cout << "Setting up prerequisites..." << std::endl;

    // Create the hook contract
    std::cout << "Creating hook contract..." << std::endl;

    // For this example, we'll create a simple contract bytecode
    // This is a minimal EVM contract that can serve as a hook
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

    TransactionReceipt txReceipt = ContractCreateTransaction()
                                     .setAdminKey(operatorPrivateKey->getPublicKey())
                                     .setGas(500000ULL)
                                     .setBytecode(contractBytecode)
                                     .freezeWith(&client)
                                     .sign(operatorPrivateKey)
                                     .execute(client)
                                     .getReceipt(client);

    if (!txReceipt.mContractId.has_value())
    {
      std::cerr << "Failed to create hook contract!" << std::endl;
      return 1;
    }

    const ContractId contractId = txReceipt.mContractId.value();
    std::cout << "Hook contract created with ID: " << contractId.toString() << std::endl;

    // Create account with hook
    std::cout << "Creating account with hook..." << std::endl;
    const std::shared_ptr<PrivateKey> accountKey = ED25519PrivateKey::generatePrivateKey();
    const std::shared_ptr<PublicKey> accountPublicKey = accountKey->getPublicKey();

    // Create an EVM hook
    EvmHookSpec evmHookSpec;
    evmHookSpec.setContractId(contractId);
    EvmHook evmHook;
    evmHook.setEvmHookSpec(evmHookSpec);

    // Create hook creation details
    const std::shared_ptr<PublicKey> adminKey = client.getOperatorPublicKey();
    HookCreationDetails hookDetails;
    hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookDetails.setHookId(1LL);
    hookDetails.setEvmHook(evmHook);
    hookDetails.setAdminKey(adminKey);

    txReceipt = AccountCreateTransaction()
                  .setKeyWithoutAlias(accountPublicKey)
                  .setInitialBalance(Hbar(1ULL))
                  .addHook(hookDetails)
                  .freezeWith(&client)
                  .sign(operatorPrivateKey)
                  .execute(client)
                  .getReceipt(client);

    if (!txReceipt.mAccountId.has_value())
    {
      std::cerr << "Failed to create account with hook!" << std::endl;
      return 1;
    }

    const AccountId accountId = txReceipt.mAccountId.value();
    std::cout << "account id = " << accountId.toString() << std::endl;
    std::cout << "Successfully created account with hook!" << std::endl;

    /*
     * Step 2: Demonstrate HookStoreTransaction - the core functionality.
     */
    std::cout << "\n=== HookStoreTransaction Example ===" << std::endl;

    // Create storage key (1 byte filled with value 1)
    std::vector<std::byte> storageKey(1);
    storageKey[0] = std::byte{ 1 };

    // Create storage value (32 bytes filled with value 200)
    std::vector<std::byte> storageValue(32);
    for (auto& byte : storageValue)
    {
      byte = std::byte{ 200 };
    }

    EvmHookStorageSlot storageSlot;
    storageSlot.setKey(storageKey);
    storageSlot.setValue(storageValue);

    EvmHookStorageUpdate storageUpdate;
    storageUpdate.setStorageSlot(storageSlot);

    // Create HookId for the existing hook (accountId with hook ID 1)
    HookEntityId hookEntityId;
    hookEntityId.setAccountId(accountId);

    HookId hookId;
    hookId.setEntityId(hookEntityId);
    hookId.setHookId(1LL);

    std::cout << "Storage update created:" << std::endl;
    std::cout << "  Storage Key: ";
    for (const auto& byte : storageKey)
    {
      std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    std::cout << "  Storage Value: ";
    for (const auto& byte : storageValue)
    {
      std::cout << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;

    std::cout << "  Hook ID: " << hookId.getHookId() << std::endl;
    std::cout << "  Hook Entity ID: " << hookId.getEntityId().getAccountId()->toString() << std::endl;

    // Execute HookStoreTransaction
    std::cout << "Executing HookStoreTransaction..." << std::endl;
    txReceipt = HookStoreTransaction()
                  .setHookId(hookId)
                  .addStorageUpdate(storageUpdate)
                  .freezeWith(&client)
                  .sign(accountKey)
                  .execute(client)
                  .getReceipt(client);

    std::cout << "Successfully updated hook storage!" << std::endl;
    std::cout << "Transaction completed successfully!" << std::endl;
    std::cout << "Receipt status: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

    std::cout << "\nHookStore Example Complete!" << std::endl;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Error occurred: " << error.what() << std::endl;
    return 1;
  }

  client.close();
  return 0;
}
