// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CANCEL_AIRDROP_PARAMS_H_
#define HIERO_TCK_CPP_CANCEL_AIRDROP_PARAMS_H_

#include "common/CommonTransactionParams.h"

#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `cancelAirdrop` JSON-RPC method call.
 */
struct CancelAirdropParams
{
  /**
   * The ID of the sender of the airdrop.
   */
  std::string mSenderAccountId;

  /**
   * The ID of the sender receiver of the airdrop.
   */
  std::string mReceiverAccountId;

  /**
   * The ID of the token being airdropped.
   */
  std::string mTokenId;

  /**
   * The serial numbers of the NFTs being airdropped.
   */
  std::optional<std::vector<std::string>> mSerialNumbers;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CancelAirdropParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::CancelAirdropParams>
{
  /**
   * Convert a JSON object to a CancelAirdropParams.
   *
   * @param jsonFrom The JSON object with which to fill the CancelAirdropParams.
   * @param params   The CancelAirdropParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::CancelAirdropParams& params)
  {
    params.mSenderAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "senderAccountId");
    params.mReceiverAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "receiverAccountId");
    params.mTokenId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mSerialNumbers = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "serialNumbers");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_AIRDROP_TOKEN_PARAMS_H_
