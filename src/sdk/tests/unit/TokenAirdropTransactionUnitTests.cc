#include "AccountId.h"
#include "NftId.h"
#include "TokenAirdropTransaction.h"
#include "TokenId.h"
#include <gtest/gtest.h>
#include <token_airdrop.pb.h>

using namespace Hiero;

class TokenAirdropTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId1() const { return mTokenId1; }
  [[nodiscard]] inline const TokenId& getTestTokenId2() const { return mTokenId2; }
  [[nodiscard]] inline const AccountId& getTestAccountId1() const { return mAccountId1; }
  [[nodiscard]] inline const AccountId& getTestAccountId2() const { return mAccountId2; }
  [[nodiscard]] inline const NftId& getTestNftId1() const { return mNftId1; }
  [[nodiscard]] inline const NftId& getTestNftId2() const { return mNftId2; }
  [[nodiscard]] inline const int64_t& getTestAmount() const { return mAmount; }
  [[nodiscard]] inline const uint32_t& getTestExpectedDecimals() const { return mExpectedDecimals; }
  [[nodiscard]] inline bool getTestIsApproval() const { return mIsApproval; }

private:
  const TokenId mTokenId1 = TokenId(1ULL);
  const TokenId mTokenId2 = TokenId(2ULL);
  const AccountId mAccountId1 = AccountId(101ULL);
  const AccountId mAccountId2 = AccountId(102ULL);
  const NftId mNftId1 = NftId(mTokenId1, 1ULL);
  const NftId mNftId2 = NftId(mTokenId2, 2ULL);
  const int64_t mAmount = 5000LL;
  const uint32_t mExpectedDecimals = 100U;
  const bool mIsApproval = false;
};

//-----
TEST_F(TokenAirdropTransactionUnitTests, AddTokenTransfer)
{
  // Given
  TokenAirdropTransaction transaction;

  // When
  transaction.addTokenTransfer(getTestTokenId1(), getTestAccountId1(), getTestAmount());

  // Then
  auto transfers = transaction.getTokenTransfers();
  EXPECT_EQ(transfers.size(), 1);
  EXPECT_EQ(transfers.at(getTestTokenId1()).at(getTestAccountId1()), getTestAmount());
}

//-----
TEST_F(TokenAirdropTransactionUnitTests, AddNftTransfer)
{
  // Given
  TokenAirdropTransaction transaction;

  // When
  transaction.addNftTransfer(getTestNftId1(), getTestAccountId1(), getTestAccountId2());

  // Then
  auto nftTransfers = transaction.getNftTransfers();
  EXPECT_EQ(nftTransfers.size(), 1);
  EXPECT_EQ(nftTransfers.at(getTestTokenId1()).size(), 1);
  const auto& nftTransfer = nftTransfers.at(getTestTokenId1()).at(0);
  EXPECT_EQ(nftTransfer.mNftId, getTestNftId1());
  EXPECT_EQ(nftTransfer.mSenderAccountId, getTestAccountId1());
  EXPECT_EQ(nftTransfer.mReceiverAccountId, getTestAccountId2());
}

//-----
TEST_F(TokenAirdropTransactionUnitTests, AddTokenTransferWithDecimals)
{
  // Given
  TokenAirdropTransaction transaction;

  // When
  transaction.addTokenTransferWithDecimals(
    getTestTokenId1(), getTestAccountId1(), getTestAmount(), getTestExpectedDecimals());

  // Then
  auto transfers = transaction.getTokenTransfers();
  auto decimals = transaction.getTokenIdDecimals();
  EXPECT_EQ(transfers.size(), 1);
  EXPECT_EQ(transfers.at(getTestTokenId1()).at(getTestAccountId1()), getTestAmount());
  EXPECT_EQ(decimals.at(getTestTokenId1()), getTestExpectedDecimals());
}

//-----
TEST_F(TokenAirdropTransactionUnitTests, AddApprovedTokenTransfer)
{
  // Given
  TokenAirdropTransaction transaction;

  // When
  transaction.addApprovedTokenTransfer(getTestTokenId1(), getTestAccountId1(), getTestAmount());

  // Then
  auto transfers = transaction.getTokenTransfers();
  EXPECT_EQ(transfers.size(), 1);
  EXPECT_EQ(transfers.at(getTestTokenId1()).at(getTestAccountId1()), getTestAmount());
}

//-----
TEST_F(TokenAirdropTransactionUnitTests, AddApprovedNftTransfer)
{
  // Given
  TokenAirdropTransaction transaction;

  // When
  transaction.addApprovedNftTransfer(getTestNftId1(), getTestAccountId1(), getTestAccountId2());

  // Then
  auto nftTransfers = transaction.getNftTransfers();
  EXPECT_EQ(nftTransfers.size(), 1);
  EXPECT_EQ(nftTransfers.at(getTestTokenId1()).size(), 1);
  const auto& nftTransfer = nftTransfers.at(getTestTokenId1()).at(0);
  EXPECT_EQ(nftTransfer.mNftId, getTestNftId1());
  EXPECT_EQ(nftTransfer.mSenderAccountId, getTestAccountId1());
  EXPECT_EQ(nftTransfer.mReceiverAccountId, getTestAccountId2());
}

//-----
TEST_F(TokenAirdropTransactionUnitTests, HandleTokenTransferUpdate)
{
  // Given
  TokenAirdropTransaction transaction;
  transaction.addTokenTransfer(getTestTokenId1(), getTestAccountId1(), getTestAmount());

  // When
  transaction.addTokenTransfer(getTestTokenId1(), getTestAccountId1(), 2000LL);

  // Then
  auto transfers = transaction.getTokenTransfers();
  EXPECT_EQ(transfers.size(), 1);
  EXPECT_EQ(transfers.at(getTestTokenId1()).at(getTestAccountId1()), getTestAmount() + 2000LL);
}

//-----
TEST_F(TokenAirdropTransactionUnitTests, HandleTokenTransferRemoval)
{
  // Given
  TokenAirdropTransaction transaction;
  transaction.addTokenTransfer(getTestTokenId1(), getTestAccountId1(), getTestAmount());
  transaction.addTokenTransfer(getTestTokenId1(), getTestAccountId1(), -getTestAmount());

  // When
  auto transfers = transaction.getTokenTransfers();

  // Then
  EXPECT_TRUE(transfers.empty());
}

//-----
TEST_F(TokenAirdropTransactionUnitTests, HandleNftTransferUpdate)
{
  // Given
  TokenAirdropTransaction transaction;
  transaction.addNftTransfer(getTestNftId1(), getTestAccountId1(), getTestAccountId2());

  // When
  transaction.addNftTransfer(getTestNftId2(), getTestAccountId2(), getTestAccountId1());

  // Then
  auto nftTransfers = transaction.getNftTransfers();
  EXPECT_EQ(nftTransfers.size(), 2);
  EXPECT_EQ(nftTransfers.at(getTestTokenId1()).size(), 1);
}
