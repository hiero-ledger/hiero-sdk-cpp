// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "Defaults.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenGrantKycTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountInfoQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(AccountInfoQueryIntegrationTests, ExecuteAccountInfoQuery)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const Hbar balance(2LL);
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .setInitialBalance(balance)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  // Then
  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_FALSE(accountInfo.mIsDeleted);
  EXPECT_EQ(accountInfo.mKey->toBytes(), privateKey->getPublicKey()->toBytes());
  EXPECT_EQ(accountInfo.mBalance, balance);
  EXPECT_EQ(accountInfo.mAutoRenewPeriod, DEFAULT_AUTO_RENEW_PERIOD);
  EXPECT_EQ(accountInfo.mProxyReceived, Hbar(0LL));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountInfoQueryIntegrationTests, NoAccountId)
{
  // Given / When / Then
  EXPECT_THROW(AccountInfoQuery().execute(getTestClient()), PrecheckStatusException); // INVALID_ACCOUNT_ID
}

//-----
TEST_F(AccountInfoQueryIntegrationTests, CannotQueryDeletedAccount)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountDeleteTransaction()
                                                         .setDeleteAccountId(accountId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // When / Then
  EXPECT_THROW(AccountInfoQuery().setAccountId(accountId).execute(getTestClient()),
               PrecheckStatusException); // ACCOUNT_DELETED
}

//-----
TEST_F(AccountInfoQueryIntegrationTests, GetTokenRelationshipsAfterTokenTransfer)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKey(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  uint64_t expectedBalance = 10;
  uint32_t expectedDecimals = 0;
  std::optional<bool> expectedKycStatus = true;
  std::optional<bool> expectedFreezeStatus = false;
  bool expectedAutomaticAssociation = false;

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenCreateTransaction()
                                .setTokenName("ffff")
                                .setTokenSymbol("F")
                                .setDecimals(0)
                                .setInitialSupply(100000)
                                .setTreasuryAccountId(AccountId(2ULL))
                                .setAdminKey(getTestClient().getOperatorPublicKey())
                                .setFreezeKey(getTestClient().getOperatorPublicKey())
                                .setWipeKey(getTestClient().getOperatorPublicKey())
                                .setKycKey(getTestClient().getOperatorPublicKey())
                                .setSupplyKey(getTestClient().getOperatorPublicKey())
                                .setFeeScheduleKey(getTestClient().getOperatorPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  EXPECT_NO_THROW(txReceipt = TokenAssociateTransaction()
                                .setAccountId(accountId)
                                .setTokenIds({ tokenId })
                                .freezeWith(&getTestClient())
                                .sign(privateKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  EXPECT_NO_THROW(txReceipt = TokenGrantKycTransaction()
                                .setAccountId(accountId)
                                .setTokenId(tokenId)
                                .freezeWith(&getTestClient())
                                .sign(privateKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  EXPECT_NO_THROW(txReceipt = TransferTransaction()
                                .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -10LL)
                                .addTokenTransfer(tokenId, accountId, 10LL)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));
  EXPECT_EQ(accountInfo.mTokenRelationships.empty(), false);
  auto entry = *(accountInfo.mTokenRelationships.begin());
  EXPECT_EQ(entry.first.toString(), tokenId.toString());
  auto relationship = entry.second;
  EXPECT_EQ(relationship.mBalance, expectedBalance);
  EXPECT_EQ(relationship.mDecimals, expectedDecimals);
  EXPECT_EQ(relationship.mKycStatus, expectedKycStatus);
  EXPECT_EQ(relationship.mFreezeStatus, expectedFreezeStatus);
  EXPECT_EQ(relationship.mAutomaticAssociation, expectedAutomaticAssociation);
}
