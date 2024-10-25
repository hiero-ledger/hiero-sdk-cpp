/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef HEDERA_TCK_CPP_ACCOUNT_SERVICE_H_
#define HEDERA_TCK_CPP_ACCOUNT_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hedera::TCK::AccountService
{
/**
 * Forward declarations.
 */
class CreateAccountParams;
class DeleteAccountParams;
class UpdateAccountParams;

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
 * Update an account.
 *
 * @param params The parameters to use to update an account.
 * @return A JSON response containing the status of the account update.
 */
nlohmann::json updateAccount(const UpdateAccountParams& params);

} // namespace Hedera::TCK::AccountService

#endif // HEDERA_TCK_CPP_ACCOUNT_SERVICE_H_
