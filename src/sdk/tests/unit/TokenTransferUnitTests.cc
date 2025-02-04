// SPDX-License-Identifier: Apache-2.0
#include "TokenTransfer.h"

#include "AccountId.h"
#include "TokenId.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class TokenTransferUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTokenId; }
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline const int64_t& getTestAmount() const { return mAmount; }
  [[nodiscard]] inline const uint32_t& getTestExpectedDecimals() const { return mExpectedDecimals; }
  [[nodiscard]] inline bool getTestIsApproval() const { return mIsApproval; }

private:
  const TokenId mTokenId = TokenId(10ULL);
  const AccountId mAccountId = AccountId(200ULL);
  const int64_t mAmount = 3000LL;
  const uint32_t mExpectedDecimals = 40000U;
  const bool mIsApproval = true;
};

//-----
TEST_F(TokenTransferUnitTests, DefaultConstruction)
{
  // Given / When
  const TokenTransfer tokenTransfer;

  // Then
  EXPECT_EQ(tokenTransfer.mTokenId, TokenId());
  EXPECT_EQ(tokenTransfer.mAccountId, AccountId());
  EXPECT_EQ(tokenTransfer.mAmount, 0ULL);
  EXPECT_EQ(tokenTransfer.mExpectedDecimals, 0);
  EXPECT_FALSE(tokenTransfer.mIsApproval);
}

//-----
TEST_F(TokenTransferUnitTests, ConstructWithTokenIdAccountIdAmountAndApproval)
{
  // Given / When
  const TokenTransfer tokenTransfer(getTestTokenId(), getTestAccountId(), getTestAmount(), getTestIsApproval());

  // Then
  EXPECT_EQ(tokenTransfer.mTokenId, getTestTokenId());
  EXPECT_EQ(tokenTransfer.mAccountId, getTestAccountId());
  EXPECT_EQ(tokenTransfer.mAmount, getTestAmount());
  EXPECT_EQ(tokenTransfer.mExpectedDecimals, 0U);
  EXPECT_EQ(tokenTransfer.mIsApproval, getTestIsApproval());
}

//-----
TEST_F(TokenTransferUnitTests, ConstructWithTokenIdAccountIdAmountExpectedDecimalsAndApproval)
{
  // Given / When
  const TokenTransfer tokenTransfer(
    getTestTokenId(), getTestAccountId(), getTestAmount(), getTestExpectedDecimals(), getTestIsApproval());

  // Then
  EXPECT_EQ(tokenTransfer.mTokenId, getTestTokenId());
  EXPECT_EQ(tokenTransfer.mAccountId, getTestAccountId());
  EXPECT_EQ(tokenTransfer.mAmount, getTestAmount());
  EXPECT_EQ(tokenTransfer.mExpectedDecimals, getTestExpectedDecimals());
  EXPECT_EQ(tokenTransfer.mIsApproval, getTestIsApproval());
}

//-----
TEST_F(TokenTransferUnitTests, FromProtobuf)
{
  // Given
  proto::TokenTransferList tokenTransferList;
  tokenTransferList.set_allocated_token(getTestTokenId().toProtobuf().release());

  proto::AccountAmount* amount = tokenTransferList.add_transfers();
  amount->set_allocated_accountid(getTestAccountId().toProtobuf().release());
  amount->set_amount(getTestAmount());
  amount->set_is_approval(getTestIsApproval());

  auto decimalPtr = std::make_unique<google::protobuf::UInt32Value>();
  decimalPtr->set_value(getTestExpectedDecimals());
  tokenTransferList.set_allocated_expected_decimals(decimalPtr.release());

  // When
  const TokenTransfer tokenTransfer =
    TokenTransfer::fromProtobuf(*amount, getTestTokenId(), tokenTransferList.expected_decimals().value());

  // Then
  EXPECT_EQ(tokenTransfer.mTokenId, getTestTokenId());
  EXPECT_EQ(tokenTransfer.mAccountId, getTestAccountId());
  EXPECT_EQ(tokenTransfer.mAmount, getTestAmount());
  EXPECT_EQ(tokenTransfer.mExpectedDecimals, getTestExpectedDecimals());
  EXPECT_EQ(tokenTransfer.mIsApproval, getTestIsApproval());
}

//-----
TEST_F(TokenTransferUnitTests, ToProtobuf)
{
  // Given
  const TokenTransfer tokenTransfer(
    getTestTokenId(), getTestAccountId(), getTestAmount(), getTestExpectedDecimals(), getTestIsApproval());

  // When
  const std::unique_ptr<proto::AccountAmount> proto = tokenTransfer.toProtobuf();

  // Then
  EXPECT_EQ(proto->accountid().shardnum(), getTestAccountId().mShardNum);
  EXPECT_EQ(proto->accountid().realmnum(), getTestAccountId().mRealmNum);
  EXPECT_EQ(proto->accountid().accountnum(), getTestAccountId().mAccountNum);
  EXPECT_EQ(proto->amount(), getTestAmount());
  EXPECT_EQ(proto->is_approval(), getTestIsApproval());
}
