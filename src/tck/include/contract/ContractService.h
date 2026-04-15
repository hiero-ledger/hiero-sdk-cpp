// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CONTRACT_SERVICE_H_
#define HIERO_TCK_CPP_CONTRACT_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::ContractService
{
/**
 * Forward declarations.
 */
struct DeleteContractParams;
struct CreateContractParams;
struct ContractByteCodeQueryParams;
struct ContractCallQueryParams;
struct ContractInfoQueryParams;
struct ExecuteContractParams;

/**
 * Delete a contract.
 *
 * @param params The parameters to use to delete a contract.
 * @return A JSON response containing the status of the contract deletion.
 */
nlohmann::json deleteContract(const DeleteContractParams& params);

/**
 * Create a contract.
 *
 * @param params The parameters to use to create a contract.
 * @return A JSON response containing the created contract ID and status.
 */
nlohmann::json createContract(const CreateContractParams& params);

/**
 * Query a bytecode of contract.
 *
 * @param params The parameters to use for the contract bytecode query.
 * @return A JSON response containing the contract bytecode.
 */
nlohmann::json contractByteCodeQuery(const ContractByteCodeQueryParams& params);

/**
 * Query a smart contract function without changing state.
 *
 * @param params The parameters to use for the contract call query.
 * @return A JSON response containing contract function result fields.
 */
nlohmann::json contractCallQuery(const ContractCallQueryParams& params);

/**
 * Query full smart contract metadata.
 *
 * @param params The parameters to use for the contract info query.
 * @return A JSON response containing contract metadata fields.
 */
nlohmann::json contractInfoQuery(const ContractInfoQueryParams& params);

/**
 * Execute a contract function.
 *
 * @param params The parameters to use to execute a contract function.
 * @return A JSON response containing the status of the contract execute.
 */
nlohmann::json executeContract(const ExecuteContractParams& params);
} // namespace Hiero::TCK::ContractService

#endif // HIERO_TCK_CPP_CONTRACT_SERVICE_H_
