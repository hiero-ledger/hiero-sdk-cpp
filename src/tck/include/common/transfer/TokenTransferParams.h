// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOKEN_TRANSFER_PARAMS_H_
#define HIERO_TCK_CPP_TOKEN_TRANSFER_PARAMS_H_

#include "json/JsonUtils.h"

#include <cstdint>
#include <optional>
#include <string>

namespace Hiero::TCK
{
/**
 * Struct that contains the parameters of a token transfer.
 */
struct TokenTransferParams
{
  /**
   * The ID of the account associated with the transfer.
   */
  std::string mAccountId;

  /**
   * The ID of the token associated with the transfer.
   */
  std::string mTokenId;

  /**
   * The amount of token to be transferred.
   */
  std::string mAmount;

  /**
   * The decimals of the token to be transferred.
   */
  std::optional<uint32_t> mDecimals;
};

} // namespace Hiero::TCK

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert TokenTransferParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenTransferParams>
{
  /**
   * Convert a JSON object to a TokenTransferParams.
   *
   * @param jsonFrom The JSON object with which to fill the TokenTransferParams.
   * @param params   The TokenTransferParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenTransferParams& params)
  {
    params.mAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "accountId");
    params.mTokenId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mAmount = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "amount");
    params.mDecimals = Hiero::TCK::getOptionalJsonParameter<uint32_t>(jsonFrom, "decimals");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_TOKEN_TRANSFER_PARAMS_H_
