// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "SubscriptionHandle.h"
#include "TopicCreateTransaction.h"
#include "TopicId.h"
#include "TopicMessage.h"
#include "TopicMessageQuery.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <chrono>
#include <dotenv.h>
#include <iostream>
#include <thread>

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

  // Generate a submit key.
  const std::shared_ptr<PrivateKey> submitKey = ED25519PrivateKey::generatePrivateKey();

  // Create a topic with the submit key.
  const TopicId topicId =
    TopicCreateTransaction().setSubmitKey(submitKey).execute(client).getReceipt(client).mTopicId.value();
  std::cout << "Created topic " << topicId.toString() << " with submit key " << submitKey->toStringRaw() << std::endl;

  // Wait for topic to propagate to the mirror nodes.
  std::cout << "Waiting to propagate to mirror nodes";
  for (int i = 0; i < 5; ++i)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << '.';
  }
  std::cout << std::endl;

  // Subscribe to the topic.
  TopicMessageQuery query = TopicMessageQuery().setTopicId(topicId);
  std::shared_ptr<SubscriptionHandle> handle = query.subscribe(
    client,
    [](const TopicMessage& message)
    { std::cout << "Received message: " << internal::Utilities::byteVectorToString(message.mContents) << std::endl; });

  for (int i = 0; true; ++i)
  {
    const TransactionReceipt txReceipt = TopicMessageSubmitTransaction()
                                           .setTopicId(topicId)
                                           .setMessage("Hello from HCS " + std::to_string(i))
                                           .freezeWith(&client)
                                           // Due to the topic having a submit key, the submit key must sign.
                                           .sign(submitKey)
                                           .execute(client)
                                           .getReceipt(client);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0;
}
