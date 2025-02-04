// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Please provide a parameter -ft, -nft for creating a fungible or non-fungible token" << std::endl;
    return 0;
  }

  TokenType tokenType;
  if (strcmp(argv[1], "-ft") == 0)
  {
    tokenType = TokenType::FUNGIBLE_COMMON;
  }
  else if (strcmp(argv[1], "-nft") == 0)
  {
    tokenType = TokenType::NON_FUNGIBLE_UNIQUE;
  }
  else
  {
    std::cerr << "Please provide a parameter -ft, -nft for creating a fungible or non-fungible token" << std::endl;
    return 0;
  }

  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Metadata values for create/update
  const std::vector<std::byte> initialMetadata = { std::byte(0xAA), std::byte(0xAB), std::byte(0xAC), std::byte(0xAD) };
  const std::vector<std::byte> updatedMetadata = { std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD) };

  // Create a FT/NFT with metadata and admin key
  TokenId mutableTokenId = TokenCreateTransaction()
                             .setTokenName("ffff")
                             .setTokenSymbol("F")
                             .setDecimals(3)
                             .setInitialSupply(100000)
                             .setTokenType(tokenType)
                             .setMetadata(initialMetadata)
                             .setTreasuryAccountId(operatorAccountId)
                             .setAdminKey(operatorPrivateKey)
                             .freezeWith(&client)
                             .sign(operatorPrivateKey)
                             .execute(client)
                             .getReceipt(client)
                             .mTokenId.value();

  std::cout << "Created a mutable token " << mutableTokenId.toString() << " with metadata: " << std::endl;

  for (std::byte b : initialMetadata)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
  }
  std::cout << std::endl;

  // Update the token metadata
  TransactionReceipt txReceipt = TokenUpdateTransaction()
                                   .setTokenId(mutableTokenId)
                                   .setMetadata(updatedMetadata)
                                   .freezeWith(&client)
                                   .sign(operatorPrivateKey)
                                   .execute(client)
                                   .getReceipt(client);

  std::cout << "Updated mutable token " << mutableTokenId.toString() << " metadata:" << std::endl;

  TokenInfo tokenInfo = TokenInfoQuery().setTokenId(mutableTokenId).execute(client);

  for (std::byte b : tokenInfo.mMetadata)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
  }
  std::cout << std::endl;

  std::shared_ptr<PrivateKey> metadataKey = ED25519PrivateKey::generatePrivateKey();

  // Create an immutable FT/NFT with metadata and metadata key
  TokenId immutableTokenId = TokenCreateTransaction()
                               .setTokenName("ffff")
                               .setTokenSymbol("F")
                               .setDecimals(3)
                               .setInitialSupply(100000)
                               .setTokenType(tokenType)
                               .setMetadata(initialMetadata)
                               .setTreasuryAccountId(operatorAccountId)
                               .setMetadataKey(metadataKey)
                               .execute(client)
                               .getReceipt(client)
                               .mTokenId.value();

  std::cout << "Created a immutable token " << immutableTokenId.toString() << " with metadata: " << std::endl;

  for (std::byte b : initialMetadata)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
  }
  std::cout << std::endl;

  // Update the token metadata
  txReceipt = TokenUpdateTransaction()
                .setTokenId(immutableTokenId)
                .setMetadata(updatedMetadata)
                .freezeWith(&client)
                .sign(metadataKey)
                .execute(client)
                .getReceipt(client);

  std::cout << "Updated immutable token " << immutableTokenId.toString() << " metadata:" << std::endl;

  tokenInfo = TokenInfoQuery().setTokenId(immutableTokenId).execute(client);

  for (std::byte b : tokenInfo.mMetadata)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << " ";
  }
  std::cout << std::endl;
}