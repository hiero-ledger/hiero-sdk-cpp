// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_PAUSE_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_PAUSE_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `pauseToken` JSON-RPC method call.
 */
struct PauseTokenParams
{
  /**
   * The ID of the token to pause.
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
 * JSON serializer template specialization required to convert PauseTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::PauseTokenParams>
{
  /**
   * Convert a JSON object to a PauseTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the PauseTokenParams.
   * @param params   The PauseTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::PauseTokenParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_PAUSE_TOKEN_PARAMS_H_
