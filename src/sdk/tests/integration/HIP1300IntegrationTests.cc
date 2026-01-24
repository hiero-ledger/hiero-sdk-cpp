// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "Hbar.h"
#include "PrivateKey.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class HIP1300IntegrationTests : public BaseIntegrationTest
{
protected:
  // Target size just above the 6KB limit that triggers TRANSACTION_OVERSIZE for non-system accounts
  // Using 7KB (7168 bytes) - enough to exceed the limit without generating thousands of keys
  static constexpr size_t MAXIMUM_TRANSACTION_SIZE = 7168;
  // Approximate size of a single ED25519 signature pair in the transaction
  // (signature: 64 bytes + public key prefix + protobuf overhead ≈ 100 bytes)
  static constexpr size_t SIGNATURE_SIZE = 100;
};

//-----
TEST_F(HIP1300IntegrationTests, CanCreateTransactionWithMoreThan6KBDataWithSignaturesUsingSystemAccount)
{
  // Given
  const std::shared_ptr<PrivateKey> testKey = ED25519PrivateKey::generatePrivateKey();

  // Generate all keys upfront to avoid calling getTransactionSize() in a loop
  // which causes duplicate signature issues in the SDK
  const size_t numSignatures = (MAXIMUM_TRANSACTION_SIZE / SIGNATURE_SIZE) + 10;
  std::vector<std::shared_ptr<PrivateKey>> signingKeys;
  signingKeys.reserve(numSignatures);
  for (size_t i = 0; i < numSignatures; ++i)
  {
    signingKeys.push_back(ED25519PrivateKey::generatePrivateKey());
  }

  // When
  // Create and freeze the transaction
  AccountCreateTransaction transaction = AccountCreateTransaction().setKeyWithoutAlias(testKey->getPublicKey());
  transaction.freezeWith(&getTestClient());

  // Sign with all the pre-generated keys
  for (const auto& key : signingKeys)
  {
    transaction.sign(key);
  }

  // Note: We don't call getTransactionSize() here because it triggers buildTransaction(),
  // and then execute() adds the operator signature and builds again, causing duplicate
  // signatures in the sigmap (KEY_PREFIX_MISMATCH error). We trust that numSignatures
  // keys will exceed the 6KB limit.

  // Then
  // Execute the transaction - it should succeed with a system account
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = transaction.execute(getTestClient()).getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_TRUE(txReceipt.mAccountId.has_value());

  // Clean up - delete the created account
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(txReceipt.mAccountId.value())
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(testKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
}

//-----
TEST_F(HIP1300IntegrationTests, CanCreateTransactionWithMoreThan6KBFileDataUsingSystemAccount)
{
  // Given
  // Create 10KB of file content (more than 6KB limit)
  const std::vector<std::byte> fileContents(1024 * 10, std::byte{ 0x01 });

  // When / Then
  // Create the file - it should succeed with a system account (getTestClient uses 0.0.2)
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = FileCreateTransaction()
                                .setContents(fileContents)
                                .freezeWith(&getTestClient())
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);
  EXPECT_TRUE(txReceipt.mFileId.has_value());
}

//-----
TEST_F(HIP1300IntegrationTests, CannotCreateTransactionWithMoreThan6KBFileDataWithNormalAccount)
{
  // Given
  // First create a regular (non-system) account to use as the operator
  const std::shared_ptr<PrivateKey> regularAccountKey = ED25519PrivateKey::generatePrivateKey();

  TransactionReceipt createReceipt;
  ASSERT_NO_THROW(createReceipt = AccountCreateTransaction()
                                    .setKeyWithoutAlias(regularAccountKey->getPublicKey())
                                    .setInitialBalance(Hbar(10LL))
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient()));
  ASSERT_TRUE(createReceipt.mAccountId.has_value());

  const AccountId regularAccountId = createReceipt.mAccountId.value();

  // Create a client with the regular account as operator
  Client regularClient = Client::fromConfigFile((std::filesystem::current_path() / "local_node.json").string());
  regularClient.setOperator(regularAccountId, regularAccountKey);

  // Create 10KB of file content (more than 6KB limit)
  const std::vector<std::byte> fileContents(1024 * 10, std::byte{ 0x01 });

  // When / Then
  // Try to create the file - it should fail with TRANSACTION_OVERSIZE for a normal account
  try
  {
    FileCreateTransaction()
      .setContents(fileContents)
      .freezeWith(&regularClient)
      .execute(regularClient)
      .getReceipt(regularClient);
    FAIL() << "Expected ReceiptStatusException with TRANSACTION_OVERSIZE";
  }
  catch (const PrecheckStatusException& ex)
  {
    EXPECT_EQ(ex.mStatus, Status::TRANSACTION_OVERSIZE);
  }

  // Clean up - delete the regular account
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(regularAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(regularAccountKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
}

//-----
TEST_F(HIP1300IntegrationTests, CannotCreateTransactionWithMoreThan6KBSignaturesWithoutSystemAccount)
{
  // Given
  // First create a regular (non-system) account to use as the operator
  const std::shared_ptr<PrivateKey> regularAccountKey = ED25519PrivateKey::generatePrivateKey();

  TransactionReceipt createReceipt;
  ASSERT_NO_THROW(createReceipt = AccountCreateTransaction()
                                    .setKeyWithoutAlias(regularAccountKey->getPublicKey())
                                    .setInitialBalance(Hbar(10LL))
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient()));
  ASSERT_TRUE(createReceipt.mAccountId.has_value());

  const AccountId regularAccountId = createReceipt.mAccountId.value();

  // Create a client with the regular account as operator
  Client regularClient = Client::fromConfigFile((std::filesystem::current_path() / "local_node.json").string());
  regularClient.setOperator(regularAccountId, regularAccountKey);

  const std::shared_ptr<PrivateKey> testKey = ED25519PrivateKey::generatePrivateKey();

  // Generate all keys upfront to avoid calling getTransactionSize() in a loop
  // which causes duplicate signature issues in the SDK
  const size_t numSignatures = (MAXIMUM_TRANSACTION_SIZE / SIGNATURE_SIZE) + 10;
  std::vector<std::shared_ptr<PrivateKey>> signingKeys;
  signingKeys.reserve(numSignatures);
  for (size_t i = 0; i < numSignatures; ++i)
  {
    signingKeys.push_back(ED25519PrivateKey::generatePrivateKey());
  }

  // Create and freeze the transaction with the regular (non-system) client
  AccountCreateTransaction transaction = AccountCreateTransaction().setKeyWithoutAlias(testKey->getPublicKey());
  transaction.freezeWith(&regularClient);

  // Sign with all the pre-generated keys
  for (const auto& key : signingKeys)
  {
    transaction.sign(key);
  }

  // Note: We don't call getTransactionSize() here because it triggers buildTransaction(),
  // and then execute() adds the operator signature and builds again, causing duplicate
  // signatures in the sigmap (KEY_PREFIX_MISMATCH error).

  // When / Then
  // Try to execute - it should fail with TRANSACTION_OVERSIZE for a normal account
  try
  {
    transaction.execute(regularClient).getReceipt(regularClient);
    FAIL() << "Expected ReceiptStatusException with TRANSACTION_OVERSIZE";
  }
  catch (const PrecheckStatusException& ex)
  {
    EXPECT_EQ(ex.mStatus, Status::TRANSACTION_OVERSIZE);
  }

  // Clean up - delete the regular account
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(regularAccountId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(regularAccountKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
}

