// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_DISSOCIATE_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_DISSOCIATE_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `dissociateToken` JSON-RPC method call.
 */
struct DissociateTokenParams
{
  /**
   * The ID of the account from which to dissociate the token.
   */
  std::optional<std::string> mAccountId;

  /**
   * The IDs of the tokens to dissociate.
   */
  std::optional<std::vector<std::string>> mTokenIds;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert DissociateTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::DissociateTokenParams>
{
  /**
   * Convert a JSON object to a DissociateTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the DissociateTokenParams.
   * @param params   The DissociateTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::DissociateTokenParams& params)
  {
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mTokenIds = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "tokenIds");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_DISSOCIATE_TOKEN_PARAMS_H_
