// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ContractCallQuery.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractFunctionResult.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "FileId.h"
#include "Hbar.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/Utilities.h"

#include <dotenv.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));
  const std::shared_ptr<PublicKey> operatorPublicKey = operatorPrivateKey->getPublicKey();

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  // Get the contract's bytecode
  const std::vector<std::byte> byteCode = internal::Utilities::stringToByteVector(
    json::parse(std::ifstream(std::filesystem::current_path() / "config/stateful.json", std::ios::in))["object"]
      .get<std::string>());

  // Create the contract's bytecode file
  TransactionReceipt txReceipt = FileCreateTransaction()
                                   .setKeys({ operatorPublicKey })
                                   .setContents(byteCode)
                                   .setMaxTransactionFee(Hbar(2LL))
                                   .execute(client)
                                   .getReceipt(client);
  std::cout << "FileCreateTransaction execution completed with status: " << gStatusToString.at(txReceipt.mStatus)
            << std::endl;
  if (!txReceipt.mFileId.has_value())
  {
    std::cout << "No file created!" << std::endl;
    return 1;
  }

  const FileId fileId = txReceipt.mFileId.value();
  std::cout << "Contract bytecode file created with ID " << fileId.toString() << std::endl;

  // Create the actual contract
  txReceipt = ContractCreateTransaction()
                .setBytecodeFileId(fileId)
                .setGas(500000ULL)
                .setConstructorParameters(ContractFunctionParameters().addString("hello from hiero!").toBytes())
                .execute(client)
                .getReceipt(client);
  std::cout << "ContractCreateTransaction execution completed with status: " << gStatusToString.at(txReceipt.mStatus)
            << std::endl;
  if (!txReceipt.mContractId.has_value())
  {
    std::cout << "No contract created!" << std::endl;
    return 1;
  }

  const ContractId contractId = txReceipt.mContractId.value();
  std::cout << "Smart contract created with ID " << contractId.toString() << std::endl;

  // Call the get_message contract function
  std::cout << "Calling get_message()..." << std::endl;
  ContractFunctionResult contractFunctionResult =
    ContractCallQuery().setGas(500000ULL).setContractId(contractId).setFunction("get_message").execute(client);

  if (!contractFunctionResult.mErrorMessage.empty())
  {
    std::cout << "Error calling contract: " << contractFunctionResult.mErrorMessage << std::endl;
    return 1;
  }

  std::cout << "get_message() returned: " << contractFunctionResult.getString(0) << std::endl;

  // Now change the message
  std::cout << "Calling set_message(\"hello from hiero again!\")..." << std::endl;
  txReceipt = ContractExecuteTransaction()
                .setContractId(contractId)
                .setGas(500000ULL)
                .setFunction("set_message", ContractFunctionParameters().addString("hello from hiero again!"))
                .execute(client)
                .getReceipt(client);
  std::cout << "ContractExecuteTransaction execution completed with status: " << gStatusToString.at(txReceipt.mStatus)
            << std::endl;

  // Query again
  std::cout << "Calling get_message() again..." << std::endl;
  contractFunctionResult =
    ContractCallQuery().setGas(500000ULL).setContractId(contractId).setFunction("get_message").execute(client);

  if (!contractFunctionResult.mErrorMessage.empty())
  {
    std::cout << "Error calling contract: " << contractFunctionResult.mErrorMessage << std::endl;
    return 1;
  }

  std::cout << "get_message() returned: " << contractFunctionResult.getString(0) << std::endl;
  return 0;
}
