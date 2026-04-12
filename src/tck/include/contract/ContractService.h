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

/**
 * Delete a contract.
 *
 * @param params The parameters to use to delete a contract.
 * @return A JSON response containing the status of the contract deletion.
 */
nlohmann::json deleteContract(const DeleteContractParams& params);

} // namespace Hiero::TCK::ContractService

#endif // HIERO_TCK_CPP_CONTRACT_SERVICE_H_
