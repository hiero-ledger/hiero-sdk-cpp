// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "HookStoreTransaction.h"
#include "Status.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/ReceiptStatusException.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookEntityId.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/HookId.h"
#include "hooks/EvmHook.h"
#include "hooks/EvmHookStorageSlot.h"
#include "hooks/EvmHookStorageUpdate.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class HookStoreTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  void SetUp() override
  {
    BaseIntegrationTest::SetUp();

    mContractId =
      ContractCreateTransaction()
        .setBytecode(internal::HexConverter::hexToBytes(
          "6080604052348015600e575f5ffd5b506103da8061001c5f395ff3fe60806040526004361061001d575f3560e01c80630b6c5c041461"
          "0021575b5f5ffd5b61003b6004803603810190610036919061021c565b610051565b60405161004891906102ed565b60405180910390"
          "f35b5f61016d73ffffffffffffffffffffffffffffffffffffffff163073ffffffffffffffffffffffffffffffffffffffff16146100"
          "c2576040517f08c379a00000000000000000000000000000000000000000000000000000000081526004016100b990610386565b6040"
          "5180910390fd5b60019050979650505050505050565b5f5ffd5b5f5ffd5b5f73ffffffffffffffffffffffffffffffffffffffff8216"
          "9050919050565b5f610102826100d9565b9050919050565b610112816100f8565b811461011c575f5ffd5b50565b5f8135905061012d"
          "81610109565b92915050565b5f819050919050565b61014581610133565b811461014f575f5ffd5b50565b5f81359050610160816101"
          "3c565b92915050565b5f5ffd5b5f5ffd5b5f5ffd5b5f5f83601f84011261018757610186610166565b5b8235905067ffffffffffffff"
          "ff8111156101a4576101a361016a565b5b6020830191508360018202830111156101c0576101bf61016e565b5b9250929050565b5f5f"
          "83601f8401126101dc576101db610166565b5b8235905067ffffffffffffffff8111156101f9576101f861016a565b5b602083019150"
          "8360018202830111156102155761021461016e565b5b9250929050565b5f5f5f5f5f5f5f60a0888a031215610237576102366100d156"
          "5b5b5f6102448a828b0161011f565b97505060206102558a828b01610152565b96505060406102668a828b01610152565b9550506060"
          "88013567ffffffffffffffff811115610287576102866100d5565b5b6102938a828b01610172565b9450945050608088013567ffffff"
          "ffffffffff8111156102b6576102b56100d5565b5b6102c28a828b016101c7565b925092505092959891949750929550565b5f811515"
          "9050919050565b6102e7816102d3565b82525050565b5f6020820190506103005f8301846102de565b92915050565b5f828252602082"
          "01905092915050565b7f436f6e74726163742063616e206f6e6c792062652063616c6c656420617320615f8201527f20686f6f6b0000"
          "00000000000000000000000000000000000000000000000000602082015250565b5f610370602583610306565b915061037b82610316"
          "565b604082019050919050565b5f6020820190508181035f83015261039d81610364565b905091905056fea2646970667358221220a8"
          "c76458204f8bb9a86f59ec2f0ccb7cbe8ae4dcb65700c4b6ee91a39404083a64736f6c634300081e0033"))
        .setGas(300000ULL)
        .execute(getTestClient())
        .getReceipt(getTestClient())
        .mContractId.value();

    mPrivateKey = ED25519PrivateKey::generatePrivateKey();

    EvmHookStorageUpdate storageUpdate;
    EvmHookStorageSlot storageSlot;
    storageSlot.setKey(getTestStorageKey1());
    storageSlot.setValue(getTestStorageValue1());
    storageUpdate.setStorageSlot(storageSlot);

    EvmHook evmHook;
    EvmHookSpec evmHookSpec;
    evmHookSpec.setContractId(getTestContractId());
    evmHook.setEvmHookSpec(evmHookSpec);
    evmHook.addStorageUpdate(storageUpdate);

    HookCreationDetails hookCreationDetails;
    hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
    hookCreationDetails.setHookId(getTestHookId());
    hookCreationDetails.setEvmHook(evmHook);

    mAccountId = AccountCreateTransaction()
                   .setKeyWithoutAlias(mPrivateKey)
                   .setInitialBalance(Hbar(1LL))
                   .addHook(hookCreationDetails)
                   .execute(getTestClient())
                   .getReceipt(getTestClient())
                   .mAccountId.value();

    HookEntityId hookEntityId;
    hookEntityId.setAccountId(mAccountId);

    mTransactionHookId.setHookId(getTestHookId());
    mTransactionHookId.setEntityId(hookEntityId);
  }

  void TearDown() override
  {
    // Clean up account
    AccountDeleteTransaction()
      .setDeleteAccountId(mAccountId)
      .setTransferAccountId(AccountId(2ULL))
      .freezeWith(&getTestClient())
      .sign(mPrivateKey)
      .execute(getTestClient());

    BaseIntegrationTest::TearDown();
  }

  [[nodiscard]] inline const std::vector<std::byte>& getTestStorageKey1() const { return mStorageKey1; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestStorageKey2() const { return mStorageKey2; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestStorageValue1() const { return mStorageValue1; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestStorageValue2() const { return mStorageValue2; }
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mContractId; }
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline const std::shared_ptr<PrivateKey>& getTestPrivateKey() const { return mPrivateKey; }
  [[nodiscard]] inline int64_t getTestHookId() const { return mHookId; }
  [[nodiscard]] inline const HookId& getTransactionHookId() const { return mTransactionHookId; }

private:
  ContractId mContractId;
  AccountId mAccountId;
  std::shared_ptr<PrivateKey> mPrivateKey;
  int64_t mHookId = 1LL;
  HookId mTransactionHookId;
  const std::vector<std::byte> mStorageKey1 = { std::byte(0x01), std::byte(0x02), std::byte(0x03), std::byte(0x04) };
  const std::vector<std::byte> mStorageKey2 = { std::byte(0x09), std::byte(0x0A), std::byte(0x0B), std::byte(0x0C) };
  const std::vector<std::byte> mStorageValue1 = { std::byte(0x05), std::byte(0x06), std::byte(0x07), std::byte(0x08) };
  const std::vector<std::byte> mStorageValue2 = { std::byte(0x0D), std::byte(0x0E), std::byte(0x0F), std::byte(0x10) };
};

//-----
TEST_F(HookStoreTransactionIntegrationTests, ExecuteHookStoreTransactionWithSingleStorageUpdate)
{
  // Given
  EvmHookStorageUpdate storageUpdate;
  EvmHookStorageSlot storageSlot;
  storageSlot.setKey(getTestStorageKey2());
  storageSlot.setValue(getTestStorageValue2());
  storageUpdate.setStorageSlot(storageSlot);

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = HookStoreTransaction()
                                 .setHookId(getTransactionHookId())
                                 .addStorageUpdate(storageUpdate)
                                 .freezeWith(&getTestClient())
                                 .sign(getTestPrivateKey())
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
}

//-----
TEST_F(HookStoreTransactionIntegrationTests, ExecuteHookStoreTransactionWithTooManyStorageUpdates)
{
  // Given
  EvmHookStorageUpdate storageUpdate;
  EvmHookStorageSlot storageSlot;
  storageSlot.setKey(getTestStorageKey2());
  storageSlot.setValue(getTestStorageValue2());
  storageUpdate.setStorageSlot(storageSlot);

  HookStoreTransaction hookStoreTx = HookStoreTransaction().setHookId(getTransactionHookId());
  for (int i = 0; i < 256; ++i)
  {
    hookStoreTx.addStorageUpdate(storageUpdate);
  }

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = hookStoreTx.freezeWith(&getTestClient())
                                                      .sign(getTestPrivateKey())
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // TOO_MANY_EVM_HOOK_STORAGE_UPDATES
}

//-----
TEST_F(HookStoreTransactionIntegrationTests, ExecuteHookStoreTransactionWithoutProperSignatures)
{
  // Given
  EvmHookStorageUpdate storageUpdate;
  EvmHookStorageSlot storageSlot;
  storageSlot.setKey(getTestStorageKey2());
  storageSlot.setValue(getTestStorageValue2());
  storageUpdate.setStorageSlot(storageSlot);

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = HookStoreTransaction()
                                                      .setHookId(getTransactionHookId())
                                                      .addStorageUpdate(storageUpdate)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE
}

//-----
TEST_F(HookStoreTransactionIntegrationTests, ExecuteHookStoreTransactionWithoutInvalidHookId)
{
  // Given
  EvmHookStorageUpdate storageUpdate;
  EvmHookStorageSlot storageSlot;
  storageSlot.setKey(getTestStorageKey1());
  storageSlot.setValue(getTestStorageValue1());
  storageUpdate.setStorageSlot(storageSlot);

  HookEntityId hookEntityId;
  hookEntityId.setAccountId(getTestAccountId());

  HookId hookId;
  hookId.setHookId(999LL);
  hookId.setEntityId(hookEntityId);

  // When / Then
  EXPECT_THROW(HookStoreTransaction()
                 .setHookId(hookId)
                 .addStorageUpdate(storageUpdate)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_NOT_FOUND
}