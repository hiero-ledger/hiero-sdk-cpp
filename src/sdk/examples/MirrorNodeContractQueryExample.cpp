// SPDX-License-Identifier: Apache-2.0

#include "Client.h"
#include "ContractCallQuery.h"
#include "ContractCreateTransaction.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "FileCreateTransaction.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "impl/HexConverter.h"
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
  const std::shared_ptr<PrivateKey> operatorPrivateKey =
    ECDSAsecp256k1PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

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

  // Wait for mirror node to import contract data
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Estimate the gas needed
  MirrorNodeContractEstimateGasQuery query = MirrorNodeContractEstimateGasQuery();
  std::optional<ContractFunctionParameters> opt = std::nullopt;
  query.setContractId(contractId)
    .setSender(operatorAccountId)
    .setGasLimit(30000)
    .setGasPrice(1234)
    .setFunction("greet", opt);

  uint64_t estimatedGas = std::stoi(query.execute(client), nullptr, 16);
  std::cout << "Estimated gas was " << estimatedGas << std::endl;

  // Do the query against the consensus node using the estimated gas
  ContractFunctionResult contractFunctionResult = ContractCallQuery()
                                                    .setContractId(contractId)
                                                    .setGas(estimatedGas)
                                                    .setQueryPayment(Hbar(1LL))
                                                    .setFunction("greet")
                                                    .execute(client);

  std::cout << "Contract call query result was: "
            << internal::HexConverter::bytesToHex(contractFunctionResult.mContractCallResult) << std::endl;

  // Simulate the transaction for free, using the mirror node
  MirrorNodeContractCallQuery callQuery = MirrorNodeContractCallQuery();
  std::string callResult = callQuery.setContractId(contractId).setFunction("greet", opt).execute(client);

  std::cout << "Contract call simulation result was: " << callResult << std::endl;
}
