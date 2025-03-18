// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TRANSFER_PARAMS_H_
#define HIERO_TCK_CPP_TRANSFER_PARAMS_H_

#include "account/params/transfer/HbarTransferParams.h"
#include "account/params/transfer/NftTransferParams.h"
#include "account/params/transfer/TokenTransferParams.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonUtils.h"

#include <optional>

namespace Hiero::TCK::AccountService
{
/**
 * Struct that contains the parameters of a transfer.
 */
struct TransferParams
{
  /**
   * REQUIRED if mToken and mNft are not provided. The parameters of the Hbar transfer.
   */
  std::optional<HbarTransferParams> mHbar;

  /**
   * REQUIRED if mHbar and mNft are not provided. The parameters of the token transfer.
   */
  std::optional<TokenTransferParams> mToken;

  /**
   * REQUIRED if hbar and token are not provided. The parameters of the NFT transfer.
   */
  std::optional<NftTransferParams> mNft;

  /**
   * Is this transfer an approved transfer?
   */
  std::optional<bool> mApproved;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert TransferParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::TransferParams>
{
  /**
   * Convert a JSON object to a TransferParams.
   *
   * @param jsonFrom The JSON object with which to fill the TransferParams.
   * @param params   The TransferParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::TransferParams& params)
  {
    params.mHbar =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::AccountService::HbarTransferParams>(jsonFrom, "hbar");
    params.mToken =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::AccountService::TokenTransferParams>(jsonFrom, "token");
    params.mNft = Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::AccountService::NftTransferParams>(jsonFrom, "nft");
    params.mApproved = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "approved");

    // Only one allowance type should be allowed.
    const bool hasOnlyHbar = params.mHbar.has_value() && !params.mToken.has_value() && !params.mNft.has_value();
    const bool hasOnlyToken = !params.mHbar.has_value() && params.mToken.has_value() && !params.mNft.has_value();
    const bool hasOnlyNft = !params.mHbar.has_value() && !params.mToken.has_value() && params.mNft.has_value();
    if (!hasOnlyHbar && !hasOnlyToken && !hasOnlyNft)
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: only one type of transfer SHALL be provided.");
    }
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_TRANSFER_PARAMS_H_
