// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ContractCallQuery.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
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
    json::parse(std::ifstream(std::filesystem::current_path() / "config/hello_world.json", std::ios::in))["object"]
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
                .setGas(500000ULL)
                .setBytecodeFileId(fileId)
                .setAdminKey(operatorPublicKey)
                .setMaxTransactionFee(Hbar(16LL))
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

  // Call a smart contract function
  ContractFunctionResult contractFunctionResult =
    ContractCallQuery().setGas(500000ULL).setContractId(contractId).setFunction("greet").execute(client);

  if (!contractFunctionResult.mErrorMessage.empty())
  {
    std::cout << "Error calling contract: " << contractFunctionResult.mErrorMessage << std::endl;
    return 1;
  }

  std::cout << "Contract message: " << contractFunctionResult.getString(0) << std::endl;

  // Now delete the contract
  txReceipt = ContractDeleteTransaction()
                .setContractId(contractId)
                .setTransferAccountId(operatorAccountId)
                .setMaxTransactionFee(Hbar(1LL))
                .execute(client)
                .getReceipt(client);
  std::cout << "ContractDeleteTransaction execution completed with status: " << gStatusToString.at(txReceipt.mStatus)
            << std::endl;

  return 0;
}
