// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "BatchTransaction.h"
#include "ED25519PrivateKey.h"
#include "FileId.h"
#include "FreezeTransaction.h"
#include "Hbar.h"
#include "TopicCreateTransaction.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "WrappedTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class BatchTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(BatchTransactionIntegrationTests, CanExecuteBatchTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  WrappedTransaction innerTransaction(AccountCreateTransaction()
                                        .setKeyWithoutAlias(accountKey)
                                        .setInitialBalance(Hbar(1))
                                        .batchify(getTestClient(), operatorKey));

  // When / Then
  BatchTransaction batchTransaction = BatchTransaction().addInnerTransaction(innerTransaction);

  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = batchTransaction.execute(getTestClient()););

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()););
}

//-----
TEST_F(BatchTransactionIntegrationTests, CanExecuteAfterFromToBytes)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  WrappedTransaction innerTransaction(AccountCreateTransaction()
                                        .setKeyWithoutAlias(accountKey)
                                        .setInitialBalance(Hbar(1))
                                        .batchify(getTestClient(), operatorKey));

  // When
  BatchTransaction batchTransaction = BatchTransaction().addInnerTransaction(innerTransaction);

  std::vector<std::byte> bytes = batchTransaction.toBytes();

  WrappedTransaction wrappedTransaction;
  ASSERT_NO_THROW(wrappedTransaction = Transaction<BatchTransaction>::fromBytes(bytes));

  // Then
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = wrappedTransaction.getTransaction<BatchTransaction>()->execute(getTestClient()););

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()););
}

//-----
TEST_F(BatchTransactionIntegrationTests, CanExecuteLargeBatchTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  // When
  BatchTransaction batchTransaction = BatchTransaction();

  for (int i = 0; i < 25; ++i)
  {
    std::shared_ptr<PrivateKey> accountKey;
    ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

    WrappedTransaction innerTransaction(AccountCreateTransaction()
                                          .setKeyWithoutAlias(accountKey)
                                          .setInitialBalance(Hbar(1))
                                          .batchify(getTestClient(), operatorKey));

    batchTransaction.addInnerTransaction(innerTransaction);
  }

  // Then
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = batchTransaction.execute(getTestClient()););

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()););
}

//-----
TEST_F(BatchTransactionIntegrationTests, CannotExecuteBatchTransactionWithoutNoInnerTransactions)
{
  // Given /When / Then
  TransactionResponse txResponse;
  ASSERT_THROW(txResponse = BatchTransaction().execute(getTestClient()), PrecheckStatusException);
}

//-----
TEST_F(BatchTransactionIntegrationTests, CannotExecuteBatchTransactionWithBlacklistedTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  WrappedTransaction innerTransaction(FreezeTransaction()
                                        .setFileId(FileId())
                                        .setStartTime(std::chrono::system_clock::now() + std::chrono::seconds(30))
                                        .setFreezeType(FreezeType::FREEZE_ONLY)
                                        .batchify(getTestClient(), operatorKey));

  // When / Then
  TransactionReceipt txReceipt;
  ASSERT_THROW(
    txReceipt =
      BatchTransaction().addInnerTransaction(innerTransaction).execute(getTestClient()).getReceipt(getTestClient()),
    ReceiptStatusException);
}

//-----
TEST_F(BatchTransactionIntegrationTests, CannotExecuteBatchTransactionWithInvalidInnerBatchKey)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  WrappedTransaction innerTransaction(AccountCreateTransaction()
                                        .setKeyWithoutAlias(accountKey)
                                        .setInitialBalance(Hbar(1))
                                        .batchify(getTestClient(), accountKey));

  // When / Then
  TransactionReceipt txReceipt;
  ASSERT_THROW(
    txReceipt =
      BatchTransaction().addInnerTransaction(innerTransaction).execute(getTestClient()).getReceipt(getTestClient()),
    ReceiptStatusException);
}

//-----
TEST_F(BatchTransactionIntegrationTests, CannotExecuteBatchTransactionWithoutBatchifyingInner)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  WrappedTransaction innerTransaction(
    AccountCreateTransaction().setKeyWithoutAlias(accountKey).setInitialBalance(Hbar(1)));

  // When / Then
  TransactionReceipt txReceipt;
  ASSERT_ANY_THROW(
    txReceipt =
      BatchTransaction().addInnerTransaction(innerTransaction).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(BatchTransactionIntegrationTests, CanExecuteBatchTransactionWithChunkedInner)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TopicId topicId;
  ASSERT_NO_THROW(topicId = TopicCreateTransaction()
                              .setAdminKey(operatorKey)
                              .setMemo("testMemo")
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTopicId.value());

  WrappedTransaction innerTransaction(TopicMessageSubmitTransaction()
                                        .setTopicId(topicId)
                                        .setMessage(getTestBigContents())
                                        .batchify(getTestClient(), operatorKey));

  // Then
  BatchTransaction batchTransaction = BatchTransaction().addInnerTransaction(innerTransaction);

  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = batchTransaction.execute(getTestClient()););

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()););
}