// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_ASSOCIATE_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_ASSOCIATE_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for an `associateToken` JSON-RPC method call.
 */
struct AssociateTokenParams
{
  /**
   * The ID of the account with which to associate the tokens.
   */
  std::optional<std::string> mAccountId;

  /**
   * The IDs of the tokens to associate.
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
 * JSON serializer template specialization required to convert AssociateTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::AssociateTokenParams>
{
  /**
   * Convert a JSON object to a AssociateTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the AssociateTokenParams.
   * @param params   The AssociateTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::AssociateTokenParams& params)
  {
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mTokenIds = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "tokenIds");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_ASSOCIATE_TOKEN_PARAMS_H_
