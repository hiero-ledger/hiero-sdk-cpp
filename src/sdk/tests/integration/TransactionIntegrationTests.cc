// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "Defaults.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "PublicKey.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "impl/DurationConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPublicKey() const { return mPublicKey; }
  [[nodiscard]] inline const Hbar& getTestInitialBalance() const { return mInitialBalance; }
  [[nodiscard]] inline bool getTestReceiverSignatureRequired() const { return mReceiverSignatureRequired; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mAutoRenewPeriod;
  }
  [[nodiscard]] inline const std::string& getTestAccountMemo() const { return mAccountMemo; }
  [[nodiscard]] inline uint32_t getTestMaximumTokenAssociations() const { return mMaxTokenAssociations; }
  [[nodiscard]] inline bool getTestDeclineStakingReward() const { return mDeclineStakingReward; }
  [[nodiscard]] inline const EvmAddress& getTestEvmAddress() const { return mEvmAddress; }

private:
  const std::shared_ptr<PublicKey> mPublicKey = ED25519PrivateKey::generatePrivateKey()->getPublicKey();
  const Hbar mInitialBalance = Hbar(1000ULL, HbarUnit::TINYBAR());
  const bool mReceiverSignatureRequired = true;
  const std::chrono::system_clock::duration mAutoRenewPeriod = std::chrono::hours(3);
  const std::string mAccountMemo = "Test Account Memo";
  const uint32_t mMaxTokenAssociations = 3U;

  const bool mDeclineStakingReward = true;
  const EvmAddress mEvmAddress = EvmAddress::fromString("303132333435363738396162636465666768696a");
};

//-----
TEST_F(TransactionIntegrationTests, ExecuteTransactionRegenerateTransactionId)
{
  // Given
  AccountCreateTransaction accountCreateTransaction = AccountCreateTransaction()
                                                        .setKey(getTestPublicKey())
                                                        .setTransactionId(TransactionId::generate(AccountId(2ULL)))
                                                        .setValidTransactionDuration(std::chrono::seconds(30))
                                                        .freezeWith(&getTestClient());

  // When
  const TransactionResponse txResponse = accountCreateTransaction.execute(getTestClient());

  // Then
  TransactionReceipt txReceipt;
  AccountId accountId;
  AccountInfo accountInfo;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  ASSERT_NO_THROW(accountId = txReceipt.mAccountId.value());
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_FALSE(accountInfo.mIsDeleted);
  EXPECT_EQ(accountInfo.mKey->toBytes(), getTestPublicKey()->toBytes());
  EXPECT_EQ(accountInfo.mBalance, Hbar(0LL));
  EXPECT_EQ(accountInfo.mAutoRenewPeriod, DEFAULT_AUTO_RENEW_PERIOD);
  EXPECT_EQ(accountInfo.mProxyReceived, Hbar(0LL));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .execute(getTestClient()));
}
