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
#include "hooks/EvmHook.h"
#include "hooks/EvmHookCall.h"
#include "hooks/FungibleHookCall.h"
#include "hooks/FungibleHookType.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/NftHookCall.h"
#include "hooks/NftHookType.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

static const std::string kHookBytecodeHex =
  "608060405234801561001057600080fd5b50600436106100365760003560e01c8063c29855781461003b578063f2fde38b14610059575b60"
  "0080fd5b610043610075565b60405161005091906100a1565b60405180910390f35b610073600480360381019061006e91906100ed565b61"
  "007b565b005b60005481565b8073ffffffffffffffffffffffffffffffffffffffff1660008054906101000a900473ffffffffffffffffff"
  "ffffffffffffffffffffff1673ffffffffffffffffffffffffffffffffffffffff167f8be0079c531659141344cd1fd0a4f28419497f9722"
  "a3daafe3b4186f6b6457e060405160405180910390a3806000806101000a81548173ffffffffffffffffffffffffffffffffffffffff0219"
  "16908373ffffffffffffffffffffffffffffffffffffffff16021790555050565b6000819050919050565b61009b81610088565b82525050"
  "565b60006020820190506100b66000830184610092565b92915050565b600080fd5b600073ffffffffffffffffffffffffffffffffffffff"
  "ff82169050919050565b60006100e7826100bc565b9050919050565b6100f7816100dc565b811461010257600080fd5b50565b6000813590"
  "50610114816100ee565b92915050565b6000602082840312156101305761012f6100bc565b5b600061013e84828501610105565b91505092"
  "91505056fea2646970667358221220";

struct Prerequisites
{
  AccountId senderAccountId;
  std::shared_ptr<PrivateKey> senderPrivateKey;
  AccountId receiverAccountId;
  TokenId fungibleTokenId;
  NftId nftId;
};

struct HookCalls
{
  FungibleHookCall hbarHook;
  NftHookCall nftSenderHook;
  NftHookCall nftReceiverHook;
  FungibleHookCall fungibleTokenHook;
};

std::pair<AccountId, std::shared_ptr<PrivateKey>> createSenderAccount(Client& client,
                                                                      const HookCreationDetails& hookDetails)
{
  const std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  const TransactionReceipt txReceipt = AccountCreateTransaction()
                                         .setKeyWithoutAlias(senderKey->getPublicKey())
                                         .setInitialBalance(Hbar(10ULL))
                                         .addHook(hookDetails)
                                         .freezeWith(&client)
                                         .sign(senderKey)
                                         .execute(client)
                                         .getReceipt(client);
  if (!txReceipt.mAccountId.has_value())
  {
    throw std::runtime_error("Failed to create sender account!");
  }
  return { txReceipt.mAccountId.value(), senderKey };
}

AccountId createReceiverAccount(Client& client, const HookCreationDetails& hookDetails)
{
  const TransactionReceipt txReceipt = AccountCreateTransaction()
                                         .setKeyWithoutAlias(ED25519PrivateKey::generatePrivateKey()->getPublicKey())
                                         .setMaxAutomaticTokenAssociations(100)
                                         .setInitialBalance(Hbar(10ULL))
                                         .addHook(hookDetails)
                                         .execute(client)
                                         .getReceipt(client);
  if (!txReceipt.mAccountId.has_value())
  {
    throw std::runtime_error("Failed to create receiver account!");
  }
  return txReceipt.mAccountId.value();
}

TokenId createFungibleToken(Client& client,
                            const std::shared_ptr<PrivateKey>& senderKey,
                            const AccountId& senderAccountId)
{
  std::cout << "Creating fungible token..." << std::endl;
  const TransactionReceipt txReceipt = TokenCreateTransaction()
                                         .setTokenName("Example Fungible Token")
                                         .setTokenSymbol("EFT")
                                         .setTokenType(TokenType::FUNGIBLE_COMMON)
                                         .setDecimals(2)
                                         .setInitialSupply(10000)
                                         .setTreasuryAccountId(senderAccountId)
                                         .setAdminKey(senderKey->getPublicKey())
                                         .setSupplyKey(senderKey->getPublicKey())
                                         .freezeWith(&client)
                                         .sign(senderKey)
                                         .execute(client)
                                         .getReceipt(client);
  if (!txReceipt.mTokenId.has_value())
  {
    throw std::runtime_error("Failed to create fungible token!");
  }
  std::cout << "Created fungible token with ID: " << txReceipt.mTokenId.value().toString() << std::endl;
  return txReceipt.mTokenId.value();
}

NftId mintNft(Client& client, const std::shared_ptr<PrivateKey>& senderKey, const AccountId& senderAccountId)
{
  std::cout << "Creating NFT token..." << std::endl;
  TransactionReceipt txReceipt = TokenCreateTransaction()
                                   .setTokenName("Example NFT Token")
                                   .setTokenSymbol("ENT")
                                   .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                                   .setTreasuryAccountId(senderAccountId)
                                   .setAdminKey(senderKey->getPublicKey())
                                   .setSupplyKey(senderKey->getPublicKey())
                                   .freezeWith(&client)
                                   .sign(senderKey)
                                   .execute(client)
                                   .getReceipt(client);
  if (!txReceipt.mTokenId.has_value())
  {
    throw std::runtime_error("Failed to create NFT token!");
  }
  const TokenId nftTokenId = txReceipt.mTokenId.value();
  std::cout << "Created NFT token with ID: " << nftTokenId.toString() << std::endl;
  std::cout << "Minting NFT..." << std::endl;
  txReceipt = TokenMintTransaction()
                .setTokenId(nftTokenId)
                .addMetadata(internal::Utilities::stringToByteVector("Example NFT Metadata"))
                .freezeWith(&client)
                .sign(senderKey)
                .execute(client)
                .getReceipt(client);
  if (txReceipt.mSerialNumbers.empty())
  {
    throw std::runtime_error("Failed to mint NFT!");
  }
  const NftId nftId(nftTokenId, txReceipt.mSerialNumbers.front());
  std::cout << "Minted NFT with ID: " << nftId.toString() << std::endl;
  return nftId;
}

std::pair<TokenId, NftId> createTokensAndNft(Client& client,
                                             const std::shared_ptr<PrivateKey>& senderKey,
                                             const AccountId& senderAccountId)
{
  return { createFungibleToken(client, senderKey, senderAccountId), mintNft(client, senderKey, senderAccountId) };
}

Prerequisites setupPrerequisites(Client& client, const std::shared_ptr<PrivateKey>& operatorKey)
{
  const std::vector<std::byte> hookBytecode = internal::HexConverter::hexToBytes(kHookBytecodeHex);
  const TransactionReceipt contractReceipt = ContractCreateTransaction()
                                               .setAdminKey(operatorKey->getPublicKey())
                                               .setGas(1000000ULL)
                                               .setBytecode(hookBytecode)
                                               .freezeWith(&client)
                                               .sign(operatorKey)
                                               .execute(client)
                                               .getReceipt(client);
  if (!contractReceipt.mContractId.has_value())
  {
    throw std::runtime_error("Failed to create hook contract!");
  }
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(contractReceipt.mContractId.value());
  EvmHook evmHook;
  evmHook.setEvmHookSpec(evmHookSpec);
  HookCreationDetails hookDetails;
  hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookDetails.setHookId(1LL);
  hookDetails.setEvmHook(evmHook);
  const auto [senderAccountId, senderKey] = createSenderAccount(client, hookDetails);
  const AccountId receiverAccountId = createReceiverAccount(client, hookDetails);
  const auto [fungibleTokenId, nftId] = createTokensAndNft(client, senderKey, senderAccountId);
  return { senderAccountId, senderKey, receiverAccountId, fungibleTokenId, nftId };
}

HookCalls buildHookCalls()
{
  EvmHookCall hbarEvmHookCall;
  hbarEvmHookCall.setData({ std::byte{ 0x01 }, std::byte{ 0x02 } });
  hbarEvmHookCall.setGasLimit(20000ULL);
  FungibleHookCall hbarHook;
  hbarHook.setHookId(1LL);
  hbarHook.setEvmHookCall(hbarEvmHookCall);
  hbarHook.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);

  EvmHookCall nftSenderEvmHookCall;
  nftSenderEvmHookCall.setData({ std::byte{ 0x03 }, std::byte{ 0x04 } });
  nftSenderEvmHookCall.setGasLimit(20000ULL);
  NftHookCall nftSenderHook;
  nftSenderHook.setHookId(1LL);
  nftSenderHook.setEvmHookCall(nftSenderEvmHookCall);
  nftSenderHook.setHookType(NftHookType::PRE_HOOK);

  EvmHookCall nftReceiverEvmHookCall;
  nftReceiverEvmHookCall.setData({ std::byte{ 0x05 }, std::byte{ 0x06 } });
  nftReceiverEvmHookCall.setGasLimit(20000ULL);
  NftHookCall nftReceiverHook;
  nftReceiverHook.setHookId(1LL);
  nftReceiverHook.setEvmHookCall(nftReceiverEvmHookCall);
  nftReceiverHook.setHookType(NftHookType::PRE_HOOK);

  EvmHookCall fungibleTokenEvmHookCall;
  fungibleTokenEvmHookCall.setData({ std::byte{ 0x07 }, std::byte{ 0x08 } });
  fungibleTokenEvmHookCall.setGasLimit(20000ULL);
  FungibleHookCall fungibleTokenHook;
  fungibleTokenHook.setHookId(1LL);
  fungibleTokenHook.setEvmHookCall(fungibleTokenEvmHookCall);
  fungibleTokenHook.setHookType(FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);

  return { hbarHook, nftSenderHook, nftReceiverHook, fungibleTokenHook };
}

void run(Client& client, const std::shared_ptr<PrivateKey>& operatorKey)
{
  std::cout << "Transfer Transaction Hooks Example Start!" << std::endl;
  std::cout << "Setting up prerequisites..." << std::endl;
  const auto [senderAccountId, senderPrivateKey, receiverAccountId, fungibleTokenId, nftId] =
    setupPrerequisites(client, operatorKey);
  const auto [hbarHook, nftSenderHook, nftReceiverHook, fungibleTokenHook] = buildHookCalls();

  std::cout << "\n=== TransferTransaction with Hooks API Demonstration ===" << std::endl;
  std::cout << "\n1. Building HBAR TransferTransaction with hook..." << std::endl;
  TransactionReceipt txReceipt = TransferTransaction()
                                   .addHbarTransferWithHook(senderAccountId, Hbar(-1LL), hbarHook)
                                   .addHbarTransfer(receiverAccountId, Hbar(1LL))
                                   .execute(client)
                                   .getReceipt(client);
  std::cout << "HBAR transfer completed with status: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  std::cout << "\n2. Building NFT TransferTransaction with hooks..." << std::endl;
  txReceipt = TransferTransaction()
                .addNftTransferWithHook(nftId, senderAccountId, receiverAccountId, nftSenderHook, nftReceiverHook)
                .execute(client)
                .getReceipt(client);
  std::cout << "NFT transfer completed with status: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  std::cout << "\n3. Building Fungible Token TransferTransaction with hook..." << std::endl;
  txReceipt = TransferTransaction()
                .addTokenTransferWithHook(fungibleTokenId, senderAccountId, -1000LL, fungibleTokenHook)
                .addTokenTransfer(fungibleTokenId, receiverAccountId, 1000LL)
                .execute(client)
                .getReceipt(client);
  std::cout << "Fungible token transfer completed with status: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  std::cout << "\nAll TransferTransactions executed successfully." << std::endl;
  std::cout << "Transfer Transaction Hooks Example Complete!" << std::endl;
}

int main(int argc, char** argv)
{
  dotenv::init();
  if (std::getenv("OPERATOR_ID") == nullptr || std::getenv("OPERATOR_KEY") == nullptr ||
      std::getenv("NETWORK_NAME") == nullptr)
  {
    std::cerr << "Environment variables OPERATOR_ID, NETWORK_NAME, and OPERATOR_KEY are required." << std::endl;
    return 1;
  }
  Client client = Client::forName(std::getenv("NETWORK_NAME"));
  client.setOperator(AccountId::fromString(std::getenv("OPERATOR_ID")),
                     ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY")));
  try
  {
    run(client, ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY")));
  }
  catch (const std::exception& error)
  {
    std::cerr << "Error occurred: " << error.what() << std::endl;
    return 1;
  }
  client.close();
  return 0;
}
