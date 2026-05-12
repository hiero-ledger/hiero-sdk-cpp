// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
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
  const std::shared_ptr<PrivateKey> operatorPrivateKey =
    ECDSAsecp256k1PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the configured Hiero network (defaults to testnet when HIERO_NETWORK is unset), and set
  // the operator so all generated transactions will be paid for by this account and signed by this key.
  const char* const network = std::getenv("HIERO_NETWORK");
  Client client = network ? Client::forName(network) : Client::forTestnet();
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
