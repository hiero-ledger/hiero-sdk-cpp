// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TRANSFER_CRYPTO_PARAMS_H_
#define HIERO_TCK_CPP_TRANSFER_CRYPTO_PARAMS_H_

#include "account/params/transfer/TransferParams.h"
#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <optional>
#include <vector>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for a `transferCrypto` JSON-RPC method call.
 */
struct TransferCryptoParams
{
  /**
   * The desired key for the account.
   */
  std::optional<std::vector<TransferParams>> mTransfers;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert TransferCryptoParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::TransferCryptoParams>
{
  /**
   * Convert a JSON object to a TransferCryptoParams.
   *
   * @param jsonFrom The JSON object with which to fill the TransferCryptoParams.
   * @param params   The TransferCryptoParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::TransferCryptoParams& params)
  {
    params.mTransfers = Hiero::TCK::getOptionalJsonParameter<std::vector<Hiero::TCK::AccountService::TransferParams>>(
      jsonFrom, "mTransfers");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_TRANSFER_CRYPTO_PARAMS_H_
