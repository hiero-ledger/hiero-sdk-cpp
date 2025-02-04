// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_NFT_ALLOWANCE_PARAMS_H_
#define HIERO_TCK_CPP_NFT_ALLOWANCE_PARAMS_H_

#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::AccountService
{
/**
 * Struct that contains the parameters of an NFT allowance.
 */
struct NftAllowanceParams
{
  /**
   * The ID of the token being allowanced.
   */
  std::string mTokenId;

  /**
   * The serial numbers of the NFTs to allowance.
   */
  std::optional<std::vector<std::string>> mSerialNumbers;

  /**
   * Should the spender be allowed access to all the owner's NFTs of the specified token ID, current and future?
   */
  std::optional<bool> mApprovedForAll;

  /**
   * The ID of the spender account who already has approvedForAll privileges and is granting an allowance to
   * another account.
   */
  std::optional<std::string> mDelegateSpenderAccountId;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert NftAllowanceParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::NftAllowanceParams>
{
  /**
   * Convert a JSON object to a NftAllowanceParams.
   *
   * @param jsonFrom The JSON object with which to fill the NftAllowanceParams.
   * @param params   The NftAllowanceParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::NftAllowanceParams& params)
  {
    params.mTokenId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mSerialNumbers = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "serialNumbers");
    params.mApprovedForAll = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "approvedForAll");
    params.mDelegateSpenderAccountId =
      Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "delegateSpenderAccountId");

    if (params.mSerialNumbers.has_value() == params.mApprovedForAll.has_value())
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: only one of serialNumbers and approvedForAll MUST be "
                                         "provided.");
    }
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_NFT_ALLOWANCE_PARAMS_H_
