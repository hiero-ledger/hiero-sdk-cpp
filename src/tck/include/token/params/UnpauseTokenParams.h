// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_UNPAUSE_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_UNPAUSE_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `unpauseToken` JSON-RPC method call.
 */
struct UnpauseTokenParams
{
  /**
   * The ID of the token to unpause.
   */
  std::optional<std::string> mTokenId;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UnpauseTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::UnpauseTokenParams>
{
  /**
   * Convert a JSON object to a UnpauseTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the UnpauseTokenParams.
   * @param params   The PauseTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::UnpauseTokenParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_UNPAUSE_TOKEN_PARAMS_H_