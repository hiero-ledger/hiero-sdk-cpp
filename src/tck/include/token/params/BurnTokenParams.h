// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_BURN_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_BURN_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `burnToken` JSON-RPC method call.
 */
struct BurnTokenParams
{
  /**
   * The ID of the token to burn.
   */
  std::optional<std::string> mTokenId;

  /**
   * The amount of fungible tokens to burn.
   */
  std::optional<std::string> mAmount;

  /**
   * The serial numbers of the NFTs to burn.
   */
  std::optional<std::vector<std::string>> mSerialNumbers;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hedera::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert BurnTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::BurnTokenParams>
{
  /**
   * Convert a JSON object to a BurnTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the BurnTokenParams.
   * @param params   The BurnTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::BurnTokenParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mAmount = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "amount");
    params.mSerialNumbers = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "serialNumbers");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_BURN_TOKEN_PARAMS_H_
