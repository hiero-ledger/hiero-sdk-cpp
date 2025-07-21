// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOKEN_REJECT_PARAMS_H_
#define HIERO_TCK_CPP_TOKEN_REJECT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `tokenReject` JSON-RPC method call.
 */
struct RejectTokenParams
{
  /**
   * The ID of the account rejecting tokens.
   */
  std::optional<std::string> mOwnerAccountId;

  /**
   * The IDs of the fungible tokens to reject.
   */
  std::optional<std::vector<std::string>> mFungibleTokenIds;

  /**
   * The IDs of the NFTs to reject (format: <shard>.<realm>.<num>/<serialNum>).
   */
  std::optional<std::vector<std::string>> mNftIds;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert RejectTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::RejectTokenParams>
{
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::RejectTokenParams& params)
  {
    params.mOwnerAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "ownerAccountId");
    params.mFungibleTokenIds =
      Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "fungibleTokenIds");
    params.mNftIds = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "nftIds");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_TOKEN_REJECT_PARAMS_H_