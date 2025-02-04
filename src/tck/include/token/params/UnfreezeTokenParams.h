// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_UNFREEZE_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_UNFREEZE_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `unfreezeToken` JSON-RPC method call.
 */
struct UnfreezeTokenParams
{
  /**
   * The ID of the token to freeze.
   */
  std::optional<std::string> mTokenId;

  /**
   * The ID of the account to freeze.
   */
  std::optional<std::string> mAccountId;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hedera::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UnfreezeTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::UnfreezeTokenParams>
{
  /**
   * Convert a JSON object to a UnfreezeTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the UnfreezeTokenParams.
   * @param params   The UnfreezeTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::UnfreezeTokenParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_UNFREEZE_TOKEN_PARAMS_H_
