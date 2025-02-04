// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileContentsQuery.h"
#include "FileCreateTransaction.h"
#include "FileId.h"
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

  // Content to be stored in the file
  const std::vector<std::byte> contents = internal::Utilities::stringToByteVector("Hiero is great!");

  // Create a new file with the contents
  FileId fileId = FileCreateTransaction()
                    .setKeys({ client.getOperatorPublicKey() })
                    .setContents(contents)
                    .execute(client)
                    .getReceipt(client)
                    .mFileId.value();
  std::cout << "The created file ID is: " << fileId.toString() << std::endl;

  // Get the file contents
  FileContents fileContents = FileContentsQuery().setFileId(fileId).execute(client);
  std::cout << "The file contains the message: " << internal::Utilities::byteVectorToString(fileContents) << std::endl;

  return 0;
}
