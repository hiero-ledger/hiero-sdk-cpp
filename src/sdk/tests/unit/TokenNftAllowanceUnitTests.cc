// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "TokenId.h"
#include "TokenNftAllowance.h"

#include <crypto_approve_allowance.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class TokenNftAllowanceUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTokenId; }
  [[nodiscard]] inline const AccountId& getTestOwnerAccountId() const { return mOwnerAccountId; }
  [[nodiscard]] inline const AccountId& getTestSpenderAccountId() const { return mSpenderAccountId; }
  [[nodiscard]] inline const std::vector<uint64_t>& getTestSerialNumbers() const { return mSerialNumbers; }
  [[nodiscard]] inline const AccountId& getTestDelegatingSpenderAccountId() const
  {
    return mDelegatingSpenderAccountId;
  }

private:
  const TokenId mTokenId = TokenId(1ULL);
  const AccountId mOwnerAccountId = AccountId(2ULL);
  const AccountId mSpenderAccountId = AccountId(3ULL);
  const std::vector<uint64_t> mSerialNumbers = { 5ULL, 6ULL };
  const AccountId mDelegatingSpenderAccountId = AccountId(7ULL);
};

//-----
TEST_F(TokenNftAllowanceUnitTests, ConstructWithTokenIdOwnerSpenderSerialNumbersApproval)
{
  // Given / When
  const TokenNftAllowance tokenNftAllowance(
    getTestTokenId(), getTestOwnerAccountId(), getTestSpenderAccountId(), getTestSerialNumbers());

  // Then
  EXPECT_EQ(tokenNftAllowance.mTokenId, getTestTokenId());
  EXPECT_EQ(tokenNftAllowance.mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(tokenNftAllowance.mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(tokenNftAllowance.mSerialNumbers, getTestSerialNumbers());
  EXPECT_FALSE(tokenNftAllowance.mApprovedForAll.has_value());
  EXPECT_FALSE(tokenNftAllowance.mDelegatingSpenderAccountId.has_value());
}

//-----
TEST_F(TokenNftAllowanceUnitTests, FromProtobuf)
{
  // Given
  proto::NftAllowance protoNftAllowance;
  protoNftAllowance.set_allocated_tokenid(getTestTokenId().toProtobuf().release());
  protoNftAllowance.set_allocated_owner(getTestOwnerAccountId().toProtobuf().release());
  protoNftAllowance.set_allocated_spender(getTestSpenderAccountId().toProtobuf().release());
  protoNftAllowance.set_allocated_delegating_spender(getTestDelegatingSpenderAccountId().toProtobuf().release());

  for (const uint64_t& num : getTestSerialNumbers())
  {
    protoNftAllowance.add_serial_numbers(static_cast<int64_t>(num));
  }

  auto boolValue = std::make_unique<google::protobuf::BoolValue>();
  boolValue->set_value(false);
  protoNftAllowance.set_allocated_approved_for_all(boolValue.release());

  // When
  const TokenNftAllowance tokenNftAllowance = TokenNftAllowance::fromProtobuf(protoNftAllowance);

  // Then
  EXPECT_EQ(tokenNftAllowance.mTokenId, getTestTokenId());
  EXPECT_EQ(tokenNftAllowance.mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(tokenNftAllowance.mSpenderAccountId, getTestSpenderAccountId());
  ASSERT_TRUE(tokenNftAllowance.mApprovedForAll.has_value());
  EXPECT_FALSE(*tokenNftAllowance.mApprovedForAll);

  ASSERT_EQ(tokenNftAllowance.mSerialNumbers.size(), getTestSerialNumbers().size());
  for (int i = 0; i < getTestSerialNumbers().size(); ++i)
  {
    EXPECT_EQ(tokenNftAllowance.mSerialNumbers.at(i), getTestSerialNumbers().at(i));
  }

  ASSERT_TRUE(tokenNftAllowance.mDelegatingSpenderAccountId.has_value());
  EXPECT_EQ(*tokenNftAllowance.mDelegatingSpenderAccountId, getTestDelegatingSpenderAccountId());
}

//-----
TEST_F(TokenNftAllowanceUnitTests, ToProtobuf)
{
  // Given
  const TokenNftAllowance tokenNftAllowance(getTestTokenId(),
                                            getTestOwnerAccountId(),
                                            getTestSpenderAccountId(),
                                            getTestSerialNumbers(),
                                            false,
                                            getTestDelegatingSpenderAccountId());

  // When
  const std::unique_ptr<proto::NftAllowance> protoNftAllowance = tokenNftAllowance.toProtobuf();

  // Then
  ASSERT_TRUE(protoNftAllowance->has_tokenid());
  EXPECT_EQ(TokenId::fromProtobuf(protoNftAllowance->tokenid()), getTestTokenId());
  ASSERT_TRUE(protoNftAllowance->has_owner());
  EXPECT_EQ(AccountId::fromProtobuf(protoNftAllowance->owner()), getTestOwnerAccountId());
  ASSERT_TRUE(protoNftAllowance->has_spender());
  EXPECT_EQ(AccountId::fromProtobuf(protoNftAllowance->spender()), getTestSpenderAccountId());

  ASSERT_EQ(tokenNftAllowance.mSerialNumbers.size(), getTestSerialNumbers().size());
  for (int i = 0; i < getTestSerialNumbers().size(); ++i)
  {
    EXPECT_EQ(tokenNftAllowance.mSerialNumbers.at(i), getTestSerialNumbers().at(i));
  }

  ASSERT_TRUE(tokenNftAllowance.mApprovedForAll.has_value());
  EXPECT_FALSE(*tokenNftAllowance.mApprovedForAll);

  ASSERT_TRUE(tokenNftAllowance.mDelegatingSpenderAccountId.has_value());
  EXPECT_EQ(tokenNftAllowance.mDelegatingSpenderAccountId.value(), getTestDelegatingSpenderAccountId());
}
