// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractFunctionParameters.h"
#include "ContractId.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "SystemDeleteTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class SystemDeleteTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(SystemDeleteTransactionIntegrationTests, DeleteFile)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey })
                             .setContents(getTestFileContent())
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = SystemDeleteTransaction()
                                                         .setFileId(fileId)
                                                         .setExpirationTime(std::chrono::system_clock::now())
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
}

//-----
TEST_F(SystemDeleteTransactionIntegrationTests, DeleteContract)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey })
                             .setContents(getTestFileContent())
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  ContractId contractId;
  ASSERT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setAdminKey(operatorKey)
                      .setGas(100000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setBytecodeFileId(fileId)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = SystemDeleteTransaction()
                                                      .setContractId(contractId)
                                                      .setExpirationTime(std::chrono::system_clock::now())
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               PrecheckStatusException); // NOT_SUPPORTED

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = ContractDeleteTransaction()
                                                         .setContractId(contractId)
                                                         .setTransferAccountId(AccountId(2ULL))
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));
}
