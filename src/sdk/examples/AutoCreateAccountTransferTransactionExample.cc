// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PrivateKey.h"
#include "EvmAddress.h"
#include "Hbar.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

#include <chrono>
#include <dotenv.h>
#include <iostream>
#include <memory>
#include <stdexcept>
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
        /**
         * Auto-create a new account using a public address via a TransferTransaction. Reference: [HIP-583 Expand alias
         * support in CryptoCreate & CryptoTransfer Transactions](https://hips.hedera.com/hip/hip-583)
         *
         * - Create an ECDSA private key.
         * - Extract the ECDSA public key.
         * - Extract the Ethereum public address.
         * - Use the TransferTransaction.
         *   - Populate the FromAddress with the sender Hedera account ID.
         *   - Populate the ToAddress with Ethereum public address.
         *   - Note: Can transfer from public address to public address in the TransferTransaction for complete accounts.
         *           Transfers from hollow accounts will not work because the hollow account does not have a public key
         *           assigned to authorize transfers out of the account.
         * - Sign the TransferTransaction using an existing Hedera account and key paying for the transaction fee.
         * - The AccountCreateTransaction is executed as a child transaction triggered by the TransferTransaction.
         * - The Hedera account that was created has a public address the user specified in the TransferTransaction ToAddress.
         *   - Will not have a public key at this stage.
         *   - Cannot do anything besides receive tokens or hbars.
         *   - The alias property of the account does not have the public address.
         *   - Referred to as a hollow account.
         * - To get the new account ID, ask for the child receipts or child records for the parent transaction ID of the
         *   TransferTransaction.
         * - Get the AccountInfo and verify the account is a hollow account with the supplied public address (may need to
         *   verify with mirror node API).
         * - To enhance the hollow account to have a public key, the hollow account needs to be specified as a transaction fee
         *   payer in a HAPI transaction.
         * - Create a HAPI transaction and assign the new hollow account as the transaction fee payer.
         * - Sign with the private key that corresponds to the public key on the hollow account.
         * - Get the AccountInfo for the account and return the public key on the account to show it is a complete account.
         */

        // Step 1: Create an ECDSA private key
        std::cout << "Generating ECDSA private key..." << std::endl;
        const std::shared_ptr<ECDSAsecp256k1PrivateKey> newPrivateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

        // Step 2: Extract the ECDSA public key
        const std::shared_ptr<ECDSAsecp256k1PublicKey> newPublicKey =
            std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(newPrivateKey->getPublicKey());

        // Step 3: Extract the Ethereum public address
        const EvmAddress evmAddress = newPublicKey->toEvmAddress();

        // Step 4: Use TransferTransaction to auto-create the hollow account
        std::cout << "Executing TransferTransaction to create hollow account..." << std::endl;
        TransferTransaction transferTx;
        transferTx.addHbarTransfer(operatorAccountId, Hbar(10LL).negated());
        transferTx.addHbarTransfer(AccountId(0, 0, evmAddress), Hbar(10LL)); // Use AccountId from EvmAddress
        transferTx.freezeWith(&client);

        // Step 5: Sign and execute the TransferTransaction
        TransactionResponse response = transferTx.execute(client);

        // Step 6: Get the child receipt to find the new account ID
        TransactionReceipt receipt = TransactionReceiptQuery()
                                         .setTransactionId(response.mTransactionId)
                                         .setIncludeChildren(true)
                                         .execute(client);

        if (receipt.mChildren.empty()) {
            throw std::runtime_error("No child transactions found; account creation failed.");
        }

        AccountId newAccountId = receipt.mChildren.at(0).mAccountId.value();

        // Wait for mirror node to update (adjust duration if needed)
        std::cout << "Waiting 5 seconds for mirror node to update..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Populate the account's EVM address from mirror node
        newAccountId.populateAccountEvmAddress(client);

        std::cout << "Hollow account created: " << newAccountId.toString() << std::endl;

        // Step 7: Enhance the hollow account to a complete account by using it as payer
        std::cout << "Enhancing hollow account by using it as payer in a transaction..." << std::endl;
        Client newClient = Client::forTestnet(); // New client for the new account
        newClient.setOperator(newAccountId, newPrivateKey);

        // Create a simple transfer back to operator to trigger enhancement
        TransferTransaction enhanceTx;
        enhanceTx.addHbarTransfer(newAccountId, Hbar(1LL).negated());
        enhanceTx.addHbarTransfer(operatorAccountId, Hbar(1LL));
        enhanceTx.freezeWith(&newClient);

        // Execute with the new account as payer (signed by its private key)
        TransactionResponse enhanceResponse = enhanceTx.execute(newClient);
        TransactionReceipt enhanceReceipt = enhanceResponse.getReceipt(newClient);

        if (enhanceReceipt.mStatus != Status::SUCCESS) {
            throw std::runtime_error("Account enhancement failed with status: " + enhanceReceipt.mStatus.toString());
        }

        // Step 8: Verify the account is now complete by querying AccountInfo
        std::cout << "Verifying enhanced account..." << std::endl;
        AccountInfo info = AccountInfoQuery().setAccountId(newAccountId).execute(client);
        std::cout << "Enhanced account ID: " << info.mAccountId.toString() << std::endl;
        std::cout << "Public key: " << (info.mKey ? info.mKey->toString() : "No key (still hollow)") << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        client.close();
        return 1;
    }

    // Clean up
    client.close();
    return 0;
}
