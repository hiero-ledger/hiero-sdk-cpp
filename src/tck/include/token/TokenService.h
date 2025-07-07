// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOKEN_SERVICE_H_
#define HIERO_TCK_CPP_TOKEN_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::TokenService
{
/**
 * Forward declarations.
 */
struct AirdropTokenParams;
struct AssociateTokenParams;
struct BurnTokenParams;
struct ClaimAirdropParams;
struct CreateTokenParams;
struct DeleteTokenParams;
struct DissociateTokenParams;
struct FreezeTokenParams;
struct GrantTokenKycParams;
struct MintTokenParams;
struct PauseTokenParams;
struct RevokeTokenKycParams;
struct UnfreezeTokenParams;
struct UnpauseTokenParams;
struct UpdateTokenFeeScheduleParams;
struct UpdateTokenParams;
struct WipeTokenParams;

/**
 * Airdrop tokens to accounts.
 *
 * @params params The parameters to use to airdrop tokens.
 * @return A JSON response containing the status of the token airdrop.
 */
nlohmann::json airdropToken(const AirdropTokenParams& params);

/**
 * Associate an account with tokens.
 *
 * @param params The parameters to use to associate the account and tokens.
 * @return A JSON response containing the status of the token association.
 */
nlohmann::json associateToken(const AssociateTokenParams& params);

/**
 * Burn a token.
 *
 * @param params The parameters to use to burn a token.
 * @return A JSON response containing the status of the token burn and the new total supply of the token.
 */
nlohmann::json burnToken(const BurnTokenParams& params);

/**
 * Claim an airdrop.
 *
 * @param params The parameters to use to claim an airdrop.
 * @return A JSON response containing the status of the airdrop claim.
 */
nlohmann::json claimAirdrop(const ClaimAirdropParams& params);

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
 * Mint a token.
 *
 * @param params The parameters to use to mint a token.
 * @return A JSON response containing the status of the token mint.
 */
nlohmann::json mintToken(const MintTokenParams& params);

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

/**
 * Wipe a token or tokens from an account.
 *
 * @param params The parameters to use to wipe the token(s).
 * @ return A JSON response containing the status of the token wipe.
 */
nlohmann::json wipeToken(const WipeTokenParams& params);

} // namespace Hiero::TCK::TokenService

#endif // HIERO_TCK_CPP_TOKEN_SERVICE_H_
