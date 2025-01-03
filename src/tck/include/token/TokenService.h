// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOKEN_SERVICE_H_
#define HIERO_TCK_CPP_TOKEN_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::TokenService
{
/**
 * Forward declarations.
 */
struct BurnTokenParams;
struct CreateTokenParams;
struct DeleteTokenParams;
struct MintTokenParams;
struct UpdateTokenParams;

/**
 * Burn a token.
 *
 * @param params The parameters to use to burn a token.
 * @return A JSON response containing the status of the token burn and the new total supply of the token.
 */
nlohmann::json burnToken(const BurnTokenParams& params);

/**
 * Create a token.
 *
 * @param params The parameters to use to create a token.
 * @return A JSON response containing the created token ID and the status of the token creation.
 */
nlohmann::json createToken(const CreateTokenParams& params);

/**
 * Delete a token.
 *
 * @param params The parameters to use to delete a token.
 * @return A JSON response containing the status of the token deletion.
 */
nlohmann::json deleteToken(const DeleteTokenParams& params);

/**
 * Mint a token.
 *
 * @param params The parameters to use to mint a token.
 * @return A JSON response containing the status of the token mint.
 */
nlohmann::json mintToken(const MintTokenParams& params);

/**
 * Update a token.
 *
 * @param params The parameters to use to update a token.
 * @return A JSON response containing the status of the token update.
 */
nlohmann::json updateToken(const UpdateTokenParams& params);

} // namespace Hiero::TCK::TokenService

#endif // HIERO_TCK_CPP_TOKEN_SERVICE_H_
