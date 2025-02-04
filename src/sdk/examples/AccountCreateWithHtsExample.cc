// SPDX-License-Identifier: Apache-2.0
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenId.h"
#include "TokenMintTransaction.h"
#include "TokenNftInfo.h"
#include "TokenNftInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "impl/Utilities.h"

#include <array>
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

  // IPFS content identifiers for the NFT metadata
  const std::vector<std::vector<std::byte>> CIDs = {
    internal::Utilities::stringToByteVector("QmNPCiNA3Dsu3K5FxDPMG5Q3fZRwVTg14EXA92uqEeSRXn"),
    internal::Utilities::stringToByteVector("QmZ4dgAgt8owvnULxnKxNe8YqpavtVCXmc1Lt2XajFpJs9"),
    internal::Utilities::stringToByteVector("QmPzY5GxevjyfMUF5vEAjtyRoigzWp47MiKAtLBduLMC1T"),
    internal::Utilities::stringToByteVector("Qmd3kGgSrAwwSrhesYcY7K54f3qD7MDo38r7Po2dChtQx5"),
    internal::Utilities::stringToByteVector("QmWgkKz3ozgqtnvbCLeh7EaR1H8u5Sshx3ZJzxkcrT3jbw")
  };

  std::cout << "Example 1" << std::endl;
  std::cout << "---------" << std::endl;

  /**
   * Step 1: Create an NFT using the Hiero token service.
   */
  TokenId tokenId = TokenCreateTransaction()
                      .setTokenName("HIP-542 Example Collection")
                      .setTokenSymbol("HIP-542")
                      .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                      .setDecimals(0)
                      .setInitialSupply(0)
                      .setMaxSupply(CIDs.size())
                      .setTreasuryAccountId(operatorAccountId)
                      .setSupplyType(TokenSupplyType::FINITE)
                      .setAdminKey(operatorPrivateKey)
                      .setSupplyKey(operatorPrivateKey)
                      .execute(client)
                      .getReceipt(client)
                      .mTokenId.value();
  std::cout << "Created NFT with ID: " << tokenId.toString() << std::endl;

  /**
   * Step 2: Mint the NFTs.
   */
  TransactionReceipt txReceipt = TokenMintTransaction()
                                   .setMaxTransactionFee(Hbar(10LL))
                                   .setTokenId(tokenId)
                                   .setMetadata(CIDs)
                                   .execute(client)
                                   .getReceipt(client);
  std::cout << "Minted " << txReceipt.mSerialNumbers.size() << " NFTs" << std::endl;

  /**
   * Step 3: Create an ECDSAsecp256k1PublicKey alias.
   */
  std::shared_ptr<PublicKey> alias = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  AccountId aliasAccountId = alias->toAccountId();
  std::cout << "Created alias: " << aliasAccountId.toString() << std::endl;

  /**
   * Step 4: Transfer an NFT to the ECDSAsecp256k1PublicKey alias.
   */
  const auto nftId = NftId(tokenId, txReceipt.mSerialNumbers.at(0));
  std::cout << "Transferring NFT " << nftId.toString() << " to alias account: "
            << gStatusToString.at(TransferTransaction()
                                    .addNftTransfer(nftId, operatorAccountId, aliasAccountId)
                                    .execute(client)
                                    .setValidateStatus(false)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  /**
   * Step 5: Query the NFT to see the owner.
   */
  const TokenNftInfo tokenNftInfo = TokenNftInfoQuery().setNftId(nftId).execute(client);
  std::cout << "NFT " << nftId.toString() << " owner: " << tokenNftInfo.mAccountId.toString() << std::endl;

  /**
   * Step 6: Get the new account ID of the alias.
   */
  std::cout << "The NFT owner account ID "
            << (AccountInfoQuery().setAccountId(aliasAccountId).execute(client).mAccountId == tokenNftInfo.mAccountId
                  ? "matches "
                  : "does not match ")
            << "the account ID created by HTS." << std::endl;

  std::cout << std::endl;
  std::cout << "Example 2" << std::endl;
  std::cout << "---------" << std::endl;

  /**
   * Step 1: Create a fungible token using the Hiero token service.
   */
  tokenId = TokenCreateTransaction()
              .setTokenName("HIP-542 Token")
              .setTokenSymbol("H542")
              .setTokenType(TokenType::FUNGIBLE_COMMON)
              .setTreasuryAccountId(operatorAccountId)
              .setInitialSupply(10000) // Total supply = 10000 / 10 ^ 2
              .setDecimals(2)
              .setAutoRenewAccountId(operatorAccountId)
              .execute(client)
              .getReceipt(client)
              .mTokenId.value();
  std::cout << "Created fungible token with ID: " << tokenId.toString() << std::endl;

  /**
   * Step 2: Create an ECDSAsecp256k1PublicKey alias.
   */
  alias = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  aliasAccountId = alias->toAccountId();
  std::cout << "Created alias: " << aliasAccountId.toString() << std::endl;

  /**
   * Step 3: Transfer the fungible token to the ECDSAsecp256k1PublicKey alias.
   */
  std::cout << "Transferring token " << tokenId.toString() << " to alias account: "
            << gStatusToString.at(TransferTransaction()
                                    .addTokenTransfer(tokenId, operatorAccountId, -10LL)
                                    .addTokenTransfer(tokenId, aliasAccountId, 10LL)
                                    .execute(client)
                                    .setValidateStatus(false)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  /**
   * Step 4: Get the new account ID of the alias.
   */
  std::cout << "The new account ID of the alias is "
            << AccountInfoQuery().setAccountId(aliasAccountId).execute(client).mAccountId.toString() << std::endl;

  return 0;
}
