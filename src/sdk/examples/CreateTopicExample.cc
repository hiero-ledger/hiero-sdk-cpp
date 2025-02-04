// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "Status.h"
#include "TopicCreateTransaction.h"
#include "TopicId.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"

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

  // Create a new topic.
  const TopicId topicId = TopicCreateTransaction().execute(client).getReceipt(client).mTopicId.value();
  std::cout << "Created new topic with ID " << topicId.toString() << std::endl;

  // Submit a message on this topic.
  std::cout << "Submitting message: "
            << gStatusToString.at(TopicMessageSubmitTransaction()
                                    .setTopicId(topicId)
                                    .setMessage("Hello world!")
                                    .execute(client)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  return 0;
}
