// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountAllowanceDeleteTransaction.h"
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "NftId.h"
#include "Status.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenSupplyType.h"
#include "TokenType.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "impl/Utilities.h"

#include <dotenv.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Hiero;

/**
 * Example for HIP-336. Show functionalities around approve/delete an allowance for:
 *   1. Single NFT serial numbers
 *   2. All serial numbers at once
 *   3. Delegating spender obligations
 *
 * Note that the concept around the ERC standard that Hiero implements in regard to the allowances for NFTs does not
 * allow users to:
 *   1. Approve allowance for all serials in a NFT collection, then remove allowance for individual serial of the NFT
 *   2. Approve allowance for individual serial of the NFT, then remove allowance for all serials in the NFT collection
 */
int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // The CIDs for the NFTs.
  const std::vector<std::string> CIDs = { "QmNPCiNA3Dsu3K5FxDPMG5Q3fZRwVTg14EXA92uqEeSRXn",
                                          "QmZ4dgAgt8owvnULxnKxNe8YqpavtVCXmc1Lt2XajFpJs9",
                                          "QmPzY5GxevjyfMUF5vEAjtyRoigzWp47MiKAtLBduLMC1T" };

  // Create the NFT token class.
  const TokenId tokenId = TokenCreateTransaction()
                            .setTokenName("NFT Token")
                            .setTokenSymbol("NFTT")
                            .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                            .setMaxSupply(CIDs.size())
                            .setTreasuryAccountId(operatorAccountId)
                            .setSupplyType(TokenSupplyType::FINITE)
                            .setAdminKey(operatorPrivateKey)
                            .setSupplyKey(operatorPrivateKey)
                            .execute(client)
                            .getReceipt(client)
                            .mTokenId.value();
  std::cout << "Created NFT class with token ID: " << tokenId.toString() << std::endl;

  // Mint an NFT for each CID.
  std::vector<int64_t> serialNumbers;
  for (const auto& metadata : CIDs)
  {
    serialNumbers.push_back(TokenMintTransaction()
                              .setTokenId(tokenId)
                              .addMetadata(internal::Utilities::stringToByteVector(metadata))
                              .execute(client)
                              .getReceipt(client)
                              .mSerialNumbers.front());
    std::cout << "Minted NFT " << tokenId.toString() << " with serial number " << serialNumbers.back() << std::endl;
  }

  // Create a spender account to spend the NFTs.
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> spenderKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const AccountId spenderAccountId = AccountCreateTransaction()
                                       .setKey(spenderKey)
                                       .setInitialBalance(Hbar(2LL))
                                       .execute(client)
                                       .getReceipt(client)
                                       .mAccountId.value();
  std::cout << "Created spender account with ID " << spenderAccountId.toString() << std::endl;

  // Create a receiver account to receive the NFTs.
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> receiverKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const AccountId receiverAccountId = AccountCreateTransaction()
                                        .setKey(receiverKey)
                                        .setInitialBalance(Hbar(2LL))
                                        .execute(client)
                                        .getReceipt(client)
                                        .mAccountId.value();
  std::cout << "Created receiver account with ID " << receiverAccountId.toString() << std::endl;

  // Associate the spender with the NFTs.
  std::cout << "Associating the spender with the NFTs: "
            << gStatusToString.at(TokenAssociateTransaction()
                                    .setAccountId(spenderAccountId)
                                    .setTokenIds({ tokenId })
                                    .freezeWith(&client)
                                    .sign(spenderKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Associate the receiver with the NFTs.
  std::cout << "Associating the receiver with the NFTs: "
            << gStatusToString.at(TokenAssociateTransaction()
                                    .setAccountId(receiverAccountId)
                                    .setTokenIds({ tokenId })
                                    .freezeWith(&client)
                                    .sign(receiverKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl
            << std::endl;

  std::cout << "Example 1: Approve/delete allowances for single serial numbers" << std::endl
            << "--------------------------------------------------------------" << std::endl
            << std::endl;

  // Give the spender an allowance of the NFTs.
  const NftId nft1(tokenId, serialNumbers.at(0));
  const NftId nft2(tokenId, serialNumbers.at(1));
  std::cout << "Approve spender allowance for NFT serials " << nft1.mSerialNum << " and " << nft2.mSerialNum << ": "
            << gStatusToString.at(AccountAllowanceApproveTransaction()
                                    .approveTokenNftAllowance(nft1, operatorAccountId, spenderAccountId)
                                    .approveTokenNftAllowance(nft2, operatorAccountId, spenderAccountId)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Send NFT1 to receiver account, using the spender account's allowance. A TransactionId must be generated manually
  // from the spender account ID in order for the transaction to be executed on behalf of the spender.
  std::cout << "Transfer NFT " << nft1.mSerialNum << " on behalf of the spender: "
            << gStatusToString.at(TransferTransaction()
                                    .addApprovedNftTransfer(nft1, operatorAccountId, receiverAccountId)
                                    .setTransactionId(TransactionId::generate(spenderAccountId))
                                    .freezeWith(&client)
                                    .sign(spenderKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Remove the spender's allowance for NFT2.
  std::cout << "Remove NFT " << nft2.mSerialNum << " allowance from spender: "
            << gStatusToString.at(AccountAllowanceDeleteTransaction()
                                    .deleteAllTokenNftAllowances(nft2, operatorAccountId)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Attempt to transfer NFT2 using the spender's now-deleted allowance. This should not work.
  std::cout << "Transfer NFT " << nft2.mSerialNum << " with no allowance (should return "
            << gStatusToString.at(Status::SPENDER_DOES_NOT_HAVE_ALLOWANCE) << "): "
            << gStatusToString.at(TransferTransaction()
                                    .addApprovedNftTransfer(nft2, operatorAccountId, receiverAccountId)
                                    .setTransactionId(TransactionId::generate(spenderAccountId))
                                    .freezeWith(&client)
                                    .sign(spenderKey)
                                    .execute(client)
                                    .setValidateStatus(false)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Return NFT1 back to the treasury account.
  std::cout << "Transfer NFT " << nft1.mSerialNum << " back to the operator: "
            << gStatusToString.at(TransferTransaction()
                                    .addNftTransfer(nft1, receiverAccountId, operatorAccountId)
                                    .freezeWith(&client)
                                    .sign(receiverKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Remove the allowance for NFT1 from the spender account.
  std::cout << "Remove NFT " << nft1.mSerialNum << " allowance from spender: "
            << gStatusToString.at(AccountAllowanceDeleteTransaction()
                                    .deleteAllTokenNftAllowances(nft1, operatorAccountId)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  std::cout << std::endl
            << "Example 2: Approve/delete allowances for all serial numbers at once" << std::endl
            << "--------------------------------------------------------------" << std::endl
            << std::endl;

  // Create a delegate spender account.
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> delegateSpenderKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const AccountId delegateSpenderAccountId = AccountCreateTransaction()
                                               .setKey(delegateSpenderKey)
                                               .setInitialBalance(Hbar(2LL))
                                               .execute(client)
                                               .getReceipt(client)
                                               .mAccountId.value();
  std::cout << "Created delegate spender account with ID: " << delegateSpenderAccountId.toString() << std::endl;

  // Grant the spender an allowance for all the operator's NFTs.
  std::cout << "Approve allowance for the spender account for all NFT serials: "
            << gStatusToString.at(AccountAllowanceApproveTransaction()
                                    .approveNftAllowanceAllSerials(tokenId, operatorAccountId, spenderAccountId)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Give the delegate spender an allowance on NFT3 on behalf of the spender account, which currently has
  // 'approveForAll' rights.
  const NftId nft3(tokenId, serialNumbers.at(2));
  std::cout << "Approve allowance for the delegate spender account on behalf of the spender account for NFT3: "
            << gStatusToString.at(
                 AccountAllowanceApproveTransaction()
                   .approveTokenNftAllowance(nft3, operatorAccountId, delegateSpenderAccountId, spenderAccountId)
                   .freezeWith(&client)
                   .sign(spenderKey)
                   .execute(client)
                   .getReceipt(client)
                   .mStatus)
            << std::endl;

  // Send NFT1 to receiver account, using the spender account's allowance. A TransactionId must be generated manually
  // from the spender account ID in order for the transaction to be executed on behalf of the spender.
  std::cout << "Transfer NFT " << nft1.mSerialNum << " on behalf of the spender: "
            << gStatusToString.at(TransferTransaction()
                                    .addApprovedNftTransfer(nft1, operatorAccountId, receiverAccountId)
                                    .setTransactionId(TransactionId::generate(spenderAccountId))
                                    .freezeWith(&client)
                                    .sign(spenderKey)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Remove the spender's allowance for all serials.
  std::cout << "Remove all allowances for spender: "
            << gStatusToString.at(AccountAllowanceApproveTransaction()
                                    .deleteNftAllowanceAllSerials(tokenId, operatorAccountId, spenderAccountId)
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Attempt to transfer NFT2 using the spender's now-deleted allowance. This should not work.
  std::cout << "Transfer NFT " << nft2.mSerialNum << " with no allowance (should return "
            << gStatusToString.at(Status::SPENDER_DOES_NOT_HAVE_ALLOWANCE) << "): "
            << gStatusToString.at(TransferTransaction()
                                    .addApprovedNftTransfer(nft2, operatorAccountId, receiverAccountId)
                                    .setTransactionId(TransactionId::generate(spenderAccountId))
                                    .freezeWith(&client)
                                    .sign(spenderKey)
                                    .execute(client)
                                    .setValidateStatus(false)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  return 0;
}
