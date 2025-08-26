// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "SubscriptionHandle.h"
#include "TopicCreateTransaction.h"
#include "TopicId.h"
#include "TopicMessage.h"
#include "TopicMessageQuery.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "WrappedTransaction.h"
#include "impl/Utilities.h"

#include <chrono>
#include <dotenv.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

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
        // Step 1: Generate submit key for the topic
        const std::shared_ptr<PrivateKey> submitKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

        // Step 2: Create a new topic
        std::cout << "Creating a new topic..." << std::endl;
        TopicCreateTransaction createTx;
        createTx.setTopicMemo("hiero-sdk-cpp/ConsensusPubSubChunkedExample");
        createTx.setSubmitKey(submitKey->getPublicKey());

        TransactionResponse createResponse = createTx.execute(client);
        TransactionReceipt createReceipt = createResponse.getReceipt(client);
        TopicId topicId = createReceipt.mTopicId.value();

        std::cout << "Created topic: " << topicId.toString() << std::endl;

        // Step 3: Wait for topic to propagate to mirror nodes
        std::cout << "Waiting for topic to propagate to mirror nodes";
        for (int i = 0; i < 10; ++i) {
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
                std::cout << "Received message " << message.mSequenceNumber
                          << " which reached consensus at "
                          << std::chrono::duration_cast<std::chrono::seconds>(message.mConsensusTimestamp.time_since_epoch()).count()
                          << " and contains " << message.mContents.size() << " bytes." << std::endl;
            });

        // Step 5: Prepare and submit a large chunked message
        const std::string bigContents =
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur aliquam augue sem, ut mattis dui laoreet a. "
            // ... (the full large string remains unchanged for brevity; insert the original bigContents here)
            ;

        std::cout << "Preparing and submitting chunked message..." << std::endl;
        TopicMessageSubmitTransaction submitTx;
        submitTx.setTopicId(topicId);
        submitTx.setMaxChunks(15U);
        submitTx.setMessage(bigContents);
        submitTx.freezeWith(&client);
        submitTx.sign(operatorPrivateKey);

        // Simulate sending bytes for external signing
        std::vector<std::byte> txBytes = submitTx.toBytes();

        // Deserialize and sign with submit key
        WrappedTransaction deserializedTx = Transaction<TopicMessageSubmitTransaction>::fromBytes(txBytes);
        submitTx = *deserializedTx.getTransaction<TopicMessageSubmitTransaction>();
        submitTx.sign(submitKey);

        // Execute the transaction and get receipts for all chunks
        std::vector<TransactionResponse> txResponses = submitTx.executeAll(client);
        for (const auto& txResp : txResponses) {
            TransactionReceipt receipt = txResp.getReceipt(client);
            if (receipt.mStatus != Status::SUCCESS) {
                throw std::runtime_error("Transaction chunk failed with status: " + receipt.mStatus.toString());
            }
        }

        std::cout << "Chunked message submitted successfully." << std::endl;

        // Step 6: Receive messages for a limited time (e.g., 30 seconds) then exit
        std::cout << "Receiving messages for 30 seconds..." << std::endl;
        const auto startTime = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(30)) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        std::cout << "Message reception complete. Exiting." << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        client.close();
        return 1;
    }

    // Clean up
    client.close();
    return 0;
}
