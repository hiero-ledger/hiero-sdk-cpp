// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PendingAirdropId.h"
#include "PrivateKey.h"
#include "TokenAirdropTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenCancelAirdropTransaction.h"
#include "TokenClaimAirdropTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenMintTransaction.h"
#include "TokenPauseTransaction.h"
#include "TokenRejectTransaction.h"
#include "TokenUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

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

  // Step 1: Create 4 accounts
  std::shared_ptr<PrivateKey> aliceKey = ED25519PrivateKey::generatePrivateKey();

  AccountId alice = AccountCreateTransaction()
                      .setKey(aliceKey)
                      .setInitialBalance(Hbar(5LL))
                      .execute(client)
                      .getReceipt(client)
                      .mAccountId.value();

  std::shared_ptr<PrivateKey> bobKey = ED25519PrivateKey::generatePrivateKey();

  AccountId bob = AccountCreateTransaction()
                    .setKey(bobKey)
                    .setInitialBalance(Hbar(5LL))
                    .execute(client)
                    .getReceipt(client)
                    .mAccountId.value();

  std::shared_ptr<PrivateKey> carolKey = ED25519PrivateKey::generatePrivateKey();

  AccountId carol = AccountCreateTransaction()
                      .setKey(carolKey)
                      .setInitialBalance(Hbar(5LL))
                      .execute(client)
                      .getReceipt(client)
                      .mAccountId.value();

  std::shared_ptr<PrivateKey> treasuryKey = ED25519PrivateKey::generatePrivateKey();

  AccountId treasury = AccountCreateTransaction()
                         .setKey(treasuryKey)
                         .setInitialBalance(Hbar(5LL))
                         .execute(client)
                         .getReceipt(client)
                         .mAccountId.value();

  // Step 2: Create FT and NFT and mint
  TokenId tokenID = TokenCreateTransaction()
                      .setTokenName("Fungible Token")
                      .setTokenSymbol("TFT")
                      .setTokenMemo("Example memo")
                      .setDecimals(3)
                      .setInitialSupply(100)
                      .setMaxSupply(100)
                      .setTreasuryAccountId(treasury)
                      .setSupplyType(TokenSupplyType::FINITE)
                      .setAdminKey(client.getOperatorPublicKey())
                      .setFreezeKey(client.getOperatorPublicKey())
                      .setSupplyKey(client.getOperatorPublicKey())
                      .setMetadataKey(client.getOperatorPublicKey())
                      .setPauseKey(client.getOperatorPublicKey())
                      .freezeWith(&client)
                      .sign(treasuryKey)
                      .execute(client)
                      .getReceipt(client)
                      .mTokenId.value();

  TokenId nftID = TokenCreateTransaction()
                    .setTokenName("Test NFT")
                    .setTokenSymbol("TNFT")
                    .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                    .setTreasuryAccountId(treasury)
                    .setSupplyType(TokenSupplyType::FINITE)
                    .setMaxSupply(10)
                    .setAdminKey(client.getOperatorPublicKey())
                    .setFreezeKey(client.getOperatorPublicKey())
                    .setSupplyKey(client.getOperatorPublicKey())
                    .setMetadataKey(client.getOperatorPublicKey())
                    .setPauseKey(client.getOperatorPublicKey())
                    .freezeWith(&client)
                    .sign(treasuryKey)
                    .execute(client)
                    .getReceipt(client)
                    .mTokenId.value();

  TokenMintTransaction()
    .setTokenId(nftID)
    .setMetadata({ { std::byte(10) }, { std::byte(11) }, { std::byte(12) } })
    .execute(client)
    .getReceipt(client);

  // Step 3: Airdrop fungible tokens to all 3 accounts
  std::cout << "Airdropping FTs" << std::endl;
  TransactionRecord txnRecord = TokenAirdropTransaction()
                                  .addTokenTransfer(tokenID, alice, 10)
                                  .addTokenTransfer(tokenID, treasury, -10)
                                  .addTokenTransfer(tokenID, bob, 10)
                                  .addTokenTransfer(tokenID, treasury, -10)
                                  .addTokenTransfer(tokenID, carol, 10)
                                  .addTokenTransfer(tokenID, treasury, -10)
                                  .freezeWith(&client)
                                  .sign(treasuryKey)
                                  .execute(client)
                                  .getRecord(client);

  // Step 4: Get the transaction record and see pending airdrops
  std::cout << "Pending airdrops length: " << txnRecord.mPendingAirdropRecords.size() << std::endl;
  std::cout << "Pending airdrops: " << txnRecord.mPendingAirdropRecords[0].toString() << std::endl;

  // Step 5: Query to verify balances
  AccountBalance aliceBalance = AccountBalanceQuery().setAccountId(alice).execute(client);
  AccountBalance bobBalance = AccountBalanceQuery().setAccountId(bob).execute(client);
  AccountBalance carolBalance = AccountBalanceQuery().setAccountId(carol).execute(client);

  std::cout << "Alice FT balance after airdrop: " << aliceBalance.mTokens[tokenID] << std::endl;
  std::cout << "Bob FT balance after airdrop: " << bobBalance.mTokens[tokenID] << std::endl;
  std::cout << "Carol FT balance after airdrop: " << carolBalance.mTokens[tokenID] << std::endl;

  // Step 6: Claim the airdrop for Carol
  std::cout << "Claiming FT with Carol" << std::endl;
  TokenClaimAirdropTransaction()
    .setPendingAirdrops({ txnRecord.mPendingAirdropRecords[0].mPendingAirdropId })
    .freezeWith(&client)
    .sign(carolKey)
    .execute(client)
    .getReceipt(client);

  carolBalance = AccountBalanceQuery().setAccountId(carol).execute(client);
  std::cout << "Carol FT balance after claim: " << carolBalance.mTokens[tokenID] << std::endl;

  // Step 7: Airdrop NFTs to all three accounts
  std::cout << "Airdropping NFTs" << std::endl;
  txnRecord = TokenAirdropTransaction()
                .addNftTransfer(nftID.nft(1), treasury, alice)
                .addNftTransfer(nftID.nft(2), treasury, bob)
                .addNftTransfer(nftID.nft(3), treasury, carol)
                .freezeWith(&client)
                .sign(treasuryKey)
                .execute(client)
                .getRecord(client);

  // Step 8: Get the transaction record and verify pending airdrops
  std::cout << "Pending airdrops length: " << txnRecord.mPendingAirdropRecords.size() << std::endl;
  std::cout << "Pending airdrops for Bob: " << txnRecord.mPendingAirdropRecords[0].toString() << std::endl;
  std::cout << "Pending airdrops for Carol: " << txnRecord.mPendingAirdropRecords[1].toString() << std::endl;

  // Step 9: Query to verify balances
  aliceBalance = AccountBalanceQuery().setAccountId(alice).execute(client);
  bobBalance = AccountBalanceQuery().setAccountId(bob).execute(client);
  carolBalance = AccountBalanceQuery().setAccountId(carol).execute(client);

  std::cout << "Alice NFT balance after airdrop: " << aliceBalance.mTokens[nftID] << std::endl;
  std::cout << "Bob NFT balance after airdrop: " << bobBalance.mTokens[nftID] << std::endl;
  std::cout << "Carol NFT balance after airdrop: " << carolBalance.mTokens[nftID] << std::endl;

  // Step 10: Claim the airdrop for Bob
  std::cout << "Claiming NFT with Bob" << std::endl;
  TokenClaimAirdropTransaction()
    .setPendingAirdrops({ txnRecord.mPendingAirdropRecords[0].mPendingAirdropId })
    .freezeWith(&client)
    .sign(bobKey)
    .execute(client)
    .getReceipt(client);

  bobBalance = AccountBalanceQuery().setAccountId(bob).execute(client);
  std::cout << "Bob NFT balance after claim: " << bobBalance.mTokens[nftID] << std::endl;

  // Step 11: Cancel the airdrop for Carol
  std::cout << "Canceling NFT for Carol" << std::endl;
  TokenCancelAirdropTransaction()
    .setPendingAirdrops({ txnRecord.mPendingAirdropRecords[1].mPendingAirdropId })
    .freezeWith(&client)
    .sign(treasuryKey)
    .execute(client)
    .getReceipt(client);

  carolBalance = AccountBalanceQuery().setAccountId(carol).execute(client);
  std::cout << "Carol NFT balance after cancel: " << carolBalance.mTokens[nftID] << std::endl;

  // Step 12: Reject the NFT for Bob
  std::cout << "Rejecting NFT with Bob" << std::endl;
  TokenRejectTransaction()
    .setOwner(bob)
    .setNfts({ nftID.nft(2) })
    .freezeWith(&client)
    .sign(bobKey)
    .execute(client)
    .getReceipt(client);

  // Step 13: Query to verify Bob no longer has the NFT
  bobBalance = AccountBalanceQuery().setAccountId(bob).execute(client);
  std::cout << "Bob NFT balance after reject: " << bobBalance.mTokens[nftID] << std::endl;

  // Step 13: Query to verify the NFT was returned to the Treasury
  AccountBalance treasuryBalance = AccountBalanceQuery().setAccountId(treasury).execute(client);
  std::cout << "Treasury NFT balance after reject: " << treasuryBalance.mTokens[nftID] << std::endl;

  // Step 14: Reject the fungible tokens for Carol
  std::cout << "Rejecting FT with Carol" << std::endl;
  TokenRejectTransaction()
    .setOwner(carol)
    .setFts({ tokenID })
    .freezeWith(&client)
    .sign(carolKey)
    .execute(client)
    .getReceipt(client);

  // Step 14: Query to verify Carol no longer has the fungible tokens
  carolBalance = AccountBalanceQuery().setAccountId(carol).execute(client);
  std::cout << "Carol FT balance after reject: " << carolBalance.mTokens[tokenID] << std::endl;

  // Step 15: Query to verify Treasury received the rejected fungible tokens
  treasuryBalance = AccountBalanceQuery().setAccountId(treasury).execute(client);
  std::cout << "Treasury FT balance after reject: " << treasuryBalance.mTokens[tokenID] << std::endl;

  // Clean up:
  client.close();

  return 0;
}
