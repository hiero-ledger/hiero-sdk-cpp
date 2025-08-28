// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CREATE_FILE_PARAMS_H_
#define HIERO_TCK_CPP_CREATE_FILE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::FileService
{
/**
 * Struct to hold the arguments for a `createFile` JSON-RPC method call.
 */
struct CreateFileParams
{
  /**
   * The keys that must sign when mutating the new file.
   */
  std::optional<std::vector<std::string>> mKeys;

  /**
   * The contents of the new file.
   */
  std::optional<std::string> mContents;

  /**
   * The memo for the new file.
   */
  std::optional<std::string> mFileMemo;

  /**
   * The time at which the new file will expire.
   */
  std::optional<std::string> mExpirationTime;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::FileService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CreateFileParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::FileService::CreateFileParams>
{
  static void from_json(const json& jsonFrom, Hiero::TCK::FileService::CreateFileParams& params)
  {
    params.mKeys = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "keys");
    params.mContents = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contents");
    params.mFileMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "fileMemo");
    params.mExpirationTime = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "expirationTime");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CREATE_FILE_PARAMS_H_