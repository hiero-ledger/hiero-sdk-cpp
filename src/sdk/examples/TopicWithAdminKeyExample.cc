// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "KeyList.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicId.h"
#include "TopicUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

#include <array>
#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Generate a threshold key for the topic.
  std::array<std::shared_ptr<PrivateKey>, 3> keys = { ECDSAsecp256k1PrivateKey::generatePrivateKey(),
                                                      ECDSAsecp256k1PrivateKey::generatePrivateKey(),
                                                      ECDSAsecp256k1PrivateKey::generatePrivateKey() };
  auto topicKeys = std::make_shared<KeyList>(KeyList::withThreshold(2));
  std::for_each(
    keys.cbegin(), keys.cend(), [&topicKeys](const std::shared_ptr<PrivateKey>& key) { topicKeys->push_back(key); });

  // Create the topic.
  const TopicId topicId = TopicCreateTransaction()
                            .setAdminKey(topicKeys)
                            .freezeWith(&client)
                            .sign(keys.at(0))
                            .sign(keys.at(1))
                            .execute(client)
                            .getReceipt(client)
                            .mTopicId.value();
  std::cout << "Create new topic with ID " << topicId.toString() << std::endl;

  // Generate a new threshold key to be the new topic admin key.
  std::array<std::shared_ptr<PrivateKey>, 4> newKeys = { ECDSAsecp256k1PrivateKey::generatePrivateKey(),
                                                         ECDSAsecp256k1PrivateKey::generatePrivateKey(),
                                                         ECDSAsecp256k1PrivateKey::generatePrivateKey(),
                                                         ECDSAsecp256k1PrivateKey::generatePrivateKey() };
  topicKeys = std::make_shared<KeyList>(KeyList::withThreshold(3));
  std::for_each(newKeys.cbegin(),
                newKeys.cend(),
                [&topicKeys](const std::shared_ptr<PrivateKey>& key) { topicKeys->push_back(key); });

  // Update the topic.
  std::cout << "Updating topic with new admin threshold key: "
            << gStatusToString.at(TopicUpdateTransaction()
                                    .setTopicId(topicId)
                                    .setAdminKey(topicKeys)
                                    .freezeWith(&client)
                                    // Two of the three old keys are required to sign.
                                    .sign(keys.at(0))
                                    .sign(keys.at(2))
                                    // Three of the four new keys are required to sign.
                                    .sign(newKeys.at(0))
                                    .sign(newKeys.at(1))
                                    .sign(newKeys.at(3))
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  // Clean up: Attempt to delete the topic with admin key
  std::cout << "Deleting topic with the admin key: "
            << gStatusToString.at(TopicDeleteTransaction()
                                    .setTopicId(topicId)
                                    .freezeWith(&client)
                                    .sign(newKeys.at(0))
                                    .sign(newKeys.at(1))
                                    .sign(newKeys.at(3))
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  return 0;
}
