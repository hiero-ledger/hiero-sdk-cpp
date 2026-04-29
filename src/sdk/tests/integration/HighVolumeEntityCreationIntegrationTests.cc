// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class HighVolumeEntityCreationIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(HighVolumeEntityCreationIntegrationTests, CanCreateAccountWithHighVolume)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithoutAlias(accountKey)
                                 .setInitialBalance(Hbar(1))
                                 .setHighVolume(true)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  ASSERT_TRUE(txReceipt.mAccountId.has_value());

  TransactionRecord txRecord;
  ASSERT_NO_THROW(txRecord = txResponse.getRecord(getTestClient()));
  EXPECT_GE(txRecord.mHighVolumePricingMultiplier, 1000ULL);
}

//-----
TEST_F(HighVolumeEntityCreationIntegrationTests, CanCreateAccountWithHighVolumeAndCustomMaxFee)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithoutAlias(accountKey)
                                 .setInitialBalance(Hbar(1))
                                 .setHighVolume(true)
                                 .setMaxTransactionFee(Hbar(5))
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  ASSERT_TRUE(txReceipt.mAccountId.has_value());
}

//-----
TEST_F(HighVolumeEntityCreationIntegrationTests, DISABLED_FailsWithInsufficientFeeWhenHighVolumeAndLowMaxFee)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  // When / Then
  EXPECT_THROW(AccountCreateTransaction()
                 .setKeyWithoutAlias(accountKey)
                 .setInitialBalance(Hbar(1))
                 .setHighVolume(true)
                 .setMaxTransactionFee(Hbar(0, HbarUnit::TINYBAR()))
                 .execute(getTestClient()),
               PrecheckStatusException);
}

//-----
TEST_F(HighVolumeEntityCreationIntegrationTests, CanCreateAccountWithoutHighVolume)
{
  // Given
  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithoutAlias(accountKey)
                                 .setInitialBalance(Hbar(1))
                                 .setHighVolume(false)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  ASSERT_TRUE(txReceipt.mAccountId.has_value());

  TransactionRecord txRecord;
  ASSERT_NO_THROW(txRecord = txResponse.getRecord(getTestClient()));
  EXPECT_EQ(txRecord.mHighVolumePricingMultiplier, 0ULL);
}
