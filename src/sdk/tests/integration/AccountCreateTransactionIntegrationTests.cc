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
#include <transaction_body.pb.h>

using namespace Hiero;

class AccountCreateTransactionIntegrationTests : public BaseIntegrationTest
{
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
                                 .setKey(testPublicKey)
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
                                                .setKey(testPublicKey)
                                                .setStakedAccountId(operatorAccountId)
                                                .freezeWith(&getTestClient())
                                                .execute(getTestClient()));
  EXPECT_NO_THROW(txResponseStakedNodeId = AccountCreateTransaction()
                                             .setKey(testPublicKey)
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
  EXPECT_NO_THROW(txResponse = AccountCreateTransaction().setKey(testKey->getPublicKey()).execute(getTestClient()));

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
                                     .setKey(adminPublicKey)
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
                                     .setKey(adminKeyPublicKey)
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
                                     .setKey(adminKeyPublicKey)
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
                                     .setKey(adminPrivateKey->getPublicKey())
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
                                     .setKey(adminPrivateKey->getPublicKey())
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
  ASSERT_NO_THROW(createAccount = AccountCreateTransaction().setKey(testPublicKey).setInitialBalance(Hbar(5LL)););

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
  ASSERT_NO_THROW(createAccount = AccountCreateTransaction().setKey(testPublicKey));

  const Hbar expectedBalance = Hbar(5LL);
  std::vector<AccountId> nodeAccountIds = getTestClient().getClientNetwork()->getNodeAccountIdsForExecute();

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
                      .setNodeAccountIds(nodeAccountIds) // will fail if nodeAccountIds are bad so not checking equality
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
  ASSERT_NO_THROW(createAccount = AccountCreateTransaction().setKey(testPublicKey).setInitialBalance(Hbar(5LL)););
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
                 .setKey(key)
                 .setMaxAutomaticTokenAssociations(-2)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               PrecheckStatusException); // INVALID_MAX_AUTO_ASSOCIATIONS
}
