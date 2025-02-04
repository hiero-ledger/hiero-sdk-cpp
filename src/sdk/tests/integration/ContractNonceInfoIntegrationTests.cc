// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractFunctionResult.h"
#include "ContractId.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class ContractNonceInfoIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::string& getTestBytecodeHexWithContractNonceInfo() const
  {
    return mTestBytecodeHexWithContractNonceInfo;
  }

private:
  const std::string mTestBytecodeHexWithContractNonceInfo =
    "6080604052348015600f57600080fd5b50604051601a90603b565b604051809103906000f0801580156035573d6000803e3d6000fd5"
    "b50506047565b605c8061009483390190565b603f806100556000396000f3fe6080604052600080fdfea2646970667358221220a201"
    "22cbad3457fedcc0600363d6e895f17048f5caa4afdab9e655123737567d64736f6c634300081200336080604052348015600f57600"
    "080fd5b50603f80601d6000396000f3fe6080604052600080fdfea264697066735822122053dfd8835e3dc6fedfb8b4806460b9b716"
    "3f8a7248bac510c6d6808d9da9d6d364736f6c63430008120033";
};

//-----
TEST_F(ContractNonceInfoIntegrationTests, ContractADeploysContractBInConstructor)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractADeploysContractBInConstructor]";
  FileId fileId;
  ASSERT_NO_THROW(fileId =
                    FileCreateTransaction()
                      .setKeys({ operatorKey->getPublicKey() })
                      .setContents(internal::Utilities::stringToByteVector(getTestBytecodeHexWithContractNonceInfo()))
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mFileId.value());

  TransactionResponse response;
  ASSERT_NO_THROW(response = ContractCreateTransaction()
                               .setAdminKey(operatorKey->getPublicKey())
                               .setGas(100000ULL)
                               .setBytecodeFileId(fileId)
                               .setMemo(memo)
                               .execute(getTestClient()));

  ContractFunctionResult contractFunctionResult;
  ASSERT_NO_THROW(contractFunctionResult = response.getRecord(getTestClient()).mContractFunctionResult.value());

  ContractId contractA = contractFunctionResult.mContractId;
  std::vector<ContractNonceInfo> contractNonces;

  for (const auto& mContractNonce : contractFunctionResult.mContractNonces)
  {
    if (!(mContractNonce.mContractId == contractA))
    {
      contractNonces.push_back(mContractNonce);
    }
  }

  ContractId contractB = (*contractNonces.begin()).mContractId;

  ContractNonceInfo contractANonceInfo;
  ContractNonceInfo contractBNonceInfo;

  for (const auto& mContractNonce : contractFunctionResult.mContractNonces)
  {
    if (mContractNonce.mContractId == contractA)
    {
      contractANonceInfo = mContractNonce;
    }
    else if (mContractNonce.mContractId == contractB)
    {
      contractBNonceInfo = mContractNonce;
    }
  }

  // When
  EXPECT_EQ(contractANonceInfo.mNonce, 2); // A.nonce = 2
  EXPECT_EQ(contractBNonceInfo.mNonce, 1); // B.nonce = 1
  EXPECT_EQ(contractFunctionResult.mSignerNonce.has_value(),
            false); // signer nonce should be set only for Ethereum transactions

  // Clean up
  ContractId contractId;
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(contractId = response.getReceipt(getTestClient()).mContractId.value());
  ASSERT_NO_THROW(txReceipt = ContractDeleteTransaction()
                                .setContractId(contractId)
                                .setTransferAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  ASSERT_NO_THROW(txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}
