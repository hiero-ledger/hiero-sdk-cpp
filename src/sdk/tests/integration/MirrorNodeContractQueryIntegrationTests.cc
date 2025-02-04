// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "ContractCallQuery.h"
#include "ContractCreateTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractFunctionParameters.h"
#include "ContractFunctionResult.h"
#include "ContractId.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileId.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include "impl/HexConverter.h"
#include "impl/MirrorNodeContractCallQuery.h"
#include "impl/MirrorNodeContractEstimateGasQuery.h"
#include "impl/Utilities.h"

#include "exceptions/IllegalStateException.h"

#include <chrono>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <thread>

using json = nlohmann::json;
using namespace Hiero;

class MirrorNodeContractQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(MirrorNodeContractQueryIntegrationTests, CanEstimateAndCall)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  ContractId contractId;
  EXPECT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setBytecodeFileId(fileId)
                      .setAdminKey(operatorKey->getPublicKey())
                      .setGas(1000000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // Wait for mirror node block
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Then
  std::string response;
  MirrorNodeContractEstimateGasQuery estimateGasQuery = MirrorNodeContractEstimateGasQuery();
  std::optional<ContractFunctionParameters> opt = std::nullopt;
  EXPECT_NO_THROW(response = estimateGasQuery.setContractId(contractId)
                               .setSender(getTestClient().getOperatorAccountId().value())
                               .setGasLimit(50000)
                               .setGasPrice(1234)
                               .setFunction("getMessage", opt)
                               .execute(getTestClient()));

  uint64_t estimatedGas = std::stoi(response, nullptr, 16);

  ContractFunctionResult contractFunctionResult;
  EXPECT_NO_THROW(contractFunctionResult = ContractCallQuery()
                                             .setContractId(contractId)
                                             .setGas(estimatedGas)
                                             .setQueryPayment(Hbar(1LL))
                                             .setFunction("getMessage")
                                             .execute(getTestClient()));

  std::string callResult;
  MirrorNodeContractCallQuery callQuery = MirrorNodeContractCallQuery();
  EXPECT_NO_THROW(callResult =
                    callQuery.setContractId(contractId).setFunction("getMessage", opt).execute(getTestClient()));

  // Mirror node returns the call result in lower case
  std::transform(
    callResult.begin(), callResult.end(), callResult.begin(), [](unsigned char c) { return std::toupper(c); });

  ASSERT_EQ(internal::HexConverter::bytesToHex(contractFunctionResult.mContractCallResult), callResult);

  EXPECT_NO_THROW(ContractExecuteTransaction()
                    .setContractId(contractId)
                    .setGas(estimatedGas)
                    .setFunction("getMessage")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
}

//-----
TEST_F(MirrorNodeContractQueryIntegrationTests, ReturnsDefaultGasWhenContractIsNotDeployed)
{
  // Given
  ContractId contractId(0, 0, 999999);

  // When / Then
  std::string response;
  MirrorNodeContractEstimateGasQuery estimateGasQuery = MirrorNodeContractEstimateGasQuery();
  std::optional<ContractFunctionParameters> opt = std::nullopt;
  EXPECT_THROW(response =
                 estimateGasQuery.setContractId(contractId).setFunction("getMessage", opt).execute(getTestClient()),
               IllegalStateException); // This would mean that the contract has no corresponding address.
}

//-----
TEST_F(MirrorNodeContractQueryIntegrationTests, FailWhenGasLimitIsLow)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  FileId fileId;
  ASSERT_NO_THROW(fileId = FileCreateTransaction()
                             .setKeys({ operatorKey->getPublicKey() })
                             .setContents(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                             .execute(getTestClient())
                             .getReceipt(getTestClient())
                             .mFileId.value());

  // When
  ContractId contractId;
  EXPECT_NO_THROW(contractId =
                    ContractCreateTransaction()
                      .setBytecodeFileId(fileId)
                      .setAdminKey(operatorKey->getPublicKey())
                      .setGas(1000000ULL)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // Wait for mirror node block
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Then
  std::string response;
  MirrorNodeContractEstimateGasQuery estimateGasQuery = MirrorNodeContractEstimateGasQuery();
  std::optional<ContractFunctionParameters> opt = std::nullopt;
  EXPECT_THROW(response = estimateGasQuery.setContractId(contractId)
                            .setSender(getTestClient().getOperatorAccountId().value())
                            .setGasLimit(100)
                            .setGasPrice(1234)
                            .setFunction("getMessage", opt)
                            .execute(getTestClient()),
               IllegalStateException); // Gas field must be greater than or equal to 21000
}
