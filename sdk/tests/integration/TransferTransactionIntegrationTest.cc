/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2022 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Hedera;

class TransferTransactionIntegrationTest : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const Hbar& getTestBalance() const { return mTestBalance; }

  [[nodiscard]] inline const Client& getTestClient() const { return mClient; }

  void SetUp() override
  {
    const auto accountId = AccountId::fromString("0.0.3");
    const std::string_view accountIdStr = "0.0.3";
    const std::string_view networkTag = "network";
    const std::string_view operatorTag = "operator";
    const std::string_view accountIdTag = "accountId";
    const std::string_view privateKeyTag = "privateKey";

    const std::string testPathToJSON = std::filesystem::current_path() / "local_node.json";
    const std::unique_ptr<PrivateKey> testPrivateKey = ED25519PrivateKey::generatePrivateKey();
    const std::shared_ptr<PublicKey> testPublicKey = testPrivateKey->getPublicKey();

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

    mClient = Client::forNetwork(networkMap);
    mClient.setOperator(operatorAccountId, ED25519PrivateKey::fromString(operatorAccountPrivateKey));
  }

private:
  const AccountId mTestAccountId = AccountId::fromString("0.0.1023");
  const Hbar mTestBalance = Hbar(10ULL, HbarUnit::TINYBAR());

  Client mClient;
};

// Tests invoking of method execute() from TransferTransaction.
TEST_F(TransferTransactionIntegrationTest, ExecuteRequestToTestnetNode)
{
  // Given
  const auto testBalance = getTestBalance();
  const auto testPublicKey = ED25519PrivateKey::generatePrivateKey()->getPublicKey();
  const auto accountOperatorId = AccountId::fromString("0.0.1024");
  const auto testMemo = "Test memo for TransferTransaction.";

  AccountCreateTransaction testAccountCreateTransaction;
  testAccountCreateTransaction.setKey(testPublicKey);
  testAccountCreateTransaction.setInitialBalance(testBalance);

  TransactionReceipt testTxReceipt =
    testAccountCreateTransaction.execute(getTestClient()).getRecord(getTestClient()).getReceipt().value();
  AccountId createdAccountId = testTxReceipt.getAccountId().value();

  TransferTransaction transferTransaction;
  transferTransaction.addApprovedHbarTransfer(createdAccountId, Hbar(20ULL, HbarUnit::TINYBAR()));
  transferTransaction.addApprovedHbarTransfer(accountOperatorId, Hbar(-20ULL, HbarUnit::TINYBAR()));
  transferTransaction.setTransactionMemo(testMemo);

  // When
  const TransactionResponse txResponse = transferTransaction.execute(getTestClient());

  // Then
  const TransactionRecord txRecord = txResponse.getRecord(getTestClient());
  const TransactionReceipt txReceipt = txResponse.getReceipt(getTestClient());
  EXPECT_TRUE(txResponse.getValidateStatus());
  EXPECT_TRUE(txRecord.getReceipt().has_value());
  EXPECT_TRUE(txRecord.getConsensusTimestamp().has_value());
  EXPECT_EQ(txRecord.getTransactionMemo(), testMemo);
}