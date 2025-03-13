// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_WIPE_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_WIPE_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `wipeToken` JSON-RPC method call.
 */
struct WipeTokenParams
{
  /**
   * The ID of the token to wipe.
   */
  std::optional<std::string> mTokenId;

  /**
   * The ID of the account from which to wipe the tokens.
   */
  std::optional<std::string> mAccountId;

  /**
   * The amount of fungible tokens to wipe.
   */
  std::optional<std::string> mAmount;

  /**
   * The serial numbers of the NFTs to wipe.
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
 * JSON serializer template specialization required to convert WipeTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::WipeTokenParams>
{
  /**
   * Convert a JSON object to a WipeTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the WipeTokenParams.
   * @param params   The WipeTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::WipeTokenParams& params)
  {
    params.mTokenId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mAmount = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "amount");
    params.mSerialNumbers = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "serialNumbers");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_WIPE_TOKEN_PARAMS_H_
