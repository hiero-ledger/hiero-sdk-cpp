// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "ContractCallQuery.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractFunctionParameters.h"
#include "ContractFunctionResult.h"
#include "ContractId.h"
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

class ContractExecuteTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(ContractExecuteTransactionIntegrationTests, ExecuteContractExecuteTransaction)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string newMessage = "new message";
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());
  ContractId contractId;
  ASSERT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setBytecodeFileId(fileId)
                      .setAdminKey(operatorKey->getPublicKey())
                      .setGas(1000000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ContractExecuteTransaction()
                                .setContractId(contractId)
                                .setGas(1000000ULL)
                                .setFunction("setMessage", ContractFunctionParameters().addString(newMessage))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  EXPECT_EQ(ContractCallQuery()
              .setContractId(contractId)
              .setGas(1000000ULL)
              .setFunction("getMessage")
              .execute(getTestClient())
              .getString(0),
            newMessage);

  // Clean up
  ASSERT_NO_THROW(txReceipt = ContractDeleteTransaction()
                                .setContractId(contractId)
                                .setTransferAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
// Disabled due to client executing with privileged account
// this bypasses the Precheck in the service code so changing
// operator for certain tests will be a separate issue
TEST_F(ContractExecuteTransactionIntegrationTests, DISABLED_CannotExecuteContractWithoutContractId)
{
  // Given / When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 ContractExecuteTransaction()
                   .setGas(100000ULL)
                   .setFunction("setMessage", ContractFunctionParameters().addString("new message"))
                   .execute(getTestClient())
                   .getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_CONTRACT_ID
}

//-----
TEST_F(ContractExecuteTransactionIntegrationTests, CannotExecuteContractWithNoFunctionParameters)
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
  ASSERT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setBytecodeFileId(fileId)
                      .setAdminKey(operatorKey->getPublicKey())
                      .setGas(1000000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = ContractExecuteTransaction()
                                                      .setContractId(contractId)
                                                      .setGas(1000000ULL)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // CONTRACT_REVERT_EXECUTED

  // Clean up
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = ContractDeleteTransaction()
                                .setContractId(contractId)
                                .setTransferAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractExecuteTransactionIntegrationTests, CannotExecuteContractWithNoGas)
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
  ASSERT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setBytecodeFileId(fileId)
                      .setAdminKey(operatorKey->getPublicKey())
                      .setGas(1000000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 ContractExecuteTransaction()
                   .setContractId(contractId)
                   .setFunction("setMessage", ContractFunctionParameters().addString("new message"))
                   .execute(getTestClient())
                   .getReceipt(getTestClient()),
               PrecheckStatusException); // INSUFFICIENT_GAS

  // Clean up
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = ContractDeleteTransaction()
                                .setContractId(contractId)
                                .setTransferAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}
