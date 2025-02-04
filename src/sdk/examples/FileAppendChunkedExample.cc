// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "FileAppendTransaction.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <dotenv.h>
#include <iostream>
#include <vector>

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

  // Create the contents to append.
  const std::vector<std::byte> toAppend(4096 * 9, std::byte(0x01));

  // Append the contents to the file.
  const std::vector<TransactionResponse> txResponses = FileAppendTransaction()
                                                         .setFileId(fileId)
                                                         .setContents(toAppend)
                                                         .setMaxChunks(40)
                                                         .setMaxTransactionFee(Hbar(1000))
                                                         .executeAll(client);

  // Since FileAppendTransactions get the receipts between chunks, there's no need to check here that all chunks were
  // completely processed.

  // Check the new file size.
  std::cout << "The new file size is " << FileInfoQuery().setFileId(fileId).execute(client).mSize << std::endl;

  // Delete the file.
  const TransactionReceipt txReceipt = FileDeleteTransaction().setFileId(fileId).execute(client).getReceipt(client);
  std::cout << "Deleted file with response code: " << gStatusToString.at(txReceipt.mStatus) << std::endl;

  return 0;
}
