// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOKEN_SERVICE_H_
#define HIERO_TCK_CPP_TOKEN_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::TokenService
{
/**
 * Forward declarations.
 */
struct AssociateTokenParams;
struct CreateTokenParams;
struct DeleteTokenParams;
struct DissociateTokenParams;
struct FreezeTokenParams;
struct GrantTokenKycParams;
struct PauseTokenParams;
struct RevokeTokenKycParams;
struct UnfreezeTokenParams;
struct UnpauseTokenParams;
struct UpdateTokenFeeScheduleParams;
struct UpdateTokenParams;

/**
 * Associate an account with tokens.
 *
 * @param params The parameters to use to associate the account and tokens.
 * @return A JSON response containing the status of the token association.
 */
nlohmann::json associateToken(const AssociateTokenParams& params);

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
 * Dissociate an account from tokens.
 *
 * @param params The parameters to use to dissociate the account.
 * @return A JSON response containing the status of the account dissociation.
 */
nlohmann::json dissociateToken(const DissociateTokenParams& params);

/**
 * Freeze a token on an account.
 *
 * @params The parameters to use to freeze a token.
 * @return A JSON response containing the status of the token freeze.
 */
nlohmann::json freezeToken(const FreezeTokenParams& params);

/**
 * Grant KYC of a token to an account.
 *
 * @param params The parameters to use to grant KYC.
 * @return A JSON response containing the status of the token KYC grant.
 */
nlohmann::json grantTokenKyc(const GrantTokenKycParams& params);

/**
 * Pause a token.
 *
 * @param params The parameters to use to pause a token.
 * @return A JSON response containing the status of the token pause.
 */
nlohmann::json pauseToken(const PauseTokenParams& params);

/**
 * Revoke KYC of a token from an account.
 *
 * @param params The parameters to use to revoke KYC.
 * @return A JSON response containing the status of the token KYC revoke.
 */
nlohmann::json revokeTokenKyc(const RevokeTokenKycParams& params);

/**
 * Unfreeze a token from an account.
 *
 * @params The parameters to use to unfreeze a token.
 * @return A JSON response containing the status of the token unfreeze.
 */
nlohmann::json unfreezeToken(const UnfreezeTokenParams& params);

/**
 * Unpause a token.
 *
 * @param params The parameters to use to unpause a token.
 * @return A JSON response containing the status of the token unpause.
 */
nlohmann::json unpauseToken(const UnpauseTokenParams& params);

/**
 * Update the fee schedule of a token.
 *
 * @param params The parameters to use to update a token's fee schedule.
 * @return A JSON response containing the status of the fee schedule update.
 */
nlohmann::json updateTokenFeeSchedule(const UpdateTokenFeeScheduleParams& params);

/**
 * Update a token.
 *
 * @param params The parameters to use to update a token.
 * @ return A JSON response containing the status of the token update.
 */
nlohmann::json updateToken(const UpdateTokenParams& params);

} // namespace Hiero::TCK::TokenService

#endif // HIERO_TCK_CPP_TOKEN_SERVICE_H_
