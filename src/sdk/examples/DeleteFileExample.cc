// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

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

  // Create a new file.
  const FileId fileId = FileCreateTransaction()
                          .setKeys({ client.getOperatorPublicKey() })
                          .setContents(internal::Utilities::stringToByteVector("Hiero is great!"))
                          .execute(client)
                          .getReceipt(client)
                          .mFileId.value();
  std::cout << "Created new file with ID " << fileId.toString() << std::endl;

  // Delete the newly-created file.
  const TransactionReceipt txReceipt = FileDeleteTransaction().setFileId(fileId).execute(client).getReceipt(client);
  std::cout << "Deleted file with response code: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  return 0;
}
