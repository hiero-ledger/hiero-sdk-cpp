// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_HBAR_TRANSFER_PARAMS_H_
#define HIERO_TCK_CPP_HBAR_TRANSFER_PARAMS_H_

#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"
#include "json/JsonUtils.h"

#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK
{
/**
 * Struct that contains the parameters of an Hbar transfer.
 */
struct HbarTransferParams
{
  /**
   * REQUIRED if mEvmAddress is not provided. The ID of the account associated with the transfer.
   */
  std::optional<std::string> mAccountId;

  /**
   * REQUIRED if mAccountId is not provided. The ID of the account associated with the transfer.
   */
  std::optional<std::string> mEvmAddress;

  /**
   * The amount of Hbar transferred (in tinybars).
   */
  std::string mAmount;
};

} // namespace Hiero::TCK

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert HbarTransferParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::HbarTransferParams>
{
  /**
   * Convert a JSON object to a HbarTransferParams.
   *
   * @param jsonFrom The JSON object with which to fill the HbarTransferParams.
   * @param params   The HbarTransferParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::HbarTransferParams& params)
  {
    params.mAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "accountId");
    params.mEvmAddress = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "evmAddress");
    params.mAmount = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "amount");

    if ((params.mAccountId.has_value() && params.mEvmAddress.has_value()) ||
        (!params.mAccountId.has_value() && !params.mEvmAddress.has_value()))
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: only one of accountId or evmAddress SHALL be provided.");
    }
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_HBAR_TRANSFER_PARAMS_H_
