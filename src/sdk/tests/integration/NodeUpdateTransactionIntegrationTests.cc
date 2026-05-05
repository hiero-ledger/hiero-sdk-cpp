// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AddressBookQuery.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "FileId.h"
#include "Hbar.h"
#include "NodeAddress.h"
#include "NodeAddressBook.h"
#include "NodeUpdateTransaction.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <thread>

using namespace Hiero;

class NodeUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] const uint64_t& getNodeId() const { return mNodeId; }

private:
  const uint64_t mNodeId = 1; // Used by other tests; CanExecuteNodeUpdateTransaction uses 0ULL
};

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, CanExecuteNodeUpdateTransaction)
{
  // Given
  // Use a client targeted at node 0 (account 0.0.3) to match the reference
  // implementations in the Java and Swift SDKs.
  std::unordered_map<std::string, AccountId> network;
  network["localhost:50211"] = AccountId(3ULL);
  Client client = Client::forNetwork(network);
  client.setMirrorNetwork({ "localhost:5600" });
  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  NodeAddressBook addressBook = AddressBookQuery().setFileId(FileId::ADDRESS_BOOK).execute(client);
  const auto& nodeAddresses = addressBook.getNodeAddresses();
  const auto nodeIt = std::find_if(
    nodeAddresses.begin(), nodeAddresses.end(), [](const NodeAddress& address) { return address.getNodeId() == 0ULL; });
  ASSERT_NE(nodeIt, nodeAddresses.end());
  const std::string originalDescription = nodeIt->getDescription();
  const std::string updatedDescription =
    originalDescription.empty() ? "SDK test update" : originalDescription + " (updated)";

  // When / Then
  TransactionResponse txResponse;

  ASSERT_NO_THROW(txResponse =
                    NodeUpdateTransaction().setNodeId(0ULL).setDescription(updatedDescription).execute(client));

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(client));

  // Clean up
  ASSERT_NO_THROW(
    txReceipt =
      NodeUpdateTransaction().setNodeId(0ULL).setDescription(originalDescription).execute(client).getReceipt(client));
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, CanChangeNodeAccountIdToTheSameAccount)
{
  // Given
  // Set up the network
  std::unordered_map<std::string, AccountId> network;
  network["localhost:51211"] = AccountId(4ULL);

  // Create client for the network
  Client client = Client::forNetwork(network);

  // Set mirror network
  std::vector<std::string> mirrorNetwork = { "localhost:5600" };
  client.setMirrorNetwork(mirrorNetwork);

  // Set the operator to be account 0.0.2
  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  const AccountId originalNodeAccountId = AccountId::fromString("0.0.4");

  // When
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse =
                    NodeUpdateTransaction().setNodeId(getNodeId()).setAccountId(originalNodeAccountId).execute(client));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.setValidateStatus(true).getReceipt(client));
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, ChangeNodeAccountIdMissingAdminSig)
{
  // Given - Set up the network
  std::unordered_map<std::string, AccountId> network;
  network["localhost:51211"] = AccountId(4ULL);
  Client client = Client::forNetwork(network);

  std::vector<std::string> mirrorNetwork = { "localhost:5600" };
  client.setMirrorNetwork(mirrorNetwork);

  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  // Create new operator account
  std::shared_ptr<PrivateKey> newOperatorKey = ED25519PrivateKey::generatePrivateKey();
  Hbar newBalance(2LL);

  TransactionResponse createResponse =
    AccountCreateTransaction().setKey(newOperatorKey->getPublicKey()).setInitialBalance(newBalance).execute(client);
  TransactionReceipt createReceipt = createResponse.setValidateStatus(true).getReceipt(client);
  AccountId operatorAccountId = createReceipt.mAccountId.value();

  client.setOperator(operatorAccountId, newOperatorKey);

  // When - Try to update without admin signature
  TransactionResponse updateResponse =
    NodeUpdateTransaction().setNodeId(getNodeId()).setAccountId(operatorAccountId).execute(client);

  // Then - Should fail with INVALID_SIGNATURE
  EXPECT_THROW({ updateResponse.setValidateStatus(true).getReceipt(client); }, ReceiptStatusException);
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, ChangeNodeAccountIdMissingAccountSig)
{
  // Given - Set up the network
  std::unordered_map<std::string, AccountId> network;
  network["localhost:51211"] = AccountId(4ULL);
  Client client = Client::forNetwork(network);

  std::vector<std::string> mirrorNetwork = { "localhost:5600" };
  client.setMirrorNetwork(mirrorNetwork);

  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  // Create new account
  std::shared_ptr<PrivateKey> newOperatorKey = ED25519PrivateKey::generatePrivateKey();
  Hbar newBalance(2LL);

  TransactionResponse createResponse =
    AccountCreateTransaction().setKey(newOperatorKey->getPublicKey()).setInitialBalance(newBalance).execute(client);
  TransactionReceipt createReceipt = createResponse.setValidateStatus(true).getReceipt(client);
  AccountId nodeAccountId = createReceipt.mAccountId.value();

  // When - Try to update without new account signature
  TransactionResponse updateResponse =
    NodeUpdateTransaction().setNodeId(getNodeId()).setAccountId(nodeAccountId).execute(client);

  // Then - Should fail with INVALID_SIGNATURE
  EXPECT_THROW({ updateResponse.setValidateStatus(true).getReceipt(client); }, ReceiptStatusException);
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, ChangeNodeAccountIdToNonExistentAccountId)
{
  // Given - Set up the network
  std::unordered_map<std::string, AccountId> network;
  network["localhost:51211"] = AccountId(4ULL);
  Client client = Client::forNetwork(network);

  std::vector<std::string> mirrorNetwork = { "localhost:5600" };
  client.setMirrorNetwork(mirrorNetwork);

  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  // When - Try to update to non-existent account
  TransactionResponse updateResponse =
    NodeUpdateTransaction().setNodeId(getNodeId()).setAccountId(AccountId(9999999ULL)).execute(client);

  // Then - Should fail with INVALID_SIGNATURE
  EXPECT_THROW({ updateResponse.setValidateStatus(true).getReceipt(client); }, ReceiptStatusException);
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, CanChangeNodeAccountIdToDeletedAccountId)
{
  // Given - Set up the network
  std::unordered_map<std::string, AccountId> network;
  network["localhost:51211"] = AccountId(4ULL);
  Client client = Client::forNetwork(network);

  std::vector<std::string> mirrorNetwork = { "localhost:5600" };
  client.setMirrorNetwork(mirrorNetwork);

  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  // Create account to be deleted
  std::shared_ptr<PrivateKey> newAccountKey = ED25519PrivateKey::generatePrivateKey();
  TransactionResponse createResponse = AccountCreateTransaction().setKey(newAccountKey->getPublicKey()).execute(client);
  TransactionReceipt createReceipt = createResponse.setValidateStatus(true).getReceipt(client);
  AccountId newAccount = createReceipt.mAccountId.value();

  // Delete the account
  AccountDeleteTransaction deleteTransaction = AccountDeleteTransaction()
                                                 .setDeleteAccountId(newAccount)
                                                 .setTransferAccountId(client.getOperatorAccountId().value())
                                                 .freezeWith(&client);
  TransactionResponse deleteResponse = deleteTransaction.sign(newAccountKey).execute(client);
  deleteResponse.setValidateStatus(true).getReceipt(client);

  // When - Try to update to deleted account
  NodeUpdateTransaction updateTransaction =
    NodeUpdateTransaction().setNodeId(getNodeId()).setAccountId(newAccount).freezeWith(&client);
  TransactionResponse updateResponse = updateTransaction.sign(newAccountKey).execute(client);

  // Then - Should fail with ACCOUNT_DELETED
  EXPECT_THROW({ updateResponse.setValidateStatus(true).getReceipt(client); }, ReceiptStatusException);
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, ChangeNodeAccountIdNoBalance)
{
  // Given - Set up the network
  std::unordered_map<std::string, AccountId> network;
  network["localhost:51211"] = AccountId(4ULL);
  Client client = Client::forNetwork(network);

  std::vector<std::string> mirrorNetwork = { "localhost:5600" };
  client.setMirrorNetwork(mirrorNetwork);

  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  // Create account with zero balance
  std::shared_ptr<PrivateKey> newAccountKey = ED25519PrivateKey::generatePrivateKey();
  TransactionResponse createResponse = AccountCreateTransaction().setKey(newAccountKey->getPublicKey()).execute(client);
  TransactionReceipt createReceipt = createResponse.setValidateStatus(true).getReceipt(client);
  AccountId newAccount = createReceipt.mAccountId.value();

  // When - Try to update to account with zero balance
  NodeUpdateTransaction updateTransaction =
    NodeUpdateTransaction().setNodeId(getNodeId()).setAccountId(newAccount).freezeWith(&client);
  TransactionResponse updateResponse = updateTransaction.sign(newAccountKey).execute(client);

  // Then - Should fail with NODE_ACCOUNT_HAS_ZERO_BALANCE
  EXPECT_THROW({ updateResponse.setValidateStatus(true).getReceipt(client); }, ReceiptStatusException);
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, CanChangeNodeAccountUpdateAddressbookAndRetry)
{
  // Given - Set up the network with two nodes
  // Note: Use the actual DNS names that will appear in the address book
  const AccountId originalNodeAccountId = AccountId(3ULL);
  const AccountId node2OriginalAccountId = AccountId(4ULL);

  std::unordered_map<std::string, AccountId> network;
  network["network-node1-svc.solo.svc.cluster.local:50211"] = originalNodeAccountId;
  network["network-node2-svc.solo.svc.cluster.local:51211"] = node2OriginalAccountId;

  Client client = Client::forNetwork(network);
  std::vector<std::string> mirrorNetwork = { "127.0.0.1:5600" };
  client.setMirrorNetwork(mirrorNetwork);

  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  NodeAddressBook addressBook = AddressBookQuery().setFileId(FileId::ADDRESS_BOOK).execute(client);
  for (const auto& address : addressBook.getNodeAddresses())
  {
    std::cout << address.toString() << std::endl;
  }

  // Create the account that will be the new node account id
  std::shared_ptr<PrivateKey> newAccountKey = ED25519PrivateKey::generatePrivateKey();
  AccountId newNodeAccountId;
  std::cout << "Creating new node account" << std::endl;
  ASSERT_NO_THROW(newNodeAccountId = AccountCreateTransaction()
                                       .setKeyWithoutAlias(newAccountKey->getPublicKey())
                                       .setInitialBalance(Hbar(1LL))
                                       .execute(client)
                                       .getReceipt(client)
                                       .mAccountId.value());
  std::cout << "New node account created!" << std::endl;

  // Update node account id
  TransactionReceipt txReceipt;
  std::cout << "Updating node with new account ID" << std::endl;
  ASSERT_NO_THROW(txReceipt = NodeUpdateTransaction()
                                .setNodeId(getNodeId())
                                .setAccountId(newNodeAccountId)
                                .freezeWith(&client)
                                .sign(newAccountKey)
                                .execute(client)
                                .getReceipt(client));
  std::cout << "Node updated!" << std::endl;

  // Poll the address book until we see the updated account ID, or timeout after 2 minutes
  std::cout << "Waiting for mirror node to update..." << std::endl;
  const auto startTime = std::chrono::steady_clock::now();
  const auto timeout = std::chrono::seconds(120);
  bool addressBookUpdated = false;

  while (std::chrono::steady_clock::now() - startTime < timeout)
  {
    std::this_thread::sleep_for(std::chrono::seconds(3));

    try
    {
      NodeAddressBook currentAddressBook = AddressBookQuery().setFileId(FileId::ADDRESS_BOOK).execute(client);
      for (const auto& address : currentAddressBook.getNodeAddresses())
      {
        if (address.getNodeId() == getNodeId() && address.getAccountId() == newNodeAccountId)
        {
          std::cout << "Mirror node updated! Node " << getNodeId() << " now has AccountId "
                    << newNodeAccountId.toString() << std::endl;
          addressBookUpdated = true;
          break;
        }
      }

      if (addressBookUpdated)
      {
        break;
      }
    }
    catch (...)
    {
      // Ignore query errors and keep polling
    }
  }

  if (!addressBookUpdated)
  {
    std::cout << "WARNING: Mirror node did not update within timeout period" << std::endl;
  }

  // Submit a transaction targeting ONLY node 0.0.4 (which is now invalid)
  // This should trigger INVALID_NODE_ACCOUNT, update the address book to learn that node2 is now different
  std::shared_ptr<PrivateKey> key = ED25519PrivateKey::generatePrivateKey();

  // Expected throw: node 0.0.4 is now invalid, address book should be updated
  std::cout << "Creating new account targeting invalid node" << std::endl;
  EXPECT_THROW(const TransactionResponse txResponse = AccountCreateTransaction()
                                                        .setKeyWithoutAlias(key->getPublicKey())
                                                        .setNodeAccountIds({ node2OriginalAccountId })
                                                        .execute(client),
               PrecheckStatusException);
  std::cout << "New account successfully not created!" << std::endl;

  std::cout << "Creating new account with address book retry" << std::endl;
  EXPECT_NO_THROW(txReceipt = AccountCreateTransaction()
                                .setKeyWithoutAlias(key->getPublicKey())
                                .setNodeAccountIds({ newNodeAccountId })
                                .execute(client)
                                .getReceipt(client));
  std::cout << "New account created!" << std::endl;

  // Revert the node account id so other tests can still function properly.
  std::cout << "Resetting node account ID" << std::endl;
  ASSERT_NO_THROW(txReceipt = NodeUpdateTransaction()
                                .setNodeId(getNodeId())
                                .setAccountId(node2OriginalAccountId)
                                .execute(client)
                                .getReceipt(client));
  std::cout << "Node account ID reset!" << std::endl;

  // Wait for mirror node to import data AND for consensus nodes to synchronize
  std::cout << "Waiting for mirror node to update..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "Mirror node should be good!" << std::endl;
}

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, CanChangeNodeAccountWithoutMirrorNodeSetup)
{
  // Given - Set up the network without mirror node
  // Note: Use the actual DNS names that will appear in the address book
  const AccountId originalNodeAccountId = AccountId(3ULL);
  const AccountId node2OriginalAccountId = AccountId(4ULL);

  std::unordered_map<std::string, AccountId> network;
  network["network-node1-svc.solo.svc.cluster.local:50211"] = originalNodeAccountId;
  network["network-node2-svc.solo.svc.cluster.local:51211"] = node2OriginalAccountId;

  Client client = Client::forNetwork(network);
  // Note: No mirror network set

  const std::string operatorKeyStr =
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137";
  std::shared_ptr<PrivateKey> originalOperatorKey = ED25519PrivateKey::fromString(operatorKeyStr);
  client.setOperator(AccountId(2ULL), originalOperatorKey);

  // Create the account that will be the new node account id
  std::shared_ptr<PrivateKey> newAccountKey = ED25519PrivateKey::generatePrivateKey();
  AccountId newNodeAccountId;
  std::cout << "Creating new node account" << std::endl;
  ASSERT_NO_THROW(newNodeAccountId = AccountCreateTransaction()
                                       .setKeyWithoutAlias(newAccountKey->getPublicKey())
                                       .setInitialBalance(Hbar(1LL))
                                       .execute(client)
                                       .getReceipt(client)
                                       .mAccountId.value());
  std::cout << "New node account created!" << std::endl;

  // Update node account id
  TransactionReceipt txReceipt;
  std::cout << "Updating node with new account ID" << std::endl;
  ASSERT_NO_THROW(txReceipt = NodeUpdateTransaction()
                                .setNodeId(getNodeId())
                                .setAccountId(newNodeAccountId)
                                .freezeWith(&client)
                                .sign(newAccountKey)
                                .execute(client)
                                .getReceipt(client));
  std::cout << "Node updated!" << std::endl;

  // Submit to the updated node - should retry
  std::shared_ptr<PrivateKey> key = ED25519PrivateKey::generatePrivateKey();
  std::cout << "Creating new account" << std::endl;
  EXPECT_NO_THROW(txReceipt = AccountCreateTransaction()
                                .setKeyWithoutAlias(key)
                                .setNodeAccountIds({ originalNodeAccountId, node2OriginalAccountId })
                                .execute(client)
                                .getReceipt(client));
  std::cout << "New account created!" << std::endl;

  // Revert the node account id so other tests can still function properly.
  std::cout << "Resetting node account ID" << std::endl;
  ASSERT_NO_THROW(txReceipt = NodeUpdateTransaction()
                                .setNodeId(getNodeId())
                                .setAccountId(node2OriginalAccountId)
                                .execute(client)
                                .getReceipt(client));
  std::cout << "Node account ID reset!" << std::endl;

  // Wait for mirror node to import data AND for consensus nodes to synchronize
  std::cout << "Waiting for mirror node to update..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "Mirror node should be good!" << std::endl;
}
