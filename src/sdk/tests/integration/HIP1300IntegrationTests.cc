// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "PrivateKey.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>

using namespace Hiero;

class HIP1300IntegrationTests : public BaseIntegrationTest
{
protected:
  static constexpr size_t MAXIMUM_TRANSACTION_SIZE = 130000;

  [[nodiscard]] inline const AccountId& getSystemAccountId() const { return mSystemAccountId; }
  [[nodiscard]] inline const std::shared_ptr<PrivateKey>& getSystemAccountKey() const { return mSystemAccountKey; }

private:
  const AccountId mSystemAccountId = AccountId::fromString("0.0.2");
  const std::shared_ptr<PrivateKey> mSystemAccountKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
};

//-----
TEST_F(HIP1300IntegrationTests, CanCreateTransactionWithMoreThan6KBDataWithSignaturesUsingSystemAccount)
{
  // Given
  // Create a client with the system account as operator
  Client systemClient = Client::fromConfigFile((std::filesystem::current_path() / "local_node.json").string());
  systemClient.setOperator(getSystemAccountId(), getSystemAccountKey());
  systemClient.setNetworkUpdatePeriod(std::chrono::seconds(360));

  const std::shared_ptr<PrivateKey> testKey = ED25519PrivateKey::generatePrivateKey();

  // When
  // Create and freeze the transaction
  AccountCreateTransaction transaction = AccountCreateTransaction().setKeyWithoutAlias(testKey->getPublicKey());
  transaction.freezeWith(&systemClient);

  // Sign with multiple keys until we exceed the maximum transaction size
  while (transaction.getTransactionSize() < MAXIMUM_TRANSACTION_SIZE)
  {
    transaction.sign(ED25519PrivateKey::generatePrivateKey());
  }

  // Then
  // Execute the transaction - it should succeed with a system account
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = transaction.execute(systemClient).getReceipt(systemClient));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_TRUE(txReceipt.mAccountId.has_value());

  // Clean up - delete the created account
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(txReceipt.mAccountId.value())
                    .setTransferAccountId(getSystemAccountId())
                    .freezeWith(&systemClient)
                    .sign(testKey)
                    .execute(systemClient)
                    .getReceipt(systemClient));
}

//-----
TEST_F(HIP1300IntegrationTests, CanCreateTransactionWithMoreThan6KBFileDataUsingSystemAccount)
{
  // Given
  // Create a client with the system account as operator
  Client systemClient = Client::fromConfigFile((std::filesystem::current_path() / "local_node.json").string());
  systemClient.setOperator(getSystemAccountId(), getSystemAccountKey());
  systemClient.setNetworkUpdatePeriod(std::chrono::seconds(360));

  // Create 10KB of file content (more than 6KB limit)
  const std::vector<std::byte> fileContents(1024 * 10, std::byte{ 0x01 });

  // When / Then
  // Create the file - it should succeed with a system account
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = FileCreateTransaction()
                                .setContents(fileContents)
                                .freezeWith(&systemClient)
                                .execute(systemClient)
                                .getReceipt(systemClient));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_TRUE(txReceipt.mFileId.has_value());
}

//-----
TEST_F(HIP1300IntegrationTests, CannotCreateTransactionWithMoreThan6KBFileDataWithNormalAccount)
{
  // Given
  // Create 10KB of file content (more than 6KB limit)
  const std::vector<std::byte> fileContents(1024 * 10, std::byte{ 0x01 });

  // When / Then
  // Try to create the file - it should fail with TRANSACTION_OVERSIZE for a normal account
  try
  {
    FileCreateTransaction()
      .setContents(fileContents)
      .freezeWith(&getTestClient())
      .execute(getTestClient())
      .getReceipt(getTestClient());
    FAIL() << "Expected ReceiptStatusException with TRANSACTION_OVERSIZE";
  }
  catch (const ReceiptStatusException& ex)
  {
    EXPECT_EQ(ex.mStatus, Status::TRANSACTION_OVERSIZE);
  }
}

//-----
TEST_F(HIP1300IntegrationTests, CannotCreateTransactionWithMoreThan6KBSignaturesWithoutSystemAccount)
{
  // Given
  const std::shared_ptr<PrivateKey> testKey = ED25519PrivateKey::generatePrivateKey();

  // Create and freeze the transaction
  AccountCreateTransaction transaction = AccountCreateTransaction().setKeyWithoutAlias(testKey->getPublicKey());
  transaction.freezeWith(&getTestClient());

  // Sign with multiple keys until we exceed the maximum transaction size
  while (transaction.getTransactionSize() < MAXIMUM_TRANSACTION_SIZE)
  {
    transaction.sign(ED25519PrivateKey::generatePrivateKey());
  }

  // When / Then
  // Try to execute - it should fail with TRANSACTION_OVERSIZE for a normal account
  try
  {
    transaction.execute(getTestClient()).getReceipt(getTestClient());
    FAIL() << "Expected ReceiptStatusException with TRANSACTION_OVERSIZE";
  }
  catch (const ReceiptStatusException& ex)
  {
    EXPECT_EQ(ex.mStatus, Status::TRANSACTION_OVERSIZE);
  }
}

