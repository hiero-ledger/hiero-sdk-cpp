// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"

#include <dotenv.h>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace Hedera; // Assuming the namespace is Hedera; adjust if it's Hiero

int main(int argc, char** argv) {
    dotenv::init();

    // Load environment variables and validate
    const std::string operatorIdStr = std::getenv("OPERATOR_ID");
    const std::string operatorKeyStr = std::getenv("OPERATOR_KEY");
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
        // Step 1: Generate batch key
        const std::shared_ptr<PrivateKey> batchKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

        // Step 2: Create Alice's account
        std::cout << "Creating Alice's account and preparing batched transfer..." << std::endl;
        const std::shared_ptr<PrivateKey> aliceKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
        AccountCreateTransaction createTx;
        createTx.setKey(aliceKey->getPublicKey()); // Assuming setKeyWithoutAlias is a typo or deprecated; use setKey if appropriate
        createTx.setInitialBalance(Hbar(15LL));

        TransactionResponse createResponse = createTx.execute(client);
        TransactionReceipt createReceipt = createResponse.getReceipt(client);
        AccountId aliceId = createReceipt.mAccountId.value();

        std::cout << "Created Alice's account: " << aliceId.toString() << std::endl;

        // Step 3: Create client for Alice
        Client aliceClient = Client::forTestnet();
        aliceClient.setOperator(aliceId, aliceKey);

        // Step 4: Batchify a transfer transaction (Alice transfers 1 Hbar to operator)
        TransferTransaction transferTx;
        transferTx.addHbarTransfer(client.getOperatorAccountId().value(), Hbar(1LL));
        transferTx.addHbarTransfer(aliceId, Hbar(1LL).negated());

        WrappedTransaction aliceBatchedTransfer = transferTx.batchify(aliceClient, batchKey);

        // Step 5: Get initial balances for comparison
        Hbar aliceBalanceBefore = AccountBalanceQuery().setAccountId(aliceId).execute(client).mBalance;
        Hbar operatorBalanceBefore = AccountBalanceQuery().setAccountId(client.getOperatorAccountId().value()).execute(client).mBalance;

        // Step 6: Execute the batch transaction
        std::cout << "Executing batch transaction..." << std::endl;
        BatchTransaction batchTx;
        batchTx.addInnerTransaction(aliceBatchedTransfer);
        batchTx.freezeWith(&client);
        batchTx.sign(batchKey);

        TransactionResponse batchResponse = batchTx.execute(client);
        TransactionReceipt batchReceipt = batchResponse.getReceipt(client);

        if (batchReceipt.mStatus != Status::SUCCESS) {
            throw std::runtime_error("Batch transaction failed with status: " + batchReceipt.mStatus.toString());
        }

        std::cout << "Batch transaction executed successfully." << std::endl;

        // Step 7: Verify new balances
        std::cout << "Verifying balances after batch execution..." << std::endl;
        Hbar aliceBalanceAfter = AccountBalanceQuery().setAccountId(aliceId).execute(client).mBalance;
        Hbar operatorBalanceAfter = AccountBalanceQuery().setAccountId(client.getOperatorAccountId().value()).execute(client).mBalance;

        std::cout << "Alice's balance - Before: " << aliceBalanceBefore.toString() << ", After: " << aliceBalanceAfter.toString() << std::endl;
        std::cout << "Operator's balance - Before: " << operatorBalanceBefore.toString() << ", After: " << operatorBalanceAfter.toString() << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        client.close();
        return 1;
    }

    // Clean up
    client.close();
    return 0;
}
