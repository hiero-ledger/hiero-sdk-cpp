// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"

#include <crypto_get_account_balance.pb.h>
#include <gtest/gtest.h>

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