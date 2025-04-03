// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_AIRDROP_TOKEN_PARAMS_H_
#define HIERO_TCK_CPP_AIRDROP_TOKEN_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "common/transfer/TransferParams.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonUtils.h"

#include <optional>
#include <vector>

namespace Hiero::TCK::TokenService
{
/**
 * Struct to hold the arguments for a `airdropToken` JSON-RPC method call.
 */
struct AirdropTokenParams
{
  /**
   * The airdrop information.
   */
  std::optional<std::vector<TransferParams>> mTokenTransfers;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TokenService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert AirdropTokenParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TokenService::AirdropTokenParams>
{
  /**
   * Convert a JSON object to a AirdropTokenParams.
   *
   * @param jsonFrom The JSON object with which to fill the AirdropTokenParams.
   * @param params   The AirdropTokenParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TokenService::AirdropTokenParams& params)
  {
    params.mTokenTransfers =
      Hiero::TCK::getOptionalJsonParameter<std::vector<Hiero::TCK::TransferParams>>(jsonFrom, "tokenTransfers");

    // Hbar transfer information is not allowed and calls for an INVALID_PARAMS response.
    if (params.mTokenTransfers.has_value())
    {
      for (const Hiero::TCK::TransferParams& transfer : params.mTokenTransfers.value())
      {
        if (transfer.mHbar.has_value())
        {
          throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                             "invalid parameters: Hbar transfers are NOT allowed as part of a token"
                                             "airdrop.");
        }
      }
    }

    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_AIRDROP_TOKEN_PARAMS_H_
