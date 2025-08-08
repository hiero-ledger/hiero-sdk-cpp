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
#include <stdexcept>
#include <string>
#include <thread>

using namespace Hiero;

int main(int argc, char** argv) {
    dotenv::init();

    // Load and validate environment variables
    const std::string operatorIdStr = std::getenv("OPERATOR_ID") ? std::getenv("OPERATOR_ID") : "";
    const std::string operatorKeyStr = std::getenv("OPERATOR_KEY") ? std::getenv("OPERATOR_KEY") : "";
    if (operatorIdStr.empty() || operatorKeyStr.empty()) {
        std::cerr << "Error: OPERATOR_ID and OPERATOR_KEY must be set in .env file." << std::endl;
        return 1;
    }

    const AccountId operatorAccountId = AccountId::fromString(operatorIdStr);
    const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(operatorKeyStr);

    // Create client for the Hedera testnet and set operator
    Client client = Client::forTestnet();
    client.setOperator(operatorAccountId, operatorPrivateKey);

    try {
        // Step 1: Generate submit key
        const std::shared_ptr<PrivateKey> submitKey = ED25519PrivateKey::generatePrivateKey();

        // Step 2: Create a new topic with the submit key
        std::cout << "Creating a new topic with submit key..." << std::endl;
        TopicCreateTransaction createTx;
        createTx.setSubmitKey(submitKey->getPublicKey());

        TransactionResponse createResponse = createTx.execute(client);
        TransactionReceipt createReceipt = createResponse.getReceipt(client);
        TopicId topicId = createReceipt.mTopicId.value();

        std::cout << "Created topic: " << topicId.toString() << " with submit key: " << submitKey->toStringRaw() << std::endl;

        // Step 3: Wait for topic to propagate to mirror nodes
        std::cout << "Waiting for topic to propagate to mirror nodes";
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << '.';
            std::cout.flush();
        }
        std::cout << std::endl;

        // Step 4: Subscribe to the topic for incoming messages
        std::cout << "Subscribing to topic messages..." << std::endl;
        std::shared_ptr<SubscriptionHandle> handle = std::make_shared<SubscriptionHandle>();
        TopicMessageQuery query;
        query.setTopicId(topicId);
        *handle = query.subscribe(
            client,
            [](const TopicMessage& message) {
                std::cout << "Received message: " << internal::Utilities::byteVectorToString(message.mContents) << std::endl;
            });

        // Step 5: Submit a finite number of messages (e.g., 10) with signing and delay
        std::cout << "Submitting messages to the topic..." << std::endl;
        const int numMessages = 10;
        for (int i = 0; i < numMessages; ++i) {
            TopicMessageSubmitTransaction submitTx;
            submitTx.setTopicId(topicId);
            submitTx.setMessage("Hello from HCS " + std::to_string(i));
            submitTx.freezeWith(&client);
            submitTx.sign(submitKey); // Sign with submit key as required by the topic

            TransactionResponse submitResponse = submitTx.execute(client);
            TransactionReceipt submitReceipt = submitResponse.getReceipt(client);

            if (submitReceipt.mStatus != Status::SUCCESS) {
                throw std::runtime_error("Message submission failed with status: " + submitReceipt.mStatus.toString());
            }

            std::cout << "Submitted message " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Step 6: Wait briefly to receive remaining messages, then unsubscribe
        std::cout << "Waiting 5 seconds to receive any remaining messages..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        std::cout << "Unsubscribing and exiting." << std::endl;
        handle->unsubscribe();

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        client.close();
        return 1;
    }

    // Clean up
    client.close();
    return 0;
}
