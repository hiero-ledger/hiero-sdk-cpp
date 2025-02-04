// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TransactionReceiptIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(TransactionReceiptIntegrationTests, ExecuteAccountCreateTransactionAndCheckTransactionReceipt)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");

  // When
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = AccountCreateTransaction()
                                .setKey(operatorKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  EXPECT_NO_THROW(txReceipt.validateStatus());
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_TRUE(txReceipt.mAccountId.has_value());
  EXPECT_FALSE(txReceipt.mFileId.has_value());
  EXPECT_FALSE(txReceipt.mContractId.has_value());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(txReceipt.mAccountId.value())
                    .setTransferAccountId(AccountId(2ULL))
                    .execute(getTestClient()));
}

//-----
TEST_F(TransactionReceiptIntegrationTests, ExecuteFileCreateTransactionAndCheckTransactionReceipt)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");

  // When
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = FileCreateTransaction()
                                .setKeys({ operatorKey->getPublicKey() })
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_TRUE(txReceipt.mFileId.has_value());
  EXPECT_FALSE(txReceipt.mAccountId.has_value());
  EXPECT_FALSE(txReceipt.mContractId.has_value());

  // Clean up
  ASSERT_NO_THROW(txReceipt = FileDeleteTransaction()
                                .setFileId(txReceipt.mFileId.value())
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}

//-----
TEST_F(TransactionReceiptIntegrationTests, ExecuteContractCreateTransactionAndCheckTransactionReceipt)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");

  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(getTestFileContent())
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = ContractCreateTransaction()
                                .setGas(500000ULL)
                                .setBytecodeFileId(fileId)
                                .setAdminKey(operatorKey->getPublicKey())
                                .setMaxTransactionFee(Hbar(16LL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_TRUE(txReceipt.mContractId.has_value());
  EXPECT_FALSE(txReceipt.mAccountId.has_value());
  EXPECT_FALSE(txReceipt.mFileId.has_value());

  // Clean up
  ASSERT_NO_THROW(txReceipt = ContractDeleteTransaction()
                                .setContractId(txReceipt.mContractId.value())
                                .setTransferAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TransactionReceiptIntegrationTests, ExecuteTokenCreateTransactionAndCheckTransactionReceipt)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = TokenCreateTransaction()
                                 .setTokenName("test token name")
                                 .setTokenSymbol("test token symbol")
                                 .setAdminKey(operatorKey)
                                 .setTreasuryAccountId(AccountId(2ULL))
                                 .setAdminKey(operatorKey)
                                 .execute(getTestClient()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt =
                    TransactionReceiptQuery().setTransactionId(txResponse.mTransactionId).execute(getTestClient()));

  // Then
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_FALSE(txReceipt.mAccountId.has_value());
  EXPECT_FALSE(txReceipt.mContractId.has_value());
  EXPECT_FALSE(txReceipt.mFileId.has_value());
  EXPECT_TRUE(txReceipt.mTokenId.has_value());

  // Clean up
  ASSERT_NO_THROW(txReceipt = TokenDeleteTransaction()
                                .setTokenId(txReceipt.mTokenId.value())
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
}