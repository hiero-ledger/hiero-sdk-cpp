// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "AccountUpdateTransaction.h"
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  void SetUp() override
  {
    BaseIntegrationTest::SetUp();

    mContractId =
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

  [[nodiscard]] const ContractId& getTestContractId() const { return mContractId; }

private:
  ContractId mContractId;
};

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, ExecuteAccountUpdateTransaction)
{
  // Given
  const std::shared_ptr<PrivateKey> initialPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> newPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const bool newReceiverSignatureRequired = true;
  const std::chrono::system_clock::duration newAutoRenewPeriod = std::chrono::seconds(8000000);
  const std::chrono::system_clock::time_point newExpirationTime =
    std::chrono::system_clock::now() + std::chrono::seconds(3000000);
  const std::string newAccountMemo = "New Account Memo!";
  const int32_t newMaxAutomaticTokenAssociations = 100;
  const uint64_t newStakedNodeId = 0ULL;
  const bool newDeclineStakingRewards = true;

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(initialPrivateKey->getPublicKey())
                                .setAutoRenewPeriod(std::chrono::seconds(2592000))
                                .setAccountMemo("test account memo")
                                .setMaxAutomaticTokenAssociations(10)
                                .setStakedAccountId(AccountId(2ULL))
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountUpdateTransaction()
                                 .setAccountId(accountId)
                                 .setKey(newPrivateKey->getPublicKey())
                                 .setReceiverSignatureRequired(newReceiverSignatureRequired)
                                 .setAutoRenewPeriod(newAutoRenewPeriod)
                                 .setExpirationTime(newExpirationTime)
                                 .setAccountMemo(newAccountMemo)
                                 .setMaxAutomaticTokenAssociations(newMaxAutomaticTokenAssociations)
                                 .setStakedNodeId(newStakedNodeId)
                                 .setDeclineStakingReward(newDeclineStakingRewards)
                                 .freezeWith(&getTestClient())
                                 .sign(initialPrivateKey)
                                 .sign(newPrivateKey)
                                 .execute(getTestClient()));

  // Then
  AccountInfo accountInfo;
  ASSERT_NO_THROW(const TransactionReceipt txReceipt = txResponse.getReceipt(getTestClient()));
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_FALSE(accountInfo.mIsDeleted);
  EXPECT_EQ(accountInfo.mKey->toBytes(), newPrivateKey->getPublicKey()->toBytes());
  EXPECT_EQ(accountInfo.mReceiverSignatureRequired, newReceiverSignatureRequired);
  // Can't do direct time comparison due to system clock precision limitations
  EXPECT_LE(accountInfo.mExpirationTime, newExpirationTime);
  EXPECT_EQ(accountInfo.mAutoRenewPeriod, newAutoRenewPeriod);
  EXPECT_EQ(accountInfo.mMemo, newAccountMemo);
  EXPECT_EQ(accountInfo.mMaxAutomaticTokenAssociations, newMaxAutomaticTokenAssociations);
  ASSERT_TRUE(accountInfo.mStakingInfo.mStakedNodeId.has_value());
  EXPECT_EQ(accountInfo.mStakingInfo.mStakedNodeId, newStakedNodeId);
  EXPECT_EQ(accountInfo.mStakingInfo.mDeclineRewards, newDeclineStakingRewards);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(newPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotUpdateAccountWithoutSignature)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(
    const TransactionReceipt txReceipt =
      AccountUpdateTransaction().setAccountId(accountId).execute(getTestClient()).getReceipt(getTestClient()),
    ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotUpdateAccountWithoutAccountId)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               PrecheckStatusException); // ACCOUNT_ID_DOES_NOT_EXIST

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, ExecuteWithOnlyAccountId)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, InvalidAutoRenewPeriod)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::chrono::system_clock::duration invalidAutoRenewPeriod = std::chrono::seconds(777600000);

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .setAccountId(accountId)
                                                      .setAutoRenewPeriod(invalidAutoRenewPeriod)
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // AUTORENEW_DURATION_NOT_IN_RANGE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests,
       CannotUpdateMaxAutomaticTokenAssociationsToLowerThanCurrentlyAssociated)
{
  // Given
  const int64_t amount = 10LL;

  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = std::shared_ptr<PrivateKey>(
      ED25519PrivateKey::fromString(
        "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137")
        .release()));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(accountKey)
                                .setMaxAutomaticTokenAssociations(1)
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  ASSERT_NO_THROW(TransferTransaction()
                    .addTokenTransfer(tokenId, getTestClient().getOperatorAccountId().value(), -amount)
                    .addTokenTransfer(tokenId, accountId, amount)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // When / Then
  ASSERT_THROW(AccountUpdateTransaction()
                 .setAccountId(accountId)
                 .setMaxAutomaticTokenAssociations(0)
                 .freezeWith(&getTestClient())
                 .sign(accountKey)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // EXISTING_AUTOMATIC_ASSOCIATIONS_EXCEED_GIVEN_LIMIT
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CanAddHookToAccount)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .addHookToCreate(hookCreationDetails)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotUpdateWithMultipleOfSameHook)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .setAccountId(accountId)
                                                      .addHookToCreate(hookCreationDetails)
                                                      .addHookToCreate(hookCreationDetails)
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               PrecheckStatusException); // HOOK_ID_REPEATED_IN_CREATION_DETAILS

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotUpdateWithHookAlreadyInUse)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .addHookToCreate(hookCreationDetails)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .setAccountId(accountId)
                                                      .addHookToCreate(hookCreationDetails)
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_ID_IN_USE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CanAddHookToAccountWithStorageUpdates)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

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
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .addHookToCreate(hookCreationDetails)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CanDeleteHook)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  const int64_t hookId = 1LL;
  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(hookId);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .addHookToCreate(hookCreationDetails)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // When / Then
  EXPECT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .addHookToDelete(hookId)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotDeleteNonExistentHook)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  ASSERT_NO_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                         .setAccountId(accountId)
                                                         .addHookToCreate(hookCreationDetails)
                                                         .freezeWith(&getTestClient())
                                                         .sign(privateKey)
                                                         .execute(getTestClient())
                                                         .getReceipt(getTestClient()));

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .setAccountId(accountId)
                                                      .addHookToDelete(999LL)
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_NOT_FOUND

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountUpdateTransactionIntegrationTests, CannotAddAndDeleteSameHook)
{
  // Given
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId accountId;
  ASSERT_NO_THROW(accountId = AccountCreateTransaction()
                                .setKeyWithoutAlias(privateKey->getPublicKey())
                                .execute(getTestClient())
                                .getReceipt(getTestClient())
                                .mAccountId.value());

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  const int64_t hookId = 1LL;
  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(hookId);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountUpdateTransaction()
                                                      .setAccountId(accountId)
                                                      .addHookToCreate(hookCreationDetails)
                                                      .addHookToDelete(hookId)
                                                      .freezeWith(&getTestClient())
                                                      .sign(privateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_NOT_FOUND

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(privateKey)
                    .execute(getTestClient()));
}
