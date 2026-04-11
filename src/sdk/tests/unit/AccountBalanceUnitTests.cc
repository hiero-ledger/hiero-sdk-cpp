// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"

#include <gtest/gtest.h>
#include <services/crypto_get_account_balance.pb.h>

using namespace Hiero;

class AccountBalanceUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const Hbar& getTestBalance() const { return mBalance; }

private:
  const Hbar mBalance = Hbar(100LL);
};

//-----
TEST_F(AccountBalanceUnitTests, DeserializeAccountBalanceFromProtobuf)
{
  // Given
  proto::CryptoGetAccountBalanceResponse testProtoAccountBalance;
  testProtoAccountBalance.set_balance(static_cast<unsigned long long>(getTestBalance().toTinybars()));

  // When
  const AccountBalance accountBalance = AccountBalance::fromProtobuf(testProtoAccountBalance);

  // Then
  EXPECT_EQ(accountBalance.mBalance, getTestBalance());
}

//-----
TEST_F(AccountBalanceUnitTests, ToString)
{
  // Given
  AccountBalance accountBalance;
  accountBalance.mBalance = getTestBalance();

  // When
  const std::string result = accountBalance.toString();

  // Then
  EXPECT_FALSE(result.empty());
  EXPECT_NE(result.find(getTestBalance().toString()), std::string::npos);
}

//-----
TEST_F(AccountBalanceUnitTests, DefaultEqualityOperator)
{
  // Given
  AccountBalance accountBalance1;
  AccountBalance accountBalance2;

  // When
  const bool areEqual = (accountBalance1 == accountBalance2);

  // Then
  EXPECT_TRUE(areEqual);
}

//-----
TEST_F(AccountBalanceUnitTests, EqualityOperatorWithSameBalance)
{
  // Given
  AccountBalance accountBalance1;
  accountBalance1.mBalance = getTestBalance();

  AccountBalance accountBalance2;
  accountBalance2.mBalance = getTestBalance();

  // When
  const bool areEqual = (accountBalance1 == accountBalance2);

  // Then
  EXPECT_TRUE(areEqual);
}

//-----
TEST_F(AccountBalanceUnitTests, EqualityOperatorWithDifferentBalance)
{
  // Given
  AccountBalance accountBalance1;
  accountBalance1.mBalance = getTestBalance();

  AccountBalance accountBalance2;
  accountBalance2.mBalance = Hbar(200LL);

  // When
  const bool areEqual = (accountBalance1 == accountBalance2);

  // Then
  EXPECT_FALSE(areEqual);
}

//-----
TEST_F(AccountBalanceUnitTests, EqualityOperatorWithSameTokens)
{
  // Given
  TokenId tokenId(0, 0, 1);

  AccountBalance accountBalance1;
  accountBalance1.mBalance = getTestBalance();
  accountBalance1.mTokens[tokenId] = 50;

  AccountBalance accountBalance2;
  accountBalance2.mBalance = getTestBalance();
  accountBalance2.mTokens[tokenId] = 50;

  // When
  const bool areEqual = (accountBalance1 == accountBalance2);

  // Then
  EXPECT_TRUE(areEqual);
}

//-----
TEST_F(AccountBalanceUnitTests, EqualityOperatorWithDifferentTokens)
{
  // Given
  TokenId tokenId(0, 0, 1);

  AccountBalance accountBalance1;
  accountBalance1.mBalance = getTestBalance();
  accountBalance1.mTokens[tokenId] = 50;

  AccountBalance accountBalance2;
  accountBalance2.mBalance = getTestBalance();
  accountBalance2.mTokens[tokenId] = 100;

  // When
  const bool areEqual = (accountBalance1 == accountBalance2);

  // Then
  EXPECT_FALSE(areEqual);
}
