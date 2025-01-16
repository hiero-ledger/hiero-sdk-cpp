// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_MINT_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_MINT_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `mintToken` JSON-RPC method call.
 */
struct MintTokenParams
{
  /**
   * The ID of the token to mint.
   */
  std::optional<std::string> mTokenId;

  /**
   * The amount of fungible tokens to mint.
   */
  std::optional<std::string> mAmount;

  /**
   * The metadata for the NFTs to mint, as hex strings.
   */
  std::optional<std::vector<std::string>> mMetadata;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hedera::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert MintTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::MintTokenParams>
{
  /**
   * Convert a JSON object to a MintTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the MintTokenParams.
   * @param params   The MintTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::MintTokenParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mAmount = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "amount");
    params.mMetadata = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "metadata");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_MINT_TOKEN_PARAMS_H_
