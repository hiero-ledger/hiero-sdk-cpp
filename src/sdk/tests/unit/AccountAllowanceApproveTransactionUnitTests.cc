// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "Hbar.h"
#include "NftId.h"
#include "TokenId.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class AccountAllowanceApproveTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestOwnerAccountId() const { return mOwnerAccountId; }
  [[nodiscard]] inline const AccountId& getTestSpenderAccountId() const { return mSpenderAccountId; }
  [[nodiscard]] inline const Hbar& getTestAmountHbar() const { return mAmountHbar; }
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTokenId; }
  [[nodiscard]] inline const std::vector<uint64_t>& getTestSerialNumbers() const { return mSerialNumbers; }
  [[nodiscard]] inline bool getTestApprovalForAll() const { return mApprovalForAll; }
  [[nodiscard]] inline const AccountId& getTestDelegatingSpenderAccountId() const
  {
    return mDelegatingSpenderAccountId;
  }

private:
  const AccountId mOwnerAccountId = AccountId(2ULL);
  const AccountId mSpenderAccountId = AccountId(3ULL);
  const Hbar mAmountHbar = Hbar(4LL);
  const TokenId mTokenId = TokenId(5ULL);
  const std::vector<uint64_t> mSerialNumbers = { 6ULL, 7ULL, 8ULL };
  const bool mApprovalForAll = true;
  const AccountId mDelegatingSpenderAccountId = AccountId(9ULL);
};

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ConstructAccountAllowanceApproveTransaction)
{
  // Given / When
  AccountAllowanceApproveTransaction transaction;

  // Then
  EXPECT_TRUE(transaction.getHbarApprovals().empty());
  EXPECT_TRUE(transaction.getTokenApprovals().empty());
  EXPECT_TRUE(transaction.getNftApprovals().empty());
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests,
       ConstructAccountAllowanceApproveTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::CryptoApproveAllowanceTransactionBody>();
  proto::CryptoAllowance* cryptoAllowance = body->add_cryptoallowances();
  cryptoAllowance->set_allocated_owner(getTestOwnerAccountId().toProtobuf().release());
  cryptoAllowance->set_allocated_spender(getTestSpenderAccountId().toProtobuf().release());
  cryptoAllowance->set_amount(getTestAmountHbar().toTinybars());

  proto::TokenAllowance* tokenAllowance = body->add_tokenallowances();
  tokenAllowance->set_allocated_tokenid(getTestTokenId().toProtobuf().release());
  tokenAllowance->set_allocated_owner(getTestOwnerAccountId().toProtobuf().release());
  tokenAllowance->set_allocated_spender(getTestSpenderAccountId().toProtobuf().release());
  tokenAllowance->set_amount(getTestAmountHbar().toTinybars());

  proto::NftAllowance* nftAllowance = body->add_nftallowances();
  nftAllowance->set_allocated_tokenid(getTestTokenId().toProtobuf().release());
  nftAllowance->set_allocated_owner(getTestOwnerAccountId().toProtobuf().release());
  nftAllowance->set_allocated_spender(getTestSpenderAccountId().toProtobuf().release());

  for (const uint64_t& num : getTestSerialNumbers())
  {
    nftAllowance->add_serial_numbers(static_cast<int64_t>(num));
  }

  nftAllowance->mutable_approved_for_all()->set_value(getTestApprovalForAll());
  nftAllowance->set_allocated_delegating_spender(getTestDelegatingSpenderAccountId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoapproveallowance(body.release());

  // When
  const AccountAllowanceApproveTransaction accountAllowanceApproveTransaction(txBody);

  // Then
  ASSERT_EQ(accountAllowanceApproveTransaction.getHbarApprovals().size(), 1);
  EXPECT_EQ(accountAllowanceApproveTransaction.getHbarApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getHbarApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getHbarApprovals().at(0).mAmount, getTestAmountHbar());

  ASSERT_EQ(accountAllowanceApproveTransaction.getTokenApprovals().size(), 1);
  EXPECT_EQ(accountAllowanceApproveTransaction.getTokenApprovals().at(0).mTokenId, getTestTokenId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getTokenApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getTokenApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getTokenApprovals().at(0).mAmount, getTestAmountHbar().toTinybars());

  ASSERT_EQ(accountAllowanceApproveTransaction.getNftApprovals().size(), 1);
  EXPECT_EQ(accountAllowanceApproveTransaction.getNftApprovals().at(0).mTokenId, getTestTokenId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getNftApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getNftApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(accountAllowanceApproveTransaction.getNftApprovals().at(0).mSerialNumbers, getTestSerialNumbers());
  ASSERT_TRUE(accountAllowanceApproveTransaction.getNftApprovals().at(0).mApprovedForAll.has_value());
  EXPECT_EQ(*accountAllowanceApproveTransaction.getNftApprovals().at(0).mApprovedForAll, getTestApprovalForAll());
  EXPECT_EQ(*accountAllowanceApproveTransaction.getNftApprovals().at(0).mDelegatingSpenderAccountId,
            getTestDelegatingSpenderAccountId());
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveHbarAllowance)
{
  // Given
  AccountAllowanceApproveTransaction transaction;

  // When
  EXPECT_NO_THROW(
    transaction.approveHbarAllowance(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmountHbar()));

  // Then
  ASSERT_EQ(transaction.getHbarApprovals().size(), 1);
  EXPECT_EQ(transaction.getHbarApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(transaction.getHbarApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(transaction.getHbarApprovals().at(0).mAmount, getTestAmountHbar());
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveHbarAllowanceFrozen)
{
  // Given
  AccountAllowanceApproveTransaction transaction = AccountAllowanceApproveTransaction()
                                                     .setNodeAccountIds({ AccountId(1ULL) })
                                                     .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(
    transaction.approveHbarAllowance(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmountHbar()),
    IllegalStateException);
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveTokenAllowance)
{
  // Given
  AccountAllowanceApproveTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.approveTokenAllowance(
    getTestTokenId(), getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmountHbar().toTinybars()));

  // Then
  ASSERT_EQ(transaction.getTokenApprovals().size(), 1);
  EXPECT_EQ(transaction.getTokenApprovals().at(0).mTokenId, getTestTokenId());
  EXPECT_EQ(transaction.getTokenApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(transaction.getTokenApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(transaction.getTokenApprovals().at(0).mAmount, getTestAmountHbar().toTinybars());
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveTokenAllowanceFrozen)
{
  // Given
  AccountAllowanceApproveTransaction transaction = AccountAllowanceApproveTransaction()
                                                     .setNodeAccountIds({ AccountId(1ULL) })
                                                     .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(
    transaction.approveTokenAllowance(
      getTestTokenId(), getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmountHbar().toTinybars()),
    IllegalStateException);
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveNftAllowance)
{
  // Given
  AccountAllowanceApproveTransaction transaction;
  const TokenId tokenId(20ULL);
  const uint64_t serialNum = 30ULL;

  // When
  for (const uint64_t& num : getTestSerialNumbers())
  {
    EXPECT_NO_THROW(transaction.approveTokenNftAllowance(
      NftId(getTestTokenId(), num), getTestOwnerAccountId(), getTestSpenderAccountId()));
  }

  EXPECT_NO_THROW(transaction.approveTokenNftAllowance(
    NftId(tokenId, serialNum), getTestOwnerAccountId(), getTestSpenderAccountId()));

  // Then
  ASSERT_EQ(transaction.getNftApprovals().size(), 2);

  EXPECT_EQ(transaction.getNftApprovals().at(0).mTokenId, getTestTokenId());
  EXPECT_EQ(transaction.getNftApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(transaction.getNftApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  ASSERT_EQ(transaction.getNftApprovals().at(0).mSerialNumbers.size(), getTestSerialNumbers().size());
  for (int i = 0; i < getTestSerialNumbers().size(); ++i)
  {
    EXPECT_EQ(transaction.getNftApprovals().at(0).mSerialNumbers.at(i), getTestSerialNumbers().at(i));
  }

  EXPECT_EQ(transaction.getNftApprovals().at(1).mTokenId, tokenId);
  EXPECT_EQ(transaction.getNftApprovals().at(1).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(transaction.getNftApprovals().at(1).mSpenderAccountId, getTestSpenderAccountId());
  ASSERT_EQ(transaction.getNftApprovals().at(1).mSerialNumbers.size(), 1);
  EXPECT_EQ(transaction.getNftApprovals().at(1).mSerialNumbers.at(0), serialNum);
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveNftAllowanceFrozen)
{
  // Given
  AccountAllowanceApproveTransaction transaction = AccountAllowanceApproveTransaction()
                                                     .setNodeAccountIds({ AccountId(1ULL) })
                                                     .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.approveTokenNftAllowance(
                 NftId(getTestTokenId(), 1ULL), getTestOwnerAccountId(), getTestSpenderAccountId()),
               IllegalStateException);
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveNftAllowanceAllSerials)
{
  // Given
  AccountAllowanceApproveTransaction transaction;

  // When
  EXPECT_NO_THROW(
    transaction.approveNftAllowanceAllSerials(getTestTokenId(), getTestOwnerAccountId(), getTestSpenderAccountId()));

  // Then
  ASSERT_EQ(transaction.getNftApprovals().size(), 1);
  EXPECT_EQ(transaction.getNftApprovals().at(0).mTokenId, getTestTokenId());
  EXPECT_EQ(transaction.getNftApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(transaction.getNftApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  ASSERT_TRUE(transaction.getNftApprovals().at(0).mSerialNumbers.empty());
  ASSERT_TRUE(transaction.getNftApprovals().at(0).mApprovedForAll.has_value());
  EXPECT_TRUE(*transaction.getNftApprovals().at(0).mApprovedForAll);
  EXPECT_FALSE(transaction.getNftApprovals().at(0).mDelegatingSpenderAccountId);
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, ApproveNftAllowanceAllSerialsFrozen)
{
  // Given
  AccountAllowanceApproveTransaction transaction = AccountAllowanceApproveTransaction()
                                                     .setNodeAccountIds({ AccountId(1ULL) })
                                                     .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(
    transaction.approveNftAllowanceAllSerials(getTestTokenId(), getTestOwnerAccountId(), getTestSpenderAccountId()),
    IllegalStateException);
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, DeleteNftAllowanceAllSerials)
{
  // Given
  AccountAllowanceApproveTransaction transaction;

  // When
  EXPECT_NO_THROW(
    transaction.deleteNftAllowanceAllSerials(getTestTokenId(), getTestOwnerAccountId(), getTestSpenderAccountId()));

  // Then
  ASSERT_EQ(transaction.getNftApprovals().size(), 1);
  EXPECT_EQ(transaction.getNftApprovals().at(0).mTokenId, getTestTokenId());
  EXPECT_EQ(transaction.getNftApprovals().at(0).mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(transaction.getNftApprovals().at(0).mSpenderAccountId, getTestSpenderAccountId());
  ASSERT_TRUE(transaction.getNftApprovals().at(0).mSerialNumbers.empty());
  ASSERT_TRUE(transaction.getNftApprovals().at(0).mApprovedForAll.has_value());
  EXPECT_FALSE(*transaction.getNftApprovals().at(0).mApprovedForAll);
  EXPECT_FALSE(transaction.getNftApprovals().at(0).mDelegatingSpenderAccountId);
}

//-----
TEST_F(AccountAllowanceApproveTransactionUnitTests, DeleteNftAllowanceAllSerialsFrozen)
{
  // Given
  AccountAllowanceApproveTransaction transaction = AccountAllowanceApproveTransaction()
                                                     .setNodeAccountIds({ AccountId(1ULL) })
                                                     .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(
    transaction.deleteNftAllowanceAllSerials(getTestTokenId(), getTestOwnerAccountId(), getTestSpenderAccountId()),
    IllegalStateException);
}
