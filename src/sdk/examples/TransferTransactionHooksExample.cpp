// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ContractId.h"
#include "ED25519PrivateKey.h"
#include "EvmAddress.h"
#include "Hbar.h"
#include "NftId.h"
#include "PrivateKey.h"
#include "PublicKey.h"
#include "TokenCreateTransaction.h"
#include "TokenId.h"
#include "TokenMintTransaction.h"
#include "TokenType.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "hooks/EvmHookCall.h"
#include "hooks/FungibleHookCall.h"
#include "hooks/FungibleHookType.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/EvmHook.h"
#include "hooks/NftHookCall.h"
#include "hooks/NftHookType.h"
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
    std::cout << "Transfer Transaction Hooks Example Start!" << std::endl;

    /*
     * Step 1: Set up prerequisites - create tokens and NFTs
     */
    std::cout << "Setting up prerequisites..." << std::endl;

    // Create hook contract bytecode
    const std::vector<std::byte> hookBytecode = internal::HexConverter::hexToBytes(
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
                                     .setGas(1000000ULL)
                                     .setBytecode(hookBytecode)
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

    // Create hook details
    EvmHookSpec evmHookSpec;
    evmHookSpec.setContractId(hookContractId);
    EvmHook evmHook;
    evmHook.setEvmHookSpec(evmHookSpec);

    HookCreationDetails hookDetails;
    hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookDetails.setHookId(1LL);
    hookDetails.setEvmHook(evmHook);

    // Create sender account
    const std::shared_ptr<PrivateKey> senderPrivateKey = ED25519PrivateKey::generatePrivateKey();
    txReceipt = AccountCreateTransaction()
                  .setKeyWithoutAlias(senderPrivateKey->getPublicKey())
                  .setInitialBalance(Hbar(10ULL))
                  .addHook(hookDetails)
                  .freezeWith(&client)
                  .sign(senderPrivateKey)
                  .execute(client)
                  .getReceipt(client);

    if (!txReceipt.mAccountId.has_value())
    {
      std::cerr << "Failed to create sender account!" << std::endl;
      return 1;
    }

    const AccountId senderAccountId = txReceipt.mAccountId.value();

    // Create receiver account
    const std::shared_ptr<PrivateKey> receiverPrivateKey = ED25519PrivateKey::generatePrivateKey();
    txReceipt = AccountCreateTransaction()
                  .setKeyWithoutAlias(receiverPrivateKey->getPublicKey())
                  .setMaxAutomaticTokenAssociations(100)
                  .setInitialBalance(Hbar(10ULL))
                  .addHook(hookDetails)
                  .execute(client)
                  .getReceipt(client);

    if (!txReceipt.mAccountId.has_value())
    {
      std::cerr << "Failed to create receiver account!" << std::endl;
      return 1;
    }

    const AccountId receiverAccountId = txReceipt.mAccountId.value();

    std::cout << "Creating fungible token..." << std::endl;
    txReceipt = TokenCreateTransaction()
                  .setTokenName("Example Fungible Token")
                  .setTokenSymbol("EFT")
                  .setTokenType(TokenType::FUNGIBLE_COMMON)
                  .setDecimals(2)
                  .setInitialSupply(10000)
                  .setTreasuryAccountId(senderAccountId)
                  .setAdminKey(senderPrivateKey->getPublicKey())
                  .setSupplyKey(senderPrivateKey->getPublicKey())
                  .freezeWith(&client)
                  .sign(senderPrivateKey)
                  .execute(client)
                  .getReceipt(client);

    if (!txReceipt.mTokenId.has_value())
    {
      std::cerr << "Failed to create fungible token!" << std::endl;
      return 1;
    }

    const TokenId fungibleTokenId = txReceipt.mTokenId.value();
    std::cout << "Created fungible token with ID: " << fungibleTokenId.toString() << std::endl;

    std::cout << "Creating NFT token..." << std::endl;
    txReceipt = TokenCreateTransaction()
                  .setTokenName("Example NFT Token")
                  .setTokenSymbol("ENT")
                  .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                  .setTreasuryAccountId(senderAccountId)
                  .setAdminKey(senderPrivateKey->getPublicKey())
                  .setSupplyKey(senderPrivateKey->getPublicKey())
                  .freezeWith(&client)
                  .sign(senderPrivateKey)
                  .execute(client)
                  .getReceipt(client);

    if (!txReceipt.mTokenId.has_value())
    {
      std::cerr << "Failed to create NFT token!" << std::endl;
      return 1;
    }

    const TokenId nftTokenId = txReceipt.mTokenId.value();
    std::cout << "Created NFT token with ID: " << nftTokenId.toString() << std::endl;

    std::cout << "Minting NFT..." << std::endl;
    const std::vector<std::byte> metadata = internal::Utilities::stringToByteVector("Example NFT Metadata");
    txReceipt = TokenMintTransaction()
                  .setTokenId(nftTokenId)
                  .addMetadata(metadata)
                  .freezeWith(&client)
                  .sign(senderPrivateKey)
                  .execute(client)
                  .getReceipt(client);

    if (txReceipt.mSerialNumbers.empty())
    {
      std::cerr << "Failed to mint NFT!" << std::endl;
      return 1;
    }

    const NftId nftId(nftTokenId, txReceipt.mSerialNumbers.front());
    std::cout << "Minted NFT with ID: " << nftId.toString() << std::endl;

    /*
     * Step 2: Demonstrate TransferTransaction API with hooks (demonstration only)
     */
    std::cout << "\n=== TransferTransaction with Hooks API Demonstration ===" << std::endl;

    // Create different hooks for different transfer types (for demonstration)
    std::cout << "Creating hook call objects (demonstration)..." << std::endl;

    // HBAR transfer with pre-tx allowance hook
    EvmHookCall hbarEvmHookCall;
    hbarEvmHookCall.setData({ std::byte{ 0x01 }, std::byte{ 0x02 } });
    hbarEvmHookCall.setGasLimit(20000ULL);

    FungibleHookCall hbarHook;
    hbarHook.setHookId(1LL);
    hbarHook.setEvmHookCall(hbarEvmHookCall);
    hbarHook.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);

    // NFT sender hook (pre-hook)
    EvmHookCall nftSenderEvmHookCall;
    nftSenderEvmHookCall.setData({ std::byte{ 0x03 }, std::byte{ 0x04 } });
    nftSenderEvmHookCall.setGasLimit(20000ULL);

    NftHookCall nftSenderHook;
    nftSenderHook.setHookId(1LL);
    nftSenderHook.setEvmHookCall(nftSenderEvmHookCall);
    nftSenderHook.setHookType(NftHookType::PRE_HOOK);

    // NFT receiver hook (pre-hook)
    EvmHookCall nftReceiverEvmHookCall;
    nftReceiverEvmHookCall.setData({ std::byte{ 0x05 }, std::byte{ 0x06 } });
    nftReceiverEvmHookCall.setGasLimit(20000ULL);

    NftHookCall nftReceiverHook;
    nftReceiverHook.setHookId(1LL);
    nftReceiverHook.setEvmHookCall(nftReceiverEvmHookCall);
    nftReceiverHook.setHookType(NftHookType::PRE_HOOK);

    // Fungible token transfer with pre-post allowance hook
    EvmHookCall fungibleTokenEvmHookCall;
    fungibleTokenEvmHookCall.setData({ std::byte{ 0x07 }, std::byte{ 0x08 } });
    fungibleTokenEvmHookCall.setGasLimit(20000ULL);

    FungibleHookCall fungibleTokenHook;
    fungibleTokenHook.setHookId(1LL);
    fungibleTokenHook.setEvmHookCall(fungibleTokenEvmHookCall);
    fungibleTokenHook.setHookType(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);

    // Build separate TransferTransactions with hooks (demonstration)
    std::cout << "Building separate TransferTransactions with hooks..." << std::endl;

    // Transaction 1: HBAR transfers with hook
    std::cout << "\n1. Building HBAR TransferTransaction with hook..." << std::endl;
    txReceipt = TransferTransaction()
                  .addHbarTransferWithHook(senderAccountId, Hbar(-1LL), hbarHook)
                  .addHbarTransfer(receiverAccountId, Hbar(1LL))
                  .execute(client)
                  .getReceipt(client);

    std::cout << "HBAR transfer completed with status: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

    // Transaction 2: NFT transfer with sender and receiver hooks
    std::cout << "\n2. Building NFT TransferTransaction with hooks..." << std::endl;
    txReceipt = TransferTransaction()
                  .addNftTransferWithHook(nftId, senderAccountId, receiverAccountId, nftSenderHook, nftReceiverHook)
                  .execute(client)
                  .getReceipt(client);

    std::cout << "NFT transfer completed with status: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

    // Transaction 3: Fungible token transfers with hook
    std::cout << "\n3. Building Fungible Token TransferTransaction with hook..." << std::endl;
    txReceipt = TransferTransaction()
                  .addTokenTransferWithHook(fungibleTokenId, senderAccountId, -1000LL, fungibleTokenHook)
                  .addTokenTransfer(fungibleTokenId, receiverAccountId, 1000LL)
                  .execute(client)
                  .getReceipt(client);

    std::cout << "Fungible token transfer completed with status: " << gStatusToString.at(txReceipt.mStatus)
              << std::endl;

    std::cout << "\nAll TransferTransactions executed successfully with the following hook calls:" << std::endl;
    std::cout << "  - Transaction 1: HBAR transfer with pre-tx allowance hook" << std::endl;
    std::cout << "  - Transaction 2: NFT transfer with sender and receiver hooks" << std::endl;
    std::cout << "  - Transaction 3: Fungible token transfer with pre-post allowance hook" << std::endl;

    std::cout << "Transfer Transaction Hooks Example Complete!" << std::endl;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Error occurred: " << error.what() << std::endl;
    return 1;
  }

  client.close();
  return 0;
}
