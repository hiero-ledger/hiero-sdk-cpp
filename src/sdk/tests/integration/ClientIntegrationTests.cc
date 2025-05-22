// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "PublicKey.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/UninitializedException.h"
#include "impl/Utilities.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

using json = nlohmann::json;
using namespace std;
using namespace Hiero;

class ClientIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::string_view& getJsonNetworkTag() const { return mJsonNetworkTag; }
  [[nodiscard]] inline const std::string_view& getJsonOperatorTag() const { return mJsonOperatorTag; }
  [[nodiscard]] inline const std::string_view& getJsonAccountIdTag() const { return mJsonAccountIdTag; }
  [[nodiscard]] inline const std::string_view& getJsonPrivateKeyTag() const { return mJsonPrivateKeyTag; }

  [[nodiscard]] inline const std::string_view& getAccountIdStr() const { return mAccountIdStr; }
  [[nodiscard]] inline const AccountId& getAccountId() const { return mAccountId; }
  [[nodiscard]] inline const std::string getPathToJSON() const { return mFilePath.string(); }

private:
  const std::string_view mJsonNetworkTag = "network";
  const std::string_view mJsonOperatorTag = "operator";
  const std::string_view mJsonAccountIdTag = "accountId";
  const std::string_view mJsonPrivateKeyTag = "privateKey";

  const std::string_view mAccountIdStr = "0.0.3";
  const AccountId mAccountId = AccountId::fromString("0.0.3");
  const std::filesystem::path mFilePath = (std::filesystem::current_path() / "local_node.json").string();
};

//-----
TEST_F(ClientIntegrationTests, ConnectToLocalNode)
{
  // Given
  const auto accountId = getAccountId();
  const std::string_view accountIdStr = getAccountIdStr();
  const std::string_view networkTag = getJsonNetworkTag();
  const std::string_view operatorTag = getJsonOperatorTag();
  const std::string_view accountIdTag = getJsonAccountIdTag();
  const std::string_view privateKeyTag = getJsonPrivateKeyTag();

  const std::string testPathToJSON = getPathToJSON();
  const std::unique_ptr<PrivateKey> testPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> testPublicKey = testPrivateKey->getPublicKey();
  const auto testInitialHbarBalance = Hbar(1000ULL, HbarUnit::TINYBAR());

  AccountId operatorAccountId;
  std::string operatorAccountPrivateKey;
  std::ifstream testInputFile(testPathToJSON, std::ios::in);
  std::string nodeAddressString;
  json jsonData = json::parse(testInputFile);

  if (jsonData[networkTag][accountIdStr].is_string())
  {
    nodeAddressString = jsonData[networkTag][accountIdStr];
  }

  if (jsonData[operatorTag][accountIdTag].is_string() && jsonData[operatorTag][privateKeyTag].is_string())
  {
    std::string operatorAccountIdStr = jsonData[operatorTag][accountIdTag];

    operatorAccountId = AccountId::fromString(operatorAccountIdStr);
    operatorAccountPrivateKey = jsonData[operatorTag][privateKeyTag];
  }

  testInputFile.close();

  std::unordered_map<std::string, AccountId> networkMap;
  networkMap.insert(std::pair<std::string, AccountId>(nodeAddressString, accountId));

  // When
  Client client = Client::forNetwork(networkMap);
  client.setOperator(operatorAccountId, ED25519PrivateKey::fromString(operatorAccountPrivateKey));

  TransactionResponse txResp = AccountCreateTransaction()
                                 .setKeyWithoutAlias(testPublicKey)
                                 .setInitialBalance(testInitialHbarBalance)
                                 .execute(client);

  const AccountId newAccountId = txResp.getReceipt(client).mAccountId.value();

  // Then
  EXPECT_EQ(client.getOperatorAccountId()->toString(), operatorAccountId.toString());
  EXPECT_NE(client.getOperatorPublicKey(), nullptr);
  EXPECT_FALSE(newAccountId.toString().empty());
}

//-----
TEST_F(ClientIntegrationTests, InitWithMirrorNetworkAnCustomRealmAndShard)
{
  // Given
  const std::string_view mirrorNetworkUrl = "127.0.0.1:5600";
  const int64_t customShardNum = 0ULL;
  const int64_t customRealmNum = 0ULL;

  // When
  Client client = Client::forMirrorNetwork({ mirrorNetworkUrl.data() }, customShardNum, customRealmNum);
  std::vector<std::string> mirrorNetwork = client.getMirrorNetwork();

  // Then
  EXPECT_EQ(mirrorNetwork.size(), 1);
}
