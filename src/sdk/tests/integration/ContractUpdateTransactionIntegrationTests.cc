// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractFunctionParameters.h"
#include "ContractId.h"
#include "ContractInfo.h"
#include "ContractInfoQuery.h"
#include "ContractUpdateTransaction.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <chrono>
#include <gtest/gtest.h>

using namespace Hiero;

class ContractUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(ContractUpdateTransactionIntegrationTests, ExecuteContractUpdateTransaction)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::shared_ptr<PrivateKey> newAdminKey = ED25519PrivateKey::generatePrivateKey();
  const auto newAutoRenewPeriod = std::chrono::hours(2016);
  const std::string newMemo = "[e2e::ContractUpdateTransaction]";
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());
  ContractId contractId;
  EXPECT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setBytecodeFileId(fileId)
                      .setAdminKey(operatorKey->getPublicKey())
                      .setGas(1000000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ContractUpdateTransaction()
                                .setContractId(contractId)
                                .setAdminKey(newAdminKey->getPublicKey())
                                .setAutoRenewPeriod(newAutoRenewPeriod)
                                .setContractMemo(newMemo)
                                .setDeclineStakingReward(true)
                                .freezeWith(&getTestClient())
                                .sign(newAdminKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));

  ASSERT_NE(contractInfo.mAdminKey.get(), nullptr);
  EXPECT_EQ(contractInfo.mAdminKey->toBytes(), newAdminKey->getPublicKey()->toBytes());
  EXPECT_EQ(contractInfo.mAutoRenewPeriod, newAutoRenewPeriod);
  EXPECT_EQ(contractInfo.mMemo, newMemo);
  EXPECT_TRUE(contractInfo.mStakingInfo.mDeclineRewards);

  // Clean up
  ASSERT_NO_THROW(txReceipt = ContractDeleteTransaction()
                                .setContractId(contractId)
                                .setTransferAccountId(AccountId(2ULL))
                                .freezeWith(&getTestClient())
                                .sign(newAdminKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractUpdateTransactionIntegrationTests, CannotUpdateContractWithNoContractId)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 ContractUpdateTransaction().execute(getTestClient()).getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_CONTRACT_ID
}

//-----
TEST_F(ContractUpdateTransactionIntegrationTests, CannotModifyImmutableContract)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());
  ContractId contractId;
  EXPECT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setBytecodeFileId(fileId)
                      .setGas(1000000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = ContractUpdateTransaction()
                                                      .setContractId(contractId)
                                                      .setContractMemo("new memo")
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // MODIFYING_IMMUTABLE_CONTRACT

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}
