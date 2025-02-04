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

  const TopicId topicId = TopicCreateTransaction().execute(client).getReceipt(client).mTopicId.value();

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
                                           .execute(client)
                                           .getReceipt(client);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0;
}
