// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "Defaults.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "PublicKey.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "WrappedTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "exceptions/UninitializedException.h"
#include "impl/HexConverter.h"
#include "impl/Network.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AccountCreateTransactionIntegrationTests : public BaseIntegrationTest
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
TEST_F(AccountCreateTransactionIntegrationTests, ExecuteAccountCreateTransaction)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> testPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PublicKey> testPublicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(testPrivateKey->getPublicKey());
  const EvmAddress testEvmAddress = testPublicKey->toEvmAddress();
  const Hbar testInitialBalance(1000LL, HbarUnit::TINYBAR());
  const std::chrono::system_clock::duration testAutoRenewPeriod = std::chrono::seconds(2592000);
  const std::string testMemo = "test account memo";
  const int32_t testMaxAutomaticTokenAssociations = -1;

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithoutAlias(testPublicKey)
                                 .setInitialBalance(testInitialBalance)
                                 .setReceiverSignatureRequired(true)
                                 .setAutoRenewPeriod(testAutoRenewPeriod)
                                 .setAccountMemo(testMemo)
                                 .setMaxAutomaticTokenAssociations(testMaxAutomaticTokenAssociations)
                                 .setDeclineStakingReward(true)
                                 .setAlias(testEvmAddress)
                                 .freezeWith(&getTestClient())
                                 .sign(testPrivateKey)
                                 .execute(getTestClient()));

  // Then
  AccountId accountId;
  AccountInfo accountInfo;
  ASSERT_NO_THROW(accountId = txResponse.getReceipt(getTestClient()).mAccountId.value());
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_EQ(internal::HexConverter::hexToBytes(accountInfo.mContractAccountId), testEvmAddress.toBytes());
  EXPECT_EQ(accountInfo.mKey->toBytes(), testPublicKey->toBytes());
  EXPECT_EQ(accountInfo.mBalance, testInitialBalance);
  EXPECT_EQ(accountInfo.mAutoRenewPeriod, testAutoRenewPeriod);
  EXPECT_EQ(accountInfo.mMemo, testMemo);
  EXPECT_EQ(accountInfo.mMaxAutomaticTokenAssociations, testMaxAutomaticTokenAssociations);
  EXPECT_TRUE(accountInfo.mStakingInfo.mDeclineRewards);
  EXPECT_FALSE(accountInfo.mStakingInfo.mStakedAccountId.has_value());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, MutuallyExclusiveStakingIds)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> testPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PublicKey> testPublicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(testPrivateKey->getPublicKey());
  const AccountId operatorAccountId(2ULL);
  const uint64_t nodeId = 0ULL;

  // When
  TransactionResponse txResponseStakedAccountId;
  TransactionResponse txResponseStakedNodeId;

  EXPECT_NO_THROW(txResponseStakedAccountId = AccountCreateTransaction()
                                                .setKeyWithoutAlias(testPublicKey)
                                                .setStakedAccountId(operatorAccountId)
                                                .freezeWith(&getTestClient())
                                                .execute(getTestClient()));
  EXPECT_NO_THROW(txResponseStakedNodeId = AccountCreateTransaction()
                                             .setKeyWithoutAlias(testPublicKey)
                                             .setStakedNodeId(nodeId)
                                             .freezeWith(&getTestClient())
                                             .execute(getTestClient()));

  // Then
  AccountId accountIdStakedAccountId;
  AccountId accountIdStakedNodeId;
  AccountInfo accountInfo;
  ASSERT_NO_THROW(accountIdStakedAccountId = txResponseStakedAccountId.getReceipt(getTestClient()).mAccountId.value());
  ASSERT_NO_THROW(accountIdStakedNodeId = txResponseStakedNodeId.getReceipt(getTestClient()).mAccountId.value());

  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountIdStakedAccountId).execute(getTestClient()));
  EXPECT_EQ(accountInfo.mAccountId, accountIdStakedAccountId);
  EXPECT_EQ(accountInfo.mKey->toBytes(), testPublicKey->toBytes());
  ASSERT_TRUE(accountInfo.mStakingInfo.mStakedAccountId.has_value());
  EXPECT_EQ(accountInfo.mStakingInfo.mStakedAccountId, operatorAccountId);
  EXPECT_FALSE(accountInfo.mStakingInfo.mStakedNodeId.has_value());

  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountIdStakedNodeId).execute(getTestClient()));
  EXPECT_EQ(accountInfo.mAccountId, accountIdStakedNodeId);
  EXPECT_EQ(accountInfo.mKey->toBytes(), testPublicKey->toBytes());
  EXPECT_FALSE(accountInfo.mStakingInfo.mStakedAccountId.has_value());
  ASSERT_TRUE(accountInfo.mStakingInfo.mStakedNodeId.has_value());
  EXPECT_EQ(accountInfo.mStakingInfo.mStakedNodeId, nodeId);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountIdStakedAccountId)
                    .setTransferAccountId(operatorAccountId)
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountIdStakedNodeId)
                    .setTransferAccountId(operatorAccountId)
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, NoInitialBalance)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> testKey = ED25519PrivateKey::generatePrivateKey();

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse =
                    AccountCreateTransaction().setKeyWithoutAlias(testKey->getPublicKey()).execute(getTestClient()));

  // Then
  AccountId accountId;
  AccountInfo accountInfo;
  ASSERT_NO_THROW(accountId = txResponse.getReceipt(getTestClient()).mAccountId.value());
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_FALSE(accountInfo.mIsDeleted);
  EXPECT_EQ(accountInfo.mKey->toBytes(), testKey->getPublicKey()->toBytes());
  EXPECT_EQ(accountInfo.mBalance, Hbar(0LL));
  EXPECT_EQ(accountInfo.mAutoRenewPeriod, DEFAULT_AUTO_RENEW_PERIOD);
  EXPECT_EQ(accountInfo.mProxyReceived, Hbar(0LL));

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, AliasFromAdminKey)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> adminPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PublicKey> adminPublicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(adminPrivateKey->getPublicKey());
  const EvmAddress evmAddress = adminPublicKey->toEvmAddress();

  AccountId adminAccountId;
  ASSERT_NO_THROW(adminAccountId = AccountCreateTransaction()
                                     .setKeyWithoutAlias(adminPublicKey)
                                     .execute(getTestClient())
                                     .getReceipt(getTestClient())
                                     .mAccountId.value());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse =
                    AccountCreateTransaction().setKey(adminPublicKey).setAlias(evmAddress).execute(getTestClient()));

  // Then
  AccountId accountId;
  AccountInfo accountInfo;
  ASSERT_NO_THROW(accountId = txResponse.getReceipt(getTestClient()).mAccountId.value());
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_EQ(internal::HexConverter::hexToBytes(accountInfo.mContractAccountId), evmAddress.toBytes());
  EXPECT_EQ(accountInfo.mKey->toBytes(), adminPublicKey->toBytes());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(adminAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminPrivateKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, AliasFromAdminKeyWithReceiverSigRequired)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> adminKeyPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PublicKey> adminKeyPublicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(adminKeyPrivateKey->getPublicKey());
  const EvmAddress evmAddress = adminKeyPublicKey->toEvmAddress();

  AccountId adminAccountId;
  ASSERT_NO_THROW(adminAccountId = AccountCreateTransaction()
                                     .setKeyWithoutAlias(adminKeyPublicKey)
                                     .execute(getTestClient())
                                     .getReceipt(getTestClient())
                                     .mAccountId.value());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setReceiverSignatureRequired(true)
                                 .setKey(adminKeyPublicKey)
                                 .setAlias(evmAddress)
                                 .freezeWith(&getTestClient())
                                 .sign(adminKeyPrivateKey)
                                 .execute(getTestClient()));

  // Then
  AccountId accountId;
  AccountInfo accountInfo;
  ASSERT_NO_THROW(accountId = txResponse.getReceipt(getTestClient()).mAccountId.value());
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_EQ(internal::HexConverter::hexToBytes(accountInfo.mContractAccountId), evmAddress.toBytes());
  EXPECT_EQ(accountInfo.mKey->toBytes(), adminKeyPublicKey->toBytes());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(adminAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminKeyPrivateKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminKeyPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CannotCreateAliasFromAdminKeyWithReceiverSigRequiredWithoutSignature)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> adminKeyPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PublicKey> adminKeyPublicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(adminKeyPrivateKey->getPublicKey());
  const EvmAddress evmAddress = adminKeyPublicKey->toEvmAddress();

  AccountId adminAccountId;
  ASSERT_NO_THROW(adminAccountId = AccountCreateTransaction()
                                     .setKeyWithoutAlias(adminKeyPublicKey)
                                     .execute(getTestClient())
                                     .getReceipt(getTestClient())
                                     .mAccountId.value());

  // When
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountCreateTransaction()
                                                      .setReceiverSignatureRequired(true)
                                                      .setKey(adminKeyPublicKey)
                                                      .setAlias(evmAddress)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(adminAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminKeyPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, AliasDifferentFromAdminKeyWithReceiverSigRequired)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminPrivateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId adminAccountId;
  ASSERT_NO_THROW(adminAccountId = AccountCreateTransaction()
                                     .setKeyWithoutAlias(adminPrivateKey->getPublicKey())
                                     .execute(getTestClient())
                                     .getReceipt(getTestClient())
                                     .mAccountId.value());

  const std::shared_ptr<ECDSAsecp256k1PrivateKey> aliasPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const EvmAddress alias =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(aliasPrivateKey->getPublicKey())->toEvmAddress();

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setReceiverSignatureRequired(true)
                                 .setKey(adminPrivateKey->getPublicKey())
                                 .setAlias(alias)
                                 .freezeWith(&getTestClient())
                                 .sign(adminPrivateKey)
                                 .sign(aliasPrivateKey)
                                 .execute(getTestClient()));

  // Then
  AccountId accountId;
  AccountInfo accountInfo;
  ASSERT_NO_THROW(accountId = txResponse.getReceipt(getTestClient()).mAccountId.value());
  ASSERT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  EXPECT_EQ(accountInfo.mAccountId, accountId);
  EXPECT_EQ(internal::HexConverter::hexToBytes(accountInfo.mContractAccountId), alias.toBytes());
  EXPECT_EQ(accountInfo.mKey->toBytes(), adminPrivateKey->getPublicKey()->toBytes());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(adminAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminPrivateKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests,
       CannotCreateWithAliasDifferentFromAdminKeyWithReceiverSigRequiredWithoutSignature)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> adminPrivateKey = ED25519PrivateKey::generatePrivateKey();
  AccountId adminAccountId;
  ASSERT_NO_THROW(adminAccountId = AccountCreateTransaction()
                                     .setKeyWithoutAlias(adminPrivateKey->getPublicKey())
                                     .execute(getTestClient())
                                     .getReceipt(getTestClient())
                                     .mAccountId.value());

  const std::shared_ptr<ECDSAsecp256k1PrivateKey> aliasPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const EvmAddress alias =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(aliasPrivateKey->getPublicKey())->toEvmAddress();

  // When
  EXPECT_THROW(const TransactionReceipt txReceipt = AccountCreateTransaction()
                                                      .setReceiverSignatureRequired(true)
                                                      .setKey(adminPrivateKey->getPublicKey())
                                                      .setAlias(alias)
                                                      .freezeWith(&getTestClient())
                                                      .sign(aliasPrivateKey)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(adminAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(adminPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, SerializeDeserializeCompareFields)
{
  // Given
  const std::shared_ptr<PrivateKey> testPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> testPublicKey = testPrivateKey->getPublicKey();

  const TransactionType expectedType = TransactionType::ACCOUNT_CREATE_TRANSACTION;

  AccountCreateTransaction createAccount;
  ASSERT_NO_THROW(createAccount =
                    AccountCreateTransaction().setKeyWithoutAlias(testPublicKey).setInitialBalance(Hbar(5LL)););

  const int expectedNodeAccountIdsSize = createAccount.getNodeAccountIds().size();
  const Hbar expectedBalance = Hbar(5LL);

  // When
  std::vector<std::byte> transactionBytesSerialized;
  ASSERT_NO_THROW(transactionBytesSerialized = createAccount.toBytes(););

  WrappedTransaction wrappedTransaction;
  ASSERT_NO_THROW(wrappedTransaction = Transaction<AccountCreateTransaction>::fromBytes(transactionBytesSerialized););
  createAccount = *wrappedTransaction.getTransaction<AccountCreateTransaction>();

  // Then
  EXPECT_EQ(expectedType, wrappedTransaction.getTransactionType());
  EXPECT_EQ(expectedNodeAccountIdsSize, createAccount.getNodeAccountIds().size());
  EXPECT_THROW(const auto txId = createAccount.getTransactionId(), UninitializedException);
  EXPECT_EQ(expectedBalance, createAccount.getInitialBalance());
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, SerializeDeserializeEditCompareFields)
{
  // Given
  const std::shared_ptr<PrivateKey> testPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> testPublicKey = testPrivateKey->getPublicKey();

  AccountCreateTransaction createAccount;
  ASSERT_NO_THROW(createAccount = AccountCreateTransaction().setKeyWithoutAlias(testPublicKey));

  const Hbar expectedBalance = Hbar(5LL);

  // When
  std::vector<std::byte> transactionBytesSerialized;
  ASSERT_NO_THROW(transactionBytesSerialized = createAccount.toBytes(););

  WrappedTransaction wrappedTransaction;
  ASSERT_NO_THROW(wrappedTransaction = Transaction<AccountCreateTransaction>::fromBytes(transactionBytesSerialized););
  createAccount = *wrappedTransaction.getTransaction<AccountCreateTransaction>();

  // execute with altered transaction fields
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt =
                    createAccount.setInitialBalance(Hbar(5LL))
                      .setTransactionId(TransactionId::generate(getTestClient().getOperatorAccountId().value()))
                      .execute(getTestClient())
                      .getReceipt(getTestClient()););

  // Then
  EXPECT_EQ(expectedBalance, createAccount.getInitialBalance());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(txReceipt.mAccountId.value())
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, IncompleteSerializeDeserializeAndExecute)
{
  // Given
  const std::shared_ptr<PrivateKey> testPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> testPublicKey = testPrivateKey->getPublicKey();

  AccountCreateTransaction createAccount;
  ASSERT_NO_THROW(createAccount =
                    AccountCreateTransaction().setKeyWithoutAlias(testPublicKey).setInitialBalance(Hbar(5LL)););
  // When // Then
  std::vector<std::byte> transactionBytes;
  ASSERT_NO_THROW(transactionBytes = createAccount.toBytes(););

  WrappedTransaction wrappedTransaction;
  ASSERT_NO_THROW(wrappedTransaction = Transaction<AccountCreateTransaction>::fromBytes(transactionBytes));

  createAccount = *wrappedTransaction.getTransaction<AccountCreateTransaction>();

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = createAccount.execute(getTestClient()).getReceipt(getTestClient()););
  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(txReceipt.mAccountId.value())
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, FreezeSignSerializeDeserializeAndExecute)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> testPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PublicKey> testPublicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(testPrivateKey->getPublicKey());
  const EvmAddress testEvmAddress = testPublicKey->toEvmAddress();
  const Hbar testInitialBalance(1000LL, HbarUnit::TINYBAR());
  const std::chrono::system_clock::duration testAutoRenewPeriod = std::chrono::seconds(2592000);
  const std::string testMemo = "test account memo";
  const int32_t testMaxAutomaticTokenAssociations = 4;

  AccountCreateTransaction createAccount;
  EXPECT_NO_THROW(createAccount = AccountCreateTransaction()
                                    .setKey(testPublicKey)
                                    .setInitialBalance(testInitialBalance)
                                    .setReceiverSignatureRequired(true)
                                    .setAutoRenewPeriod(testAutoRenewPeriod)
                                    .setAccountMemo(testMemo)
                                    .setMaxAutomaticTokenAssociations(testMaxAutomaticTokenAssociations)
                                    .setDeclineStakingReward(true)
                                    .setAlias(testEvmAddress)
                                    .freezeWith(&getTestClient())
                                    .sign(testPrivateKey));
  // When
  std::vector<std::byte> transactionBytesSerialized;
  ASSERT_NO_THROW(transactionBytesSerialized = createAccount.toBytes(););

  WrappedTransaction wrappedTransaction;
  ASSERT_NO_THROW(wrappedTransaction = Transaction<AccountCreateTransaction>::fromBytes(transactionBytesSerialized););
  createAccount = *wrappedTransaction.getTransaction<AccountCreateTransaction>();

  std::vector<std::byte> transactionBytesReserialized;
  ASSERT_NO_THROW(transactionBytesReserialized = createAccount.toBytes(););

  // Then
  EXPECT_EQ(transactionBytesSerialized, transactionBytesReserialized);

  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = createAccount.execute(getTestClient()););

  AccountId accountId;
  ASSERT_NO_THROW(accountId = txResponse.getReceipt(getTestClient()).mAccountId.value());

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(accountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testPrivateKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CannotCreateAccountWithLessThanNegativeOneAutomaticTokenAssociations)
{
  // Given
  const std::shared_ptr<PrivateKey> key = ED25519PrivateKey::generatePrivateKey();

  // When / Then
  EXPECT_THROW(AccountCreateTransaction()
                 .setKeyWithoutAlias(key)
                 .setMaxAutomaticTokenAssociations(-2)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_MAX_AUTO_ASSOCIATIONS
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CreateTransactionWithAliasCannotExecuteWithoutBothSignatures)
{
  // Given
  const std::shared_ptr<PrivateKey> edPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> ecdsaPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const Hbar testInitialBalance(1000LL, HbarUnit::TINYBAR());

  // When / Then
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithAlias(edPrivateKey->getPublicKey(), ecdsaPrivateKey)
                                 .setInitialBalance(testInitialBalance)
                                 .freezeWith(&getTestClient())
                                 .sign(edPrivateKey)
                                 .execute(getTestClient()));

  EXPECT_THROW(txResponse.getReceipt(getTestClient()), ReceiptStatusException); // INVALID_SIGNATURE;
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CreateTransactionWithAliasCanExecuteWithoutBothSignatures)
{
  // Given
  const std::shared_ptr<PrivateKey> edPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> ecdsaPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<ECDSAsecp256k1PublicKey> ecdsaPublicKey =
    std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(ecdsaPrivateKey->getPublicKey());
  const EvmAddress expectedEvmAddress = ecdsaPublicKey->toEvmAddress();
  const Hbar testInitialBalance(1000LL, HbarUnit::TINYBAR());

  // When / Then
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithAlias(edPrivateKey->getPublicKey(), ecdsaPrivateKey)
                                 .setInitialBalance(testInitialBalance)
                                 .freezeWith(&getTestClient())
                                 .sign(edPrivateKey)
                                 .sign(ecdsaPrivateKey)
                                 .execute(getTestClient()));

  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  ASSERT_EQ(txReceipt.mAccountId.has_value(), true);
  AccountId accountId = txReceipt.mAccountId.value();

  AccountInfo accountInfo;
  EXPECT_NO_THROW(accountInfo = AccountInfoQuery().setAccountId(accountId).execute(getTestClient()));

  std::string evmAddressString = accountInfo.mContractAccountId;
  std::transform(evmAddressString.begin(), evmAddressString.end(), evmAddressString.begin(), ::toupper);
  EXPECT_EQ(evmAddressString, expectedEvmAddress.toString());
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CreateTransactionWithAliasAndKeyECDSACanExecute)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> ecdsaPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const Hbar testInitialBalance(1000LL, HbarUnit::TINYBAR());

  // When / Then
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setECDSAKeyWithAlias(ecdsaPrivateKey)
                                 .setInitialBalance(testInitialBalance)
                                 .freezeWith(&getTestClient())
                                 .sign(ecdsaPrivateKey)
                                 .execute(getTestClient()));

  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  ASSERT_EQ(txReceipt.mAccountId.has_value(), true);
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CreateTransactionWithLambdaHook)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> ecdsaPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithoutAlias(ecdsaPrivateKey)
                                 .addHook(hookCreationDetails)
                                 .freezeWith(&getTestClient())
                                 .sign(ecdsaPrivateKey)
                                 .execute(getTestClient()));

  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  EXPECT_TRUE(txReceipt.mAccountId.has_value());
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CreateTransactionWithLambdaHookAndStorageUpdates)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> ecdsaPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

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
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction()
                                 .setKeyWithoutAlias(ecdsaPrivateKey)
                                 .addHook(hookCreationDetails)
                                 .freezeWith(&getTestClient())
                                 .sign(ecdsaPrivateKey)
                                 .execute(getTestClient()));

  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));

  EXPECT_TRUE(txReceipt.mAccountId.has_value());
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CreateTransactionWithLambdaHookWithNoContractId)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> ecdsaPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

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
  EXPECT_THROW(AccountCreateTransaction()
                 .setKeyWithoutAlias(ecdsaPrivateKey)
                 .addHook(hookCreationDetails)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_HOOK_CREATION_SPEC
}

//-----
TEST_F(AccountCreateTransactionIntegrationTests, CreateTransactionWithSameLambdaHookIds)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> ecdsaPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  LambdaEvmHook lambdaEvmHook;
  EvmHookSpec evmHookSpec;
  evmHookSpec.setContractId(getTestContractId());
  lambdaEvmHook.setEvmHookSpec(evmHookSpec);

  HookCreationDetails hookCreationDetails;
  hookCreationDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK);
  hookCreationDetails.setHookId(1LL);
  hookCreationDetails.setLambdaEvmHook(lambdaEvmHook);

  // When / Then
  EXPECT_THROW(AccountCreateTransaction()
                 .setKeyWithoutAlias(ecdsaPrivateKey)
                 .addHook(hookCreationDetails)
                 .addHook(hookCreationDetails)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_MAX_AUTO_ASSOCIATIONS
}