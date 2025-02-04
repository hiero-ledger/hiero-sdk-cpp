// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_DELETE_ACCOUNT_PARAMS_H_
#define HIERO_TCK_CPP_DELETE_ACCOUNT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::AccountService
{
/**
 * Struct to hold the arguments for a `deleteAccount` JSON-RPC method call.
 */
struct DeleteAccountParams
{
  /**
   * The ID of the account to delete.
   */
  std::optional<std::string> mDeleteAccountId;

  /**
   * The ID of the account to which to transfer remaining balances.
   */
  std::optional<std::string> mTransferAccountId;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::AccountService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert DeleteAccountParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::AccountService::DeleteAccountParams>
{
  /**
   * Convert a JSON object to a DeleteAccountParams.
   *
   * @param jsonFrom The JSON object with which to fill the DeleteAccountParams.
   * @param params   The DeleteAccountParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::AccountService::DeleteAccountParams& params)
  {
    params.mDeleteAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "deleteAccountId");
    params.mTransferAccountId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "transferAccountId");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_DELETE_ACCOUNT_PARAMS_H_
