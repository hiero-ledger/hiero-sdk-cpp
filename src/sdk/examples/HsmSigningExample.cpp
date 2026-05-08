// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "Hbar.h"
#include "PublicKey.h"
#include "SignableNodeTransactionBodyBytes.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

#include <dotenv.h>
#include <iostream>
#include <vector>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey =
    ECDSAsecp256k1PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  std::cout << "HSM Signing Example" << std::endl;
  std::cout << "===================" << std::endl;

  // 1. Create and freeze a transaction (do NOT sign yet)
  TransferTransaction tx = TransferTransaction()
                             .addHbarTransfer(operatorAccountId, Hbar(-1LL, HbarUnit::TINYBAR()))
                             .addHbarTransfer(AccountId(0ULL, 0ULL, 3ULL), Hbar(1LL, HbarUnit::TINYBAR()))
                             .setTransactionId(TransactionId::generate(operatorAccountId))
                             .freezeWith(&client);

  std::cout << "Transaction frozen successfully." << std::endl;

  // 2. Extract the signable body bytes for each node
  const auto signableList = tx.getSignableNodeBodyBytesList();

  std::cout << "Retrieved " << signableList.size() << " signable body bytes entries:" << std::endl;

  for (size_t i = 0; i < signableList.size(); ++i)
  {
    const auto& entry = signableList[i];
    std::cout << "  [" << i << "] Node: " << entry.mNodeAccountId.toString()
              << ", TxId: " << entry.mTransactionId.toString()
              << ", BodyBytes size: " << entry.mSignableTransactionBodyBytes.size() << " bytes" << std::endl;

    // 3. In a real HSM workflow, you would send entry.mSignableTransactionBodyBytes
    //    to the HSM for signing. The HSM would return a signature.
    //
    //    Example (pseudocode):
    //      auto signature = hsmDevice.sign(entry.mSignableTransactionBodyBytes);
    //      tx.addSignature(hsmPublicKey, signature);
    //
    //    For this example, we use the operator's private key to simulate HSM signing.
    const std::vector<std::byte> signature = operatorPrivateKey->sign(entry.mSignableTransactionBodyBytes);

    std::cout << "  [" << i << "] Signed " << signature.size() << " bytes (simulated HSM)" << std::endl;
  }

  // 4. The transaction already has the operator's signature from freezeWith(&client),
  //    so we can execute it directly. In a real HSM workflow, you would inject the
  //    HSM signatures via addSignature() before executing.
  const TransactionResponse txResponse = tx.execute(client);

  std::cout << "Transaction submitted. Getting receipt..." << std::endl;

  // 5. Get the receipt
  const TransactionReceipt txReceipt = txResponse.getReceipt(client);

  std::cout << "Transaction status: " << gStatusToString.at(txReceipt.mStatus) << std::endl;
  std::cout << "HSM signing workflow completed successfully!" << std::endl;

  return 0;
}
