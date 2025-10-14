// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractFunctionParameters.h"
#include "ContractId.h"
#include "ContractInfo.h"
#include "ContractInfoQuery.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <chrono>
#include <gtest/gtest.h>

using namespace Hiero;

class ContractCreateTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  void SetUp() override
  {
    mLambdaId =
      ContractCreateTransaction()
        .setBytecode(internal::HexConverter::hexToBytes("6080604052348015600e575f5ffd5b506103da8061001c5f395ff3fe608060"
                                                        "40526004361061001d575f3560e01c80630b6c5c0414610021"
                                                        "575b5f5ffd5b61003b6004803603810190610036919061021c565b61005156"
                                                        "5b60405161004891906102ed565b60405180910390f35b5f61"
                                                        "016d73ffffffffffffffffffffffffffffffffffffffff163073ffffffffff"
                                                        "ffffffffffffffffffffffffffffff16146100c2576040517f"
                                                        "08c379a0000000000000000000000000000000000000000000000000000000"
                                                        "0081526004016100b990610386565b60405180910390fd5b60"
                                                        "019050979650505050505050565b5f5ffd5b5f5ffd5b5f73ffffffffffffff"
                                                        "ffffffffffffffffffffffffff82169050919050565b5f6101"
                                                        "02826100d9565b9050919050565b610112816100f8565b811461011c575f5f"
                                                        "fd5b50565b5f8135905061012d81610109565b92915050565b"
                                                        "5f819050919050565b61014581610133565b811461014f575f5ffd5b50565b"
                                                        "5f813590506101608161013c565b92915050565b5f5ffd5b5f"
                                                        "5ffd5b5f5ffd5b5f5f83601f84011261018757610186610166565b5b823590"
                                                        "5067ffffffffffffffff8111156101a4576101a361016a565b"
                                                        "5b6020830191508360018202830111156101c0576101bf61016e565b5b9250"
                                                        "929050565b5f5f83601f8401126101dc576101db610166565b"
                                                        "5b8235905067ffffffffffffffff8111156101f9576101f861016a565b5b60"
                                                        "20830191508360018202830111156102155761021461016e56"
                                                        "5b5b9250929050565b5f5f5f5f5f5f5f60a0888a0312156102375761023661"
                                                        "00d1565b5b5f6102448a828b0161011f565b97505060206102"
                                                        "558a828b01610152565b96505060406102668a828b01610152565b95505060"
                                                        "6088013567ffffffffffffffff811115610287576102866100"
                                                        "d5565b5b6102938a828b01610172565b9450945050608088013567ffffffff"
                                                        "ffffffff8111156102b6576102b56100d5565b5b6102c28a82"
                                                        "8b016101c7565b925092505092959891949750929550565b5f811515905091"
                                                        "9050565b6102e7816102d3565b82525050565b5f6020820190"
                                                        "506103005f8301846102de565b92915050565b5f8282526020820190509291"
                                                        "5050565b7f436f6e74726163742063616e206f6e6c79206265"
                                                        "2063616c6c656420617320615f8201527f20686f6f6b000000000000000000"
                                                        "00000000000000000000000000000000000060208201525056"
                                                        "5b5f610370602583610306565b915061037b82610316565b60408201905091"
                                                        "9050565b5f6020820190508181035f83015261039d81610364"
                                                        "565b905091905056fea2646970667358221220a8c76458204f8bb9a86f59ec"
                                                        "2f0ccb7cbe8ae4dcb65700c4b6ee91a39404083a64736f6c63"
                                                        "4300081e0033"))
        .setGas(300000ULL)
        .execute(getTestClient())
        .getReceipt(getTestClient())
        .mContractId.value();
  }

  [[nodiscard]] const ContractId& getTestLambdaId() const { return mLambdaId; }

private:
  ContractId mLambdaId;
};

//-----
TEST_F(ContractCreateTransactionIntegrationTests, ExecuteContractCreateTransaction)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2016);
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
                      .setAutoRenewPeriod(autoRenewPeriod)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .setDeclineStakingReward(true)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));
  EXPECT_EQ(contractInfo.mContractId, contractId);
  EXPECT_EQ(contractInfo.mAccountId.toString(), contractId.toString());
  EXPECT_GT(contractInfo.mExpirationTime, std::chrono::system_clock::now());
  EXPECT_EQ(contractInfo.mAutoRenewPeriod, autoRenewPeriod);
  EXPECT_EQ(contractInfo.mStorage, 128ULL);
  EXPECT_EQ(contractInfo.mMemo, memo);

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
TEST_F(ContractCreateTransactionIntegrationTests, CreateContractWithNoAdminKey)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2016);
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
                      .setGas(1000000ULL)
                      .setAutoRenewPeriod(autoRenewPeriod)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .setDeclineStakingReward(true)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));
  EXPECT_EQ(contractInfo.mContractId, contractId);
  EXPECT_EQ(contractInfo.mAccountId.toString(), contractId.toString());
  EXPECT_GT(contractInfo.mExpirationTime, std::chrono::system_clock::now());
  EXPECT_EQ(contractInfo.mAutoRenewPeriod, autoRenewPeriod);
  EXPECT_EQ(contractInfo.mStorage, 128ULL);
  EXPECT_EQ(contractInfo.mMemo, memo);

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CannotCreateContractWithNoGas)
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

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 ContractCreateTransaction()
                   .setAdminKey(operatorKey->getPublicKey())
                   .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                   .setBytecodeFileId(fileId)
                   .setMemo("[e2e::ContractCreateTransaction]")
                   .execute(getTestClient())
                   .getReceipt(getTestClient()),
               PrecheckStatusException); // INSUFFICIENT_GAS

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CannotCreateContractWithNoConstructorParameters)
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

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = ContractCreateTransaction()
                                                      .setAdminKey(operatorKey->getPublicKey())
                                                      .setGas(1000000ULL)
                                                      .setBytecodeFileId(fileId)
                                                      .setMemo("[e2e::ContractCreateTransaction]")
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // CONTRACT_REVERT_EXECUTED

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CannotCreateContractWithBytecodeFileId)
{
  // Given/ When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt =
                 ContractCreateTransaction()
                   .setGas(1000000ULL)
                   .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                   .setMemo("[e2e::ContractCreateTransaction]")
                   .execute(getTestClient())
                   .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_FILE_ID
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CreateContractWithHook)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2016);
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
                      .setGas(1000000ULL)
                      .setAutoRenewPeriod(autoRenewPeriod)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .setDeclineStakingReward(true)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = ContractCreateTransaction()
                                 .setBytecode(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                                 .setGas(300000ULL)
                                 .addHook(hookCreationDetails)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  EXPECT_TRUE(txReceipt.mContractId.has_value());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CreateContractWithHookWithStorageUpdates)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2016);
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
                      .setGas(1000000ULL)
                      .setAutoRenewPeriod(autoRenewPeriod)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .setDeclineStakingReward(true)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  LambdaStorageSlot lambdaStorageSlot;
  lambdaStorageSlot.setKey({ std::byte(0x01), std::byte(0x23), std::byte(0x45) });
  lambdaStorageSlot.setValue({ std::byte(0x67), std::byte(0x89), std::byte(0xAB) });

  LambdaStorageUpdate lambdaStorageUpdate;
  lambdaStorageUpdate.setStorageSlot(lambdaStorageSlot);

  lambdaEvmHook.addStorageUpdate(lambdaStorageUpdate);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = ContractCreateTransaction()
                                 .setBytecode(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                                 .setGas(300000ULL)
                                 .addHook(hookCreationDetails)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  EXPECT_TRUE(txReceipt.mContractId.has_value());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CannotCreateContractWithNoContractIdForHook)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2016);
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
                      .setGas(1000000ULL)
                      .setAutoRenewPeriod(autoRenewPeriod)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .setDeclineStakingReward(true)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  LambdaEvmHook lambdaEvmHook;

  LambdaStorageSlot lambdaStorageSlot;
  lambdaStorageSlot.setKey({ std::byte(0x01), std::byte(0x23), std::byte(0x45) });
  lambdaStorageSlot.setValue({ std::byte(0x67), std::byte(0x89), std::byte(0xAB) });

  LambdaStorageUpdate lambdaStorageUpdate;
  lambdaStorageUpdate.setStorageSlot(lambdaStorageSlot);

  lambdaEvmHook.addStorageUpdate(lambdaStorageUpdate);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  TransactionResponse txResponse;
  EXPECT_THROW(txResponse = ContractCreateTransaction()
                              .setBytecode(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                              .setGas(300000ULL)
                              .addHook(hookCreationDetails)
                              .execute(getTestClient()),
               ReceiptStatusException); // CONTRACT_REVERTED

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CannotCreateContractWithDuplicateHookId)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::string memo = "[e2e::ContractCreateTransaction]";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2016);
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
                      .setGas(1000000ULL)
                      .setAutoRenewPeriod(autoRenewPeriod)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .setDeclineStakingReward(true)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  TransactionResponse txResponse;
  EXPECT_THROW(txResponse = ContractCreateTransaction()
                              .setBytecode(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                              .setGas(300000ULL)
                              .addHook(hookCreationDetails)
                              .addHook(hookCreationDetails)
                              .execute(getTestClient()),
               ReceiptStatusException); // HOOK_ID_REPEATED_IN_CREATION_DETAILS

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(ContractCreateTransactionIntegrationTests, CreateContractWithHookWithAdminKey)
{
  // Given
  const std::unique_ptr<PrivateKey> operatorKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
  const std::shared_ptr<PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();
  const std::string memo = "[e2e::ContractCreateTransaction]";
  const std::chrono::system_clock::duration autoRenewPeriod = std::chrono::hours(2016);
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
                      .setGas(1000000ULL)
                      .setAutoRenewPeriod(autoRenewPeriod)
                      .setConstructorParameters(ContractFunctionParameters().addString("Hello from Hiero.").toBytes())
                      .setMemo(memo)
                      .setAutoRenewAccountId(AccountId(2ULL))
                      .setStakedAccountId(AccountId(2ULL))
                      .setDeclineStakingReward(true)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mContractId.value());

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  LambdaStorageSlot lambdaStorageSlot;
  lambdaStorageSlot.setKey({ std::byte(0x01), std::byte(0x23), std::byte(0x45) });
  lambdaStorageSlot.setValue({ std::byte(0x67), std::byte(0x89), std::byte(0xAB) });

  LambdaStorageUpdate lambdaStorageUpdate;
  lambdaStorageUpdate.setStorageSlot(lambdaStorageSlot);

  lambdaEvmHook.addStorageUpdate(lambdaStorageUpdate);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);
  hookCreationDetails.setAdminKey(adminKey);

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = ContractCreateTransaction()
                                 .setBytecode(internal::Utilities::stringToByteVector(getTestSmartContractBytecode()))
                                 .setGas(300000ULL)
                                 .addHook(hookCreationDetails)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  EXPECT_TRUE(txReceipt.mContractId.has_value());

  // Clean up
  ASSERT_NO_THROW(const TransactionReceipt txReceipt =
                    FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}