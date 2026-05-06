// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Hbar.h"
#include "HbarAllowance.h"

#include <gtest/gtest.h>
#include <services/crypto_approve_allowance.pb.h>

using namespace Hiero;

class HbarAllowanceUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestOwnerAccountId() const { return mOwnerAccountId; }
  [[nodiscard]] inline const AccountId& getTestSpenderAccountId() const { return mSpenderAccountId; }
  [[nodiscard]] inline const Hbar& getTestAmount() const { return mAmount; }

private:
  const AccountId mOwnerAccountId = AccountId(1ULL);
  const AccountId mSpenderAccountId = AccountId(2ULL);
  const Hbar mAmount = Hbar(3LL);
};

//-----
TEST_F(HbarAllowanceUnitTests, ConstructWithOwnerSpenderAmount)
{
  // Given / When
  const HbarAllowance hbarAllowance(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmount());

  // Then
  EXPECT_EQ(hbarAllowance.mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(hbarAllowance.mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(hbarAllowance.mAmount, getTestAmount());
}

//-----
TEST_F(HbarAllowanceUnitTests, FromProtobuf)
{
  // Given
  proto::CryptoAllowance cryptoAllowance;
  cryptoAllowance.set_allocated_owner(getTestOwnerAccountId().toProtobuf().release());
  cryptoAllowance.set_allocated_spender(getTestSpenderAccountId().toProtobuf().release());
  cryptoAllowance.set_amount(getTestAmount().toTinybars());

  // When
  const HbarAllowance hbarAllowance = HbarAllowance::fromProtobuf(cryptoAllowance);

  // Then
  EXPECT_EQ(hbarAllowance.mOwnerAccountId, getTestOwnerAccountId());
  EXPECT_EQ(hbarAllowance.mSpenderAccountId, getTestSpenderAccountId());
  EXPECT_EQ(hbarAllowance.mAmount, getTestAmount());
}

//-----
TEST_F(HbarAllowanceUnitTests, ToProtobuf)
{
  // Given
  const HbarAllowance hbarAllowance(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmount());

  // When
  const std::unique_ptr<proto::CryptoAllowance> cryptoAllowance = hbarAllowance.toProtobuf();

  // Then
  ASSERT_TRUE(cryptoAllowance->has_owner());
  EXPECT_EQ(AccountId::fromProtobuf(cryptoAllowance->owner()), getTestOwnerAccountId());
  ASSERT_TRUE(cryptoAllowance->has_spender());
  EXPECT_EQ(AccountId::fromProtobuf(cryptoAllowance->spender()), getTestSpenderAccountId());
  EXPECT_EQ(cryptoAllowance->amount(), getTestAmount().toTinybars());
}

//-----
TEST_F(HbarAllowanceUnitTests, EqualityDefaultConstructed)
{
  // Given / When
  const HbarAllowance lhs;
  const HbarAllowance rhs;

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(HbarAllowanceUnitTests, EqualityIdenticallyConstructed)
{
  // Given / When
  const HbarAllowance lhs(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmount());
  const HbarAllowance rhs(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmount());

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(HbarAllowanceUnitTests, InequalityDifferentOwner)
{
  // Given / When
  const HbarAllowance lhs(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmount());
  const HbarAllowance rhs(AccountId(99ULL), getTestSpenderAccountId(), getTestAmount());

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(HbarAllowanceUnitTests, InequalityDifferentSpender)
{
  // Given / When
  const HbarAllowance lhs(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmount());
  const HbarAllowance rhs(getTestOwnerAccountId(), AccountId(99ULL), getTestAmount());

  // Then
  EXPECT_FALSE(lhs == rhs);
}

//-----
TEST_F(HbarAllowanceUnitTests, InequalityDifferentAmount)
{
  // Given / When
  const HbarAllowance lhs(getTestOwnerAccountId(), getTestSpenderAccountId(), getTestAmount());
  const HbarAllowance rhs(getTestOwnerAccountId(), getTestSpenderAccountId(), Hbar(999LL));

  // Then
  EXPECT_FALSE(lhs == rhs);
}
