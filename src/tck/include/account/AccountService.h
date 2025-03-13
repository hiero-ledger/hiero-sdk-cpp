// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_ACCOUNT_SERVICE_H_
#define HIERO_TCK_CPP_ACCOUNT_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::AccountService
{
/**
 * Forward declarations.
 */
struct ApproveAllowanceParams;
struct CreateAccountParams;
struct DeleteAccountParams;
struct DeleteAllowanceParams;
struct TransferCryptoParams;
struct UpdateAccountParams;

/**
 * Approve an allowance to an account.
 *
 * @param params The parameters to use to approve the allowance.
 * @return A JSON response containing the status of the allowance approval.
 */
nlohmann::json approveAllowance(const ApproveAllowanceParams& params);

/**
 * Delete an allowance from an account.
 *
 * @param params The parameters to use to delete the allowance.
 * @return A JSON response containing the status of the allowance deletion.
 */
nlohmann::json deleteAllowance(const DeleteAllowanceParams& params);

/**
 * Create an account.
 *
 * @param params The parameters to use to create an account.
 * @return A JSON response containing the created account ID and the status of the account creation.
 */
nlohmann::json createAccount(const CreateAccountParams& params);

/**
 * Delete an account.
 *
 * @param params The parameters to use to delete an account.
 * @return A JSON response containing the status of the account deletion.
 */
nlohmann::json deleteAccount(const DeleteAccountParams& params);

/**
 * Transfer crypto between accounts.
 *
 * @param params The parameters to use to transfer crypto.
 * @return A JSON response containing the status of the transfer.
 */
nlohmann::json transferCrypto(const TransferCryptoParams& params);

/**
 * Update an account.
 *
 * @param params The parameters to use to update an account.
 * @return A JSON response containing the status of the account update.
 */
nlohmann::json updateAccount(const UpdateAccountParams& params);

} // namespace Hiero::TCK::AccountService

#endif // HIERO_TCK_CPP_ACCOUNT_SERVICE_H_
