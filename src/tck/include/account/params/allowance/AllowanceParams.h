// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_ALLOWANCE_PARAMS_H_
#define HIERO_TCK_CPP_ALLOWANCE_PARAMS_H_

#include "account/params/allowance/HbarAllowanceParams.h"
#include "account/params/allowance/NftAllowanceParams.h"
#include "account/params/allowance/TokenAllowanceParams.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct that contains the parameters of an allowance.
 */
struct AllowanceParams
{
  /**
   * The ID of the owner account granting the allowance.
   */
  std::string mOwnerAccountId;

  /**
   * The ID of the spender account being granted the allowance.
   */
  std::string mSpenderAccountId;

  /**
   * The Hbar allowance information.
   */
  std::optional<HbarAllowanceParams> mHbar;

  /**
   * The token allowance information.
   */
  std::optional<TokenAllowanceParams> mToken;

  /**
   * The NFT allowance information.
   */
  std::optional<NftAllowanceParams> mNft;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert AllowanceParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::AllowanceParams>
{
  /**
   * Convert a JSON object to a AllowanceParams.
   *
   * @param jsonFrom The JSON object with which to fill the AllowanceParams.
   * @param params   The AllowanceParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::AllowanceParams& params)
  {
    params.mOwnerAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "ownerAccountId");
    params.mSpenderAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "spenderAccountId");
    params.mHbar =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::AccountService::HbarAllowanceParams>(jsonFrom, "hbar");
    params.mToken =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::AccountService::TokenAllowanceParams>(jsonFrom, "token");
    params.mNft = Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::AccountService::NftAllowanceParams>(jsonFrom, "nft");

    // Only one allowance type should be allowed.
    if ((!params.mHbar.has_value() || params.mToken.has_value() || params.mNft.has_value()) &&
        (params.mHbar.has_value() || !params.mToken.has_value() || params.mNft.has_value()) &&
        (params.mHbar.has_value() || params.mToken.has_value() || !params.mNft.has_value()))
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: only one type of allowance SHALL be provided.");
    }
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_ALLOWANCE_PARAMS_H_
