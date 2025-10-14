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
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <chrono>
#include <gtest/gtest.h>

using namespace Hiero;

class ContractUpdateTransactionIntegrationTests : public BaseIntegrationTest
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

//-----
TEST_F(ContractUpdateTransactionIntegrationTests, CanAddHookToContract)
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
  ASSERT_NO_THROW(contractId =
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

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ContractUpdateTransaction()
                                .setContractId(contractId)
                                .addHook(hookCreationDetails)
                                .freezeWith(&getTestClient())
                                .sign(newAdminKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));

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
TEST_F(ContractUpdateTransactionIntegrationTests, CannotAddDuplicateHooksToContract)
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
  ASSERT_NO_THROW(contractId =
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

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = ContractUpdateTransaction()
                             .setContractId(contractId)
                             .addHook(hookCreationDetails)
                             .addHook(hookCreationDetails)
                             .freezeWith(&getTestClient())
                             .sign(newAdminKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_ID_REPEATED_IN_CREATION_DETAILS

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
TEST_F(ContractUpdateTransactionIntegrationTests, CannotAddHookToContractThatAlreadyExists)
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
  ASSERT_NO_THROW(contractId =
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

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  EXPECT_NO_THROW(ContractUpdateTransaction()
                    .setContractId(contractId)
                    .addHook(hookCreationDetails)
                    .freezeWith(&getTestClient())
                    .sign(newAdminKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When / Then
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = ContractUpdateTransaction()
                             .setContractId(contractId)
                             .addHook(hookCreationDetails)
                             .freezeWith(&getTestClient())
                             .sign(newAdminKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_ID_IN_USE

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
TEST_F(ContractUpdateTransactionIntegrationTests, CanAddHookToContractWithStorageUpdates)
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
  ASSERT_NO_THROW(contractId =
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
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ContractUpdateTransaction()
                                .setContractId(contractId)
                                .addHook(hookCreationDetails)
                                .freezeWith(&getTestClient())
                                .sign(newAdminKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));

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
TEST_F(ContractUpdateTransactionIntegrationTests, CanDeleteHookFromContract)
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
  ASSERT_NO_THROW(contractId =
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

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  const int64_t hookId = 1LL;
  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(hookId);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  ASSERT_NO_THROW(ContractUpdateTransaction()
                    .setContractId(contractId)
                    .addHook(hookCreationDetails)
                    .freezeWith(&getTestClient())
                    .sign(newAdminKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = ContractUpdateTransaction()
                                .setContractId(contractId)
                                .deleteHook(hookId)
                                .freezeWith(&getTestClient())
                                .sign(newAdminKey)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));

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
TEST_F(ContractUpdateTransactionIntegrationTests, CannotDeleteNonExistentHookFromContract)
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
  ASSERT_NO_THROW(contractId =
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

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  ASSERT_NO_THROW(ContractUpdateTransaction()
                    .setContractId(contractId)
                    .addHook(hookCreationDetails)
                    .freezeWith(&getTestClient())
                    .sign(newAdminKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = ContractUpdateTransaction()
                             .setContractId(contractId)
                             .deleteHook(999LL)
                             .freezeWith(&getTestClient())
                             .sign(newAdminKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_NOT_FOUND

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));

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
TEST_F(ContractUpdateTransactionIntegrationTests, CannotAddAndDeleteSameHookFromContract)
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
  ASSERT_NO_THROW(contractId =
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

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  const int64_t hookId = 1LL;
  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(hookId);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = ContractUpdateTransaction()
                             .setContractId(contractId)
                             .addHook(hookCreationDetails)
                             .deleteHook(hookId)
                             .freezeWith(&getTestClient())
                             .sign(newAdminKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_NOT_FOUND

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));

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
TEST_F(ContractUpdateTransactionIntegrationTests, CannotDeleteAlreadyDeleteHookFromContract)
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
  ASSERT_NO_THROW(contractId =
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

  LambdaEvmHook lambdaEvmHook;
  lambdaEvmHook.setContractId(getTestLambdaId());

  const int64_t hookId = 1LL;
  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  ASSERT_NO_THROW(ContractUpdateTransaction()
                    .setContractId(contractId)
                    .addHook(hookCreationDetails)
                    .freezeWith(&getTestClient())
                    .sign(newAdminKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  ASSERT_NO_THROW(ContractUpdateTransaction()
                    .setContractId(contractId)
                    .deleteHook(hookId)
                    .freezeWith(&getTestClient())
                    .sign(newAdminKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When
  TransactionReceipt txReceipt;
  EXPECT_THROW(txReceipt = ContractUpdateTransaction()
                             .setContractId(contractId)
                             .deleteHook(hookId)
                             .freezeWith(&getTestClient())
                             .sign(newAdminKey)
                             .execute(getTestClient())
                             .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_NOT_FOUND

  // Then
  ContractInfo contractInfo;
  ASSERT_NO_THROW(contractInfo = ContractInfoQuery().setContractId(contractId).execute(getTestClient()));

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
