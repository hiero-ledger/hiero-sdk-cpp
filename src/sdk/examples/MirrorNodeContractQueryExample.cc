// SPDX-License-Identifier: Apache-2.0

#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ED25519PrivateKey.h"
#include "FileCreateTransaction.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "impl/MirrorNodeContractCallQuery.h"
#include "impl/MirrorNodeContractEstimateGasQuery.h"
#include "impl/Utilities.h"

#include <chrono>
#include <dotenv.h>
#include <iostream>
#include <thread>

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

  // Get the contract's bytecode
  const std::vector<std::byte> byteCode = internal::Utilities::stringToByteVector(
    json::parse(std::ifstream(std::filesystem::current_path() / "config/hello_world.json", std::ios::in))["object"]
      .get<std::string>());

  // Create the contract's bytecode file
  TransactionReceipt txReceipt = FileCreateTransaction()
                                   .setKeys({ operatorPrivateKey->getPublicKey() })
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
                .setAdminKey(operatorPrivateKey->getPublicKey())
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

  std::this_thread::sleep_for(std::chrono::seconds(4));

  MirrorNodeContractEstimateGasQuery query = MirrorNodeContractEstimateGasQuery();
  std::optional<ContractFunctionParameters> opt = std::nullopt;
  query.setContractId(contractId)
    .setSender(operatorAccountId)
    .setGasLimit(30000)
    .setGasPrice(1234)
    .setFunction("greet", opt);

  std::string result = query.execute(client);

  std::cout << result << std::endl;
}
