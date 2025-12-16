// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AddressBookQuery.h"
#include "BaseIntegrationTest.h"
#include "ED25519PrivateKey.h"
#include "FileId.h"
#include "FreezeTransaction.h"
#include "Hbar.h"
#include "NodeAddress.h"
#include "NodeAddressBook.h"
#include "NodeUpdateTransaction.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>
#include <thread>

using namespace Hiero;

class NodeUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] const uint64_t& getNodeId() const { return mNodeId; }
  [[nodiscard]] const AccountId& getAccountId() const { return mAccountId; }
  [[nodiscard]] const FileId& getFileId() const { return mFileId; }
  [[nodiscard]] const std::vector<Endpoint>& getGossipEndpoints() const { return mGossipEndpoints; }
  [[nodiscard]] const std::vector<Endpoint>& getGrpcServiceEndpoints() const { return mGrpcServiceEndpoints; }
  [[nodiscard]] const std::vector<std::byte> getGossipCertificate() const
  {
    return internal::HexConverter::hexToBytes(mGossipCertificateDer);
  }
  [[nodiscard]] const std::vector<std::byte> getFileHash() const
  {
    return internal::HexConverter::hexToBytes(mFileHash);
  }

  // Node ID to update in tests
  const uint64_t nodeIDToUpdate = 1;

private:
  const uint64_t mNodeId = 2;
  const AccountId mAccountId = AccountId::fromString("0.0.4");
  const FileId mFileId = FileId::fromString("0.0.150");
  // The file hash needs to be taken from the network context to be correct
  const std::string mFileHash =
    "d9ec902a9fb8dc0f1a43c84b451c59dfe47622d9e5c33965a0ace77003fcad9e0b71478976dbee9dee5d2403f9267b18";
  const Endpoint endpoint1 = Endpoint().setDomainName("test.com").setPort(123);
  const Endpoint endpoint2 = Endpoint().setDomainName("test2.com").setPort(123);
  const std::vector<Endpoint> mGossipEndpoints = { endpoint1, endpoint2 };
  const std::vector<Endpoint> mGrpcServiceEndpoints = { endpoint1, endpoint2 };
  const std::string mGossipCertificateDer =
    "3082052830820310a003020102020101300d06092a864886f70d01010c05003010310e300c060355040313056e6f6465333024170d32343130"
    "30383134333233395a181332313234313030383134333233392e3337395a3010310e300c060355040313056e6f64653330820222300d06092a"
    "864886f70d01010105000382020f003082020a0282020100af111cff0c4ad8125d2f4b8691ce87332fecc867f7a94ddc0f3f96514cc4224d44"
    "af516394f7384c1ef0a515d29aa6116b65bc7e4d7e2d848cf79fbfffedae3a6583b3957a438bdd780c4981b800676ea509bc8c619ae04093b5"
    "fc642c4484152f0e8bcaabf19eae025b630028d183a2f47caf6d9f1075efb30a4248679d871beef1b7e9115382270cbdb68682fae4b1fd592c"
    "adb414d918c0a8c23795c7c5a91e22b3e90c410825a2bc1a840efc5bf9976a7f474c7ed7dc047e4ddd2db631b68bb4475f173baa3edc234c4b"
    "ed79c83e2f826f79e07d0aade2d984da447a8514135bfa4145274a7f62959a23c4f0fae5adc6855974e7c04164951d052beb5d45cb1f3cdfd0"
    "05da894dea9151cb62ba43f4731c6bb0c83e10fd842763ba6844ef499f71bc67fa13e4917fb39f2ad18112170d31cdcb3c61c9e3253accf703"
    "dbd8427fdcb87ece78b787b6cfdc091e8fedea8ad95dc64074e1fc6d0e42ea2337e18a5e54e4aaab3791a98dfcef282e2ae1caec9cf986fabe"
    "8f36e6a21c8711647177e492d264415e765a86c58599cd97b103cb4f6a01d2edd06e3b60470cf64daca7aecf831197b466cae04baeeac19840"
    "a05394bef628aed04b611cfa13677724b08ddfd662b02fd0ef0af17eb7f4fb8c1c17fbe9324f6dc7bcc02449622636cc45ec04909b3120ab4d"
    "f4726b21bf79e955fe8f832699d2196dcd7a58bfeafb170203010001a38186308183300f0603551d130101ff04053003020100300e0603551d"
    "0f0101ff0404030204b030200603551d250101ff0416301406082b0601050507030106082b06010505070302301d0603551d0e041604146431"
    "18e05209035edd83d44a0c368de2fb2fe4c0301f0603551d23041830168014643118e05209035edd83d44a0c368de2fb2fe4c0300d06092a86"
    "4886f70d01010c05000382020100ad41c32bb52650eb4b76fce439c9404e84e4538a94916b3dc7983e8b5c58890556e7384601ca7440dde682"
    "33bb07b97bf879b64487b447df510897d2a0a4e789c409a9b237a6ad240ad5464f2ce80c58ddc4d07a29a74eb25e1223db6c00e334d7a27d32"
    "bfa6183a82f5e35bccf497c2445a526eabb0c068aba9b94cc092ea4756b0dcfb574f6179f0089e52b174ccdbd04123eeb6d70daeabd8513fcb"
    "a6be0bc2b45ca9a69802dae11cc4d9ff6053b3a87fd8b0c6bf72fffc3b81167f73cca2b3fd656c5d353c8defca8a76e2ad535f984870a590af"
    "4e28fed5c5a125bf360747c5e7742e7813d1bd39b5498c8eb6ba72f267eda034314fdbc596f6b967a0ef8be5231d364e634444c84e64bd7919"
    "425171016fcd9bb05f01c58a303dee28241f6e860fc3aac3d92aad7dac2801ce79a3b41a0e1f1509fc0d86e96d94edb18616c000152490f645"
    "61713102128990fedd3a5fa642f2ff22dc11bc4dc5b209986a0c3e4eb2bdfdd40e9fdf246f702441cac058dd8d0d51eb0796e2bea2ce1b37b2"
    "a2f468505e1f8980a9f66d719df034a6fbbd2f9585991d259678fb9a4aebdc465d22c240351ed44abffbdd11b79a706fdf7c40158d3da87f68"
    "d7bd557191a8016b5b899c07bf1b87590feb4fa4203feea9a2a7a73ec224813a12b7a21e5dc93fcde4f0a7620f570d31fe27e9b8d65b74db7d"
    "c18a5e51adc42d7805d4661938";
};

//-----
TEST_F(NodeUpdateTransactionIntegrationTests, DISABLED_CanExecuteNodeUpdateTransaction)
{
  // Given
  const std::shared_ptr<PrivateKey> adminKey = ED25519PrivateKey::generatePrivateKey();

  // When / Then
  TransactionResponse txResponse;

  ASSERT_NO_THROW(txResponse = NodeUpdateTransaction()
                                 .setNodeId(getNodeId())
                                 .setAccountId(getAccountId())
                                 .setGossipEndpoints(getGossipEndpoints())
                                 .setServiceEndpoints(getGrpcServiceEndpoints())
                                 .setGossipCaCertificate(getGossipCertificate())
                                 .setAdminKey(adminKey->getPublicKey())
                                 .freezeWith(&getTestClient())
                                 .sign(adminKey)
                                 .execute(getTestClient()));

  ASSERT_NO_THROW(txResponse = FreezeTransaction()
                                 .setFreezeType(FreezeType::PREPARE_UPGRADE)
                                 .setFileId(getFileId())
                                 .setFileHash(getFileHash())
                                 .freezeWith(&getTestClient())
                                 .execute(getTestClient()));

  ASSERT_NO_THROW(txResponse = FreezeTransaction()
                                 .setFreezeType(FreezeType::FREEZE_UPGRADE)
                                 .setStartTime(std::chrono::system_clock::now() + std::chrono::seconds(5))
                                 .setFileId(getFileId())
                                 .setFileHash(getFileHash())
                                 .freezeWith(&getTestClient())
                                 .execute(getTestClient()));

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
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
  ASSERT_NO_THROW(
    txResponse = NodeUpdateTransaction().setNodeId(nodeIDToUpdate).setAccountId(originalNodeAccountId).execute(client));

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
    NodeUpdateTransaction().setNodeId(nodeIDToUpdate).setAccountId(operatorAccountId).execute(client);

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
    NodeUpdateTransaction().setNodeId(nodeIDToUpdate).setAccountId(nodeAccountId).execute(client);

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
    NodeUpdateTransaction().setNodeId(nodeIDToUpdate).setAccountId(AccountId(9999999ULL)).execute(client);

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
    NodeUpdateTransaction().setNodeId(nodeIDToUpdate).setAccountId(newAccount).freezeWith(&client);
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
    NodeUpdateTransaction().setNodeId(nodeIDToUpdate).setAccountId(newAccount).freezeWith(&client);
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
                                .setNodeId(nodeIDToUpdate)
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
        if (address.getNodeId() == nodeIDToUpdate && address.getAccountId() == newNodeAccountId)
        {
          std::cout << "Mirror node updated! Node " << nodeIDToUpdate << " now has AccountId "
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
                                .setNodeId(nodeIDToUpdate)
                                .setAccountId(node2OriginalAccountId)
                                .execute(client)
                                .getReceipt(client));
  std::cout << "Node account ID reset!" << std::endl;

  // Poll the address book until we see the reverted account ID, or timeout after 2 minutes
  std::cout << "Waiting for mirror node to update after revert..." << std::endl;
  const auto revertStartTime = std::chrono::steady_clock::now();
  const auto revertTimeout = std::chrono::seconds(120);
  bool revertAddressBookUpdated = false;

  while (std::chrono::steady_clock::now() - revertStartTime < revertTimeout)
  {
    std::this_thread::sleep_for(std::chrono::seconds(3));

    try
    {
      NodeAddressBook currentAddressBook = AddressBookQuery().setFileId(FileId::ADDRESS_BOOK).execute(client);
      for (const auto& address : currentAddressBook.getNodeAddresses())
      {
        if (address.getNodeId() == nodeIDToUpdate && address.getAccountId() == node2OriginalAccountId)
        {
          std::cout << "Mirror node updated after revert! Node " << nodeIDToUpdate << " back to AccountId "
                    << node2OriginalAccountId.toString() << std::endl;
          revertAddressBookUpdated = true;
          break;
        }
      }

      if (revertAddressBookUpdated)
      {
        break;
      }
    }
    catch (...)
    {
      // Ignore query errors and keep polling
    }
  }

  if (!revertAddressBookUpdated)
  {
    std::cout << "WARNING: Mirror node did not update after revert within timeout period" << std::endl;
  }
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
                                .setNodeId(nodeIDToUpdate)
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
                                .setNodeId(nodeIDToUpdate)
                                .setAccountId(node2OriginalAccountId)
                                .execute(client)
                                .getReceipt(client));
  std::cout << "Node account ID reset!" << std::endl;

  // Poll the address book until we see the reverted account ID, or timeout after 2 minutes
  std::cout << "Waiting for mirror node to update after revert..." << std::endl;
  client.setMirrorNetwork({ "127.0.0.1:5600" });
  const auto revertStartTime2 = std::chrono::steady_clock::now();
  const auto revertTimeout2 = std::chrono::seconds(120);
  bool revertAddressBookUpdated2 = false;

  while (std::chrono::steady_clock::now() - revertStartTime2 < revertTimeout2)
  {
    std::this_thread::sleep_for(std::chrono::seconds(3));

    try
    {
      NodeAddressBook currentAddressBook = AddressBookQuery().setFileId(FileId::ADDRESS_BOOK).execute(client);
      for (const auto& address : currentAddressBook.getNodeAddresses())
      {
        if (address.getNodeId() == nodeIDToUpdate && address.getAccountId() == node2OriginalAccountId)
        {
          std::cout << "Mirror node updated after revert! Node " << nodeIDToUpdate << " back to AccountId "
                    << node2OriginalAccountId.toString() << std::endl;
          revertAddressBookUpdated2 = true;
          break;
        }
      }

      if (revertAddressBookUpdated2)
      {
        break;
      }
    }
    catch (...)
    {
      // Ignore query errors and keep polling
    }
  }

  if (!revertAddressBookUpdated2)
  {
    std::cout << "WARNING: Mirror node did not update after revert within timeout period" << std::endl;
  }
}